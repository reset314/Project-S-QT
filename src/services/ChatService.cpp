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
    // Wire ChatStreamClient signals to private slots so we can map
    // conversationId → aiUserId and re-emit enriched signals.
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

    // Track conversationId → aiUserId mapping for WS signal routing
    if (!conversationId.isEmpty())
        convToAiUser_[conversationId] = aiUserId;

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

    if (!conversationId.isEmpty())
        convToAiUser_[conversationId] = aiUserId;

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
        values["server_id"] = static_cast<qint64>(
            QString::fromStdString(resp.messageId).toLongLong());
        values["is_complete"] = true;
        repo_->updateMessage(clientUuid, values);
    }

    // Handle conversation replacement
    if (resp.conversationReplaced && !resp.conversationId.empty()) {
        convToAiUser_[QString::fromStdString(resp.conversationId)] = aiUserId;
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
        values["server_id"] = static_cast<qint64>(
            QString::fromStdString(resp.userMessageId).toLongLong());
        for (const auto &uuid : clientUuids) {
            repo_->updateMessage(uuid, values);
        }
    }

    // Track conversation mapping
    if (!resp.conversationId.empty()) {
        convToAiUser_[QString::fromStdString(resp.conversationId)] = aiUserId;

        // Handle conversation replacement
        if (resp.conversationReplaced) {
            emit conversationReplaced(QString::fromStdString(resp.conversationId));
        }
    }

    // Start watchdog — we now expect WS stream events
    resetWatchdog(aiUserId);
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

QString ChatService::resolveAiUserId(const QString &conversationId) const
{
    return convToAiUser_.value(conversationId);
}

void ChatService::onStreamInit(const QString &conversationId,
                               const QString &messageId,
                               const QString &timestamp)
{
    Q_UNUSED(timestamp)
    QString aiUserId = resolveAiUserId(conversationId);

    // Reset watchdog on every stream event so long-running streams don't
    // time out prematurely.
    resetWatchdog(aiUserId);

    // If aiUserId is empty we still emit the signal — upstream can try to
    // resolve it from the conversationId or learn it from stream_done.

    emit streamInitReceived(aiUserId, conversationId, messageId);
}

void ChatService::onStreamChunk(const QString &conversationId,
                                const QString &chunk)
{
    QString aiUserId = resolveAiUserId(conversationId);

    // Reset watchdog on every chunk so long-running streams don't time out.
    resetWatchdog(aiUserId);

    emit streamChunkReceived(aiUserId, chunk);
}

void ChatService::onStreamDone(const QString &conversationId,
                               const QString &messageId,
                               const QString &content)
{
    QString aiUserId = resolveAiUserId(conversationId);

    // Cancel watchdog unconditionally — stopWatchdog safely handles empty keys.
    // Must come before any aiUserId.isEmpty() guard so the timer is always
    // cleaned up even when the aiUserId cannot be resolved.
    cancelWatchdog(aiUserId);

    emit streamDoneReceived(aiUserId, messageId, content);
}

void ChatService::onStreamError(const QString &conversationId,
                                const QString &code,
                                const QString &message)
{
    QString aiUserId = resolveAiUserId(conversationId);

    // Cancel watchdog unconditionally — stopWatchdog safely handles empty keys.
    // Must come before any aiUserId.isEmpty() guard so the timer is always
    // cleaned up even when the aiUserId cannot be resolved.
    cancelWatchdog(aiUserId);

    emit streamErrorReceived(aiUserId, code, message);
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
    msg.clientUuid = UuidGenerator::generate();
    msg.aiUserId = aiUserId.toStdString();
    msg.conversationId = conversationId.toStdString();
    msg.senderType = senderType.toStdString();
    msg.msgType = msgType.toStdString();

    // Wrap plain text in the canonical content JSON envelope
    QJsonObject contentObj;
    contentObj[QStringLiteral("response")] = content;
    msg.content = contentObj;

    msg.isComplete = false; // Will be set to true when server confirms
    msg.timestamp = QDateTime::currentDateTimeUtc()
                        .toString(Qt::ISODate).toStdString();

    repo_->insertMessage(msg, aiUserId);
    return QString::fromStdString(msg.clientUuid);
}
