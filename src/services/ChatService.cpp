#include "ChatService.h"
#include "SyncService.h"
#include "../core/utils/UuidGenerator.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

ChatService::ChatService(ChatRepository *repo,
                         ChatStreamClient *wsClient,
                         SyncService *syncService,
                         QObject *parent)
    : QObject(parent)
    , repo_(repo)
    , wsClient_(wsClient)
    , syncService_(syncService)
{
    // Wire ChatStreamClient signals to private slots for re-emitting
    // enriched signals with aiUserId context.
    if (wsClient_) {
        connect(wsClient_, &ChatStreamClient::streamInit,
                this, &ChatService::onStreamInit);
        connect(wsClient_, &ChatStreamClient::streamChunk,
                this, &ChatService::onStreamChunk);
        connect(wsClient_, &ChatStreamClient::streamDone,
                this, &ChatService::onStreamDone);
        connect(wsClient_, &ChatStreamClient::streamError,
                this, &ChatService::onStreamError);
    }
}

// ---------------------------------------------------------------------------
// Public API — sendMessage
// ---------------------------------------------------------------------------

void ChatService::sendMessage(const QString &aiUserId,
                              const QString &content,
                              const QString &conversationId)
{
    // 1. Optimistic insert into local DB
    QString clientUuid = optimisticInsert(aiUserId, content, conversationId,
                                          QStringLiteral("user"),
                                          QStringLiteral("text"));

    // 2. Add to batch
    BatchState &batch = batchers_[aiUserId];

    // Store the conversationId from the first message in the batch
    if (batch.clientUuids.isEmpty())
        batch.conversationId = conversationId;

    batch.clientUuids.append(clientUuid);
    batch.contents.append(content);

    // 3. Start / restart timers
    if (!batch.cooldownTimer) {
        batch.cooldownTimer = new QTimer(this);
        batch.cooldownTimer->setSingleShot(true);
        connect(batch.cooldownTimer, &QTimer::timeout, this, [this, aiUserId]() {
            flushBatch(aiUserId);
        });
    }
    if (!batch.maxWaitTimer) {
        batch.maxWaitTimer = new QTimer(this);
        batch.maxWaitTimer->setSingleShot(true);
        connect(batch.maxWaitTimer, &QTimer::timeout, this, [this, aiUserId]() {
            flushBatch(aiUserId);
        });
    }

    // Restart both timers
    batch.cooldownTimer->start(sendCooldownMs_);

    // Only start max-wait if not already running (first message in batch)
    if (!batch.maxWaitTimer->isActive())
        batch.maxWaitTimer->start(sendMaxWaitMs_);

    // Build MessageDTO for incremental model update
    MessageDTO optimisticMsg;
    optimisticMsg.clientUuid = clientUuid.toStdString();
    optimisticMsg.aiUserId = aiUserId.toStdString();
    optimisticMsg.senderType = "user";
    optimisticMsg.msgType = "text";
    QJsonObject contentObj;
    contentObj[QStringLiteral("response")] = content;
    optimisticMsg.content = QString::fromUtf8(QJsonDocument(contentObj).toJson(QJsonDocument::Compact)).toStdString();
    optimisticMsg.isComplete = false;
    optimisticMsg.timestamp = QDateTime::currentDateTimeUtc()
                                  .toString(Qt::ISODate).toStdString();

    // Incremental model update — no flicker
    emit messageAppended(aiUserId, optimisticMsg);
    // Also notify for full sync (DB persistence)
    emit messagesChanged(aiUserId);
}

// ---------------------------------------------------------------------------
// Public API — sendMediaMessage
// ---------------------------------------------------------------------------

void ChatService::sendMediaMessage(const QString &aiUserId,
                                   const QString &filePath,
                                   const QString &content,
                                   const QString &msgType,
                                   const QString &conversationId)
{
    // Optimistic insert — mark as incomplete until server confirms
    QString effectiveMsgType = msgType.isEmpty() ? QStringLiteral("image") : msgType;
    QString clientUuid = optimisticInsert(aiUserId, content, conversationId,
                                          QStringLiteral("user"), effectiveMsgType);

    // Media messages bypass batching — send immediately
    auto result = repo_->sendFormDataMessage(aiUserId, filePath, content,
                                             conversationId, effectiveMsgType);
    if (!result) {
        qWarning() << "ChatService::sendMediaMessage failed:"
                   << QString::fromStdString(result.error().code)
                   << QString::fromStdString(result.error().message);

        // Emit as a stream error since there is no WS for media
        emit streamErrorReceived(aiUserId,
                                 QString::fromStdString(result.error().code),
                                 QString::fromStdString(result.error().message));
        return;
    }

    const auto &resp = *result;

    // Backfill serverId
    if (!resp.messageId.empty()) {
        QVariantMap values;
        values["server_id"] = QString::fromStdString(resp.messageId);
        values["is_complete"] = true;
        repo_->updateMessage(clientUuid, values);
    }

    // Handle conversation replacement
    if (resp.conversationReplaced && !resp.conversationId.empty()) {
        emit conversationReplaced(QString::fromStdString(resp.conversationId));
    }

    // Media messages have no WS stream, so start watchdog anyway in case
    // the server sends a follow-up via WS
    resetWatchdog(aiUserId);
}

// ---------------------------------------------------------------------------
// Public API — cancelAllStreams / cancelWatchdog
// ---------------------------------------------------------------------------

void ChatService::cancelAllStreams(const QString &aiUserId)
{
    stopWatchdog(aiUserId);

    // Cancel any pending batch
    if (batchers_.contains(aiUserId)) {
        BatchState &batch = batchers_[aiUserId];
        if (batch.cooldownTimer) {
            batch.cooldownTimer->stop();
            delete batch.cooldownTimer;
            batch.cooldownTimer = nullptr;
        }
        if (batch.maxWaitTimer) {
            batch.maxWaitTimer->stop();
            delete batch.maxWaitTimer;
            batch.maxWaitTimer = nullptr;
        }
        batchers_.remove(aiUserId);
    }
}

void ChatService::cancelWatchdog(const QString &aiUserId)
{
    stopWatchdog(aiUserId);
}

// ---------------------------------------------------------------------------
// Private — flushBatch
// ---------------------------------------------------------------------------

void ChatService::flushBatch(const QString &aiUserId)
{
    if (!batchers_.contains(aiUserId))
        return;

    BatchState &batch = batchers_[aiUserId];

    // Stop timers
    if (batch.cooldownTimer) {
        batch.cooldownTimer->stop();
        delete batch.cooldownTimer;
        batch.cooldownTimer = nullptr;
    }
    if (batch.maxWaitTimer) {
        batch.maxWaitTimer->stop();
        delete batch.maxWaitTimer;
        batch.maxWaitTimer = nullptr;
    }

    if (batch.clientUuids.isEmpty()) {
        batchers_.remove(aiUserId);
        return;
    }

    // Combine content — join with newlines
    QString combinedContent = batch.contents.join(QStringLiteral("\n"));
    QString conversationId = batch.conversationId;
    QStringList clientUuids = batch.clientUuids;

    // Clear batch state before async work
    batchers_.remove(aiUserId);

    // POST to server
    auto result = repo_->sendMessage(aiUserId, combinedContent, conversationId);

    if (!result) {
        qWarning() << "ChatService::flushBatch failed:"
                   << QString::fromStdString(result.error().code)
                   << QString::fromStdString(result.error().message);

        // Emit stream error so callers can handle
        emit streamErrorReceived(aiUserId,
                                 QString::fromStdString(result.error().code),
                                 QString::fromStdString(result.error().message));
        return;
    }

    const auto &resp = *result;

    // Backfill serverId on ALL optimistic messages in the batch.
    // The server returns ONE userMessageId; apply it to every batched UUID
    // so all messages are linked to the confirmed server-side record.
    if (!resp.userMessageId.empty() && !clientUuids.isEmpty()) {
        QVariantMap values;
        values["server_id"] = QString::fromStdString(resp.userMessageId);
        values["is_complete"] = 1;
        for (const auto &uuid : clientUuids) {
            repo_->updateMessage(uuid, values);
        }
    }

    // Handle conversation replacement
    if (resp.conversationReplaced && !resp.conversationId.empty()) {
        emit conversationReplaced(QString::fromStdString(resp.conversationId));
    }

    // Start watchdog — we now expect WS stream events
    resetWatchdog(aiUserId);

    // Notify UI — server confirmed; message may have server_id backfilled
    for (const auto &uuid : clientUuids) {
        emit messageMarkedComplete(aiUserId, uuid,
                                    QString::fromStdString(resp.userMessageId));
    }
    emit messagesChanged(aiUserId);
}

// ---------------------------------------------------------------------------
// Private — watchdog management
// ---------------------------------------------------------------------------

void ChatService::resetWatchdog(const QString &aiUserId)
{
    // Stop existing watchdog if any
    stopWatchdog(aiUserId);

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this, aiUserId]() {
        qWarning() << "ChatService: watchdog expired for" << aiUserId;

        // Trigger sync recovery
        if (syncService_) {
            syncService_->syncMessages(aiUserId);
        }

        // Notify listeners
        emit messageTimedOut(aiUserId);

        watchdogTimers_.remove(aiUserId);
    });

    watchdogTimers_[aiUserId] = timer;
    timer->start(streamTimeoutMs_);
}

void ChatService::stopWatchdog(const QString &aiUserId)
{
    if (watchdogTimers_.contains(aiUserId)) {
        QTimer *timer = watchdogTimers_.take(aiUserId);
        timer->stop();
        delete timer;
    }
}

// ---------------------------------------------------------------------------
// Private — ChatStreamClient signal handlers
// ---------------------------------------------------------------------------

void ChatService::onStreamInit(const QString &aiUserId,
                               const QString &messageId,
                               const QString &timestamp)
{
    qDebug() << "ChatService::onStreamInit" << "aiUserId:" << aiUserId << "msgId:" << messageId;

    if (aiUserId.isEmpty()) { qDebug() << "  -> empty aiUserId, return"; return; }
    qDebug() << "  -> step 1: resetWatchdog";
    resetWatchdog(aiUserId);
    qDebug() << "  -> step 2: check streamingAiMessages_";

    if (!streamingAiMessages_.contains(aiUserId)) {
        qDebug() << "  -> step 3: create aiMsg";
        MessageDTO aiMsg;
        aiMsg.clientUuid = UuidGenerator::generate().toStdString();
        qDebug() << "  -> step 4: set fields";
        aiMsg.aiUserId = aiUserId.toStdString();
        aiMsg.senderType = "ai";
        aiMsg.msgType = "text";
        aiMsg.content = "{}";
        aiMsg.isComplete = false;
        aiMsg.timestamp = timestamp.isEmpty()
            ? QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString()
            : timestamp.toStdString();
        if (!messageId.isEmpty())
            aiMsg.serverId = messageId.toStdString();
        qDebug() << "  -> step 5: insertMessage";

        repo_->insertMessage(aiMsg, aiUserId);
        qDebug() << "  -> step 6: update streamingAiMessages_";
        streamingAiMessages_[aiUserId] = QString::fromStdString(aiMsg.clientUuid);
        qDebug() << "  -> step 7: emit messageAppended";

        emit messageAppended(aiUserId, aiMsg);
    }
    qDebug() << "  -> step 8: emit streamInitReceived";

    emit streamInitReceived(aiUserId, QString(), messageId);
    qDebug() << "  -> step 9: emit messagesChanged";
    emit messagesChanged(aiUserId);
    qDebug() << "  -> DONE";
}

void ChatService::onStreamChunk(const QString &aiUserId,
                                const QString &chunk)
{
    resetWatchdog(aiUserId);

    QString clientUuid = streamingAiMessages_.value(aiUserId);
    if (!clientUuid.isEmpty()) {
        QVariantMap values;
        QJsonObject contentObj;
        contentObj[QStringLiteral("response")] = chunk;
        values[QStringLiteral("content")]
            = QString(QJsonDocument(contentObj).toJson(QJsonDocument::Compact));
        repo_->updateMessage(clientUuid, values);

        // Incremental model update — update text in-place
        emit messageContentUpdated(aiUserId, clientUuid, contentObj);
    }

    emit streamChunkReceived(aiUserId, chunk);
    emit messagesChanged(aiUserId);
}

void ChatService::onStreamDone(const QString &aiUserId,
                               const QString &messageId,
                               const QString &content)
{
    cancelWatchdog(aiUserId);

    QString clientUuid = streamingAiMessages_.take(aiUserId);
    qDebug() << "ChatService::onStreamDone" << "aiUserId:" << aiUserId << "clientUuid:" << clientUuid
             << "contentLen:" << content.size();
    if (!clientUuid.isEmpty()) {
        QVariantMap values;
        // Parse structured content: if the server returns JSON (starts with {),
        // use it directly; otherwise wrap as {"response": content}.
        // Matches tryParseStructuredContent() + MessageDTO::fromJson.
        QString trimmed = content.trimmed();
        QJsonObject contentObj;
        if (trimmed.startsWith('{')) {
            QJsonDocument doc = QJsonDocument::fromJson(trimmed.toUtf8());
            if (doc.isObject())
                contentObj = doc.object();
            else
                contentObj[QStringLiteral("response")] = content;
        } else {
            contentObj[QStringLiteral("response")] = content;
        }
        values[QStringLiteral("content")]
            = QString(QJsonDocument(contentObj).toJson(QJsonDocument::Compact));
        values[QStringLiteral("is_complete")] = 1;
        if (!messageId.isEmpty())
            values[QStringLiteral("server_id")] = messageId;
        repo_->updateMessage(clientUuid, values);

        // Incremental model update — finalize content and mark complete
        emit messageContentUpdated(aiUserId, clientUuid, contentObj);
        emit messageMarkedComplete(aiUserId, clientUuid, messageId);
    }

    emit streamDoneReceived(aiUserId, messageId, content);
    emit messagesChanged(aiUserId);
}

void ChatService::onStreamError(const QString &aiUserId,
                                const QString &code,
                                const QString &message)
{
    cancelWatchdog(aiUserId);

    QString clientUuid = streamingAiMessages_.take(aiUserId);
    if (!clientUuid.isEmpty()) {
        QVariantMap values;
        values[QStringLiteral("is_complete")] = 1;
        repo_->updateMessage(clientUuid, values);
    }

    emit streamErrorReceived(aiUserId, code, message);
    emit messagesChanged(aiUserId);
}

// ---------------------------------------------------------------------------
// Private — optimisticInsert
// ---------------------------------------------------------------------------

QString ChatService::optimisticInsert(const QString &aiUserId,
                                      const QString &content,
                                      const QString &conversationId,
                                      const QString &senderType,
                                      const QString &msgType)
{
    MessageDTO msg;
    msg.clientUuid = UuidGenerator::generate().toStdString();
    msg.aiUserId = aiUserId.toStdString();
    msg.senderType = senderType.toStdString();
    msg.msgType = msgType.toStdString();

    // Wrap plain text in the canonical content JSON envelope
    QJsonObject contentObj;
    contentObj[QStringLiteral("response")] = content;
    msg.content = QString::fromUtf8(QJsonDocument(contentObj).toJson(QJsonDocument::Compact)).toStdString();

    msg.isComplete = false; // Will be set to true when server confirms
    msg.timestamp = QDateTime::currentDateTimeUtc()
                        .toString(Qt::ISODate).toStdString();

    repo_->insertMessage(msg, aiUserId);
    return QString::fromStdString(msg.clientUuid);
}
