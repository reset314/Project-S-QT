#include "ChatRepository.h"
#include "../core/network/HttpClient.h"
#include "../core/storage/Database.h"
#include "../core/utils/JsonHelper.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QMap>
#include <QDateTime>

// ---------------------------------------------------------------------------
// Response struct factories
// ---------------------------------------------------------------------------

ChatSendResponse ChatSendResponse::fromJson(const QJsonObject &obj) {
    ChatSendResponse r;
    r.userMessageId = JsonHelper::getString(obj, "user_message_id");
    r.conversationId = JsonHelper::getString(obj, "conversation_id");
    r.conversationReplaced = JsonHelper::getBool(obj, "conversation_replaced", false);
    return r;
}

ChatMediaResponse ChatMediaResponse::fromJson(const QJsonObject &obj) {
    ChatMediaResponse r;
    r.content = JsonHelper::getString(obj, "content");
    r.conversationId = JsonHelper::getString(obj, "conversation_id");
    r.messageId = JsonHelper::getString(obj, "message_id");
    r.msgType = JsonHelper::getString(obj, "msg_type", "text");
    r.conversationReplaced = JsonHelper::getBool(obj, "conversation_replaced", false);
    return r;
}

SyncResult SyncResult::fromJson(const QJsonObject &obj) {
    SyncResult r;
    auto messagesVal = obj.value("messages");
    if (messagesVal.isArray()) {
        for (const auto &item : messagesVal.toArray()) {
            if (item.isObject())
                r.messages.append(MessageDTO::fromJson(item.toObject()));
        }
    }
    r.hasMore = JsonHelper::getBool(obj, "has_more", false);
    r.syncTimestamp = JsonHelper::getString(obj, "timestamp",
        QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString());
    return r;
}

// ---------------------------------------------------------------------------
// Helper — some endpoints return arrays in data, which HttpClient passes
// through as the raw envelope {success, data}.  Detect and unwrap.
// ---------------------------------------------------------------------------

static QJsonValue unwrapResponse(const QJsonObject &resp) {
    if (resp.contains("success")) {
        return resp.value("data");
    }
    return QJsonValue(resp);
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

ChatRepository::ChatRepository(HttpClient *http, Database *db, QObject *parent)
    : QObject(parent)
    , http_(http)
    , db_(db)
{
}

// ---------------------------------------------------------------------------
// Server API
// ---------------------------------------------------------------------------

Result<ChatSendResponse> ChatRepository::sendMessage(const QString &aiUserId,
                                                      const QString &content,
                                                      const QString &conversationId)
{
    QJsonObject body;
    body["ai_user_id"] = aiUserId;
    body["content"] = content;
    if (!conversationId.isEmpty())
        body["conversation_id"] = conversationId;

    auto result = http_->post("/chat", body);
    if (!result)
        return tl::make_unexpected(result.error());

    return ChatSendResponse::fromJson(*result);
}

Result<ChatMediaResponse> ChatRepository::sendFormDataMessage(
    const QString &aiUserId, const QString &filePath,
    const QString &content, const QString &conversationId,
    const QString &msgType)
{
    QMap<QString, QString> fields;
    fields["ai_user_id"] = aiUserId;
    if (!conversationId.isEmpty())
        fields["conversation_id"] = conversationId;
    if (!content.isEmpty())
        fields["content"] = content;
    if (!msgType.isEmpty())
        fields["msg_type"] = msgType;

    auto result = http_->postFormData("/chat", fields, filePath);
    if (!result)
        return tl::make_unexpected(result.error());

    return ChatMediaResponse::fromJson(*result);
}

Result<QVector<MessageDTO>> ChatRepository::getHistory(const QString &aiUserId,
                                                        const QString &beforeId,
                                                        int limit)
{
    QUrlQuery params;
    params.addQueryItem("limit", QString::number(limit));
    if (!beforeId.isEmpty())
        params.addQueryItem("before_id", beforeId);

    auto result = http_->get(QString("/aiusers/%1/messages").arg(aiUserId), params);
    if (!result)
        return tl::make_unexpected(result.error());

    QJsonValue data = unwrapResponse(*result);
    // data may be the unwrapped object containing {messages, has_more}
    QJsonObject dataObj = data.isObject() ? data.toObject() : *result;

    QVector<MessageDTO> messages;
    auto messagesVal = dataObj.value("messages");
    if (messagesVal.isArray()) {
        for (const auto &item : messagesVal.toArray()) {
            if (item.isObject())
                messages.append(MessageDTO::fromJson(item.toObject()));
        }
    }

    // Persist fetched messages locally
    if (db_) {
        int upserted = 0;
        for (const auto &msg : messages) {
            db_->upsertMessage(msg, aiUserId);
            upserted++;
        }
        qDebug() << "ChatRepository::getHistory: upserted" << upserted
                 << "messages for aiUserId" << aiUserId;
    } else {
        qWarning() << "ChatRepository::getHistory: db_ is NULL — messages NOT persisted!";
    }

    return messages;
}

Result<SyncResult> ChatRepository::syncMessages(const QString &aiUserId,
                                                  const QString &since,
                                                  const QString &afterId,
                                                  int limit)
{
    QUrlQuery params;
    params.addQueryItem("since", since);
    params.addQueryItem("limit", QString::number(limit));
    if (!afterId.isEmpty())
        params.addQueryItem("after_id", afterId);

    auto result = http_->get(QString("/aiusers/%1/messages").arg(aiUserId), params);
    if (!result)
        return tl::make_unexpected(result.error());

    QJsonValue data = unwrapResponse(*result);
    QJsonObject dataObj = data.isObject() ? data.toObject() : *result;
    SyncResult sync = SyncResult::fromJson(dataObj);

    // Merge into local DB
    if (db_) {
        for (const auto &msg : sync.messages)
            db_->upsertMessage(msg, aiUserId);
    }

    return sync;
}

Result<MessageDTO> ChatRepository::revokeMessage(const QString &messageId)
{
    auto result = http_->post(QString("/messages/%1/revoke").arg(messageId));
    if (!result)
        return tl::make_unexpected(result.error());

    MessageDTO msg = MessageDTO::fromJson(*result);

    // Update local DB
    if (db_ && !msg.serverId.empty()) {
        QVariantMap values;
        if (msg.revokedAt)
            values["revoked_at"] = msg.revokedAt->toString(Qt::ISODate);
        db_->updateMessage(QString::fromStdString(msg.clientUuid), values);
    }

    return msg;
}

Result<QJsonObject> ChatRepository::rollbackMessages(const QString &afterMessageId)
{
    QUrlQuery params;
    params.addQueryItem("after_message_id", afterMessageId);
    // HttpClient::post doesn't support query params directly, so use the root envelope approach
    // Actually, we need to send it as a POST with query params.
    // HttpClient::post() only takes path + body. Let's work around by appending query string.
    // Re-reading HttpClient: post() calls sendWithRetry which calls doRequest with empty params.
    // We'll use get() instead... no, rollback is POST.
    //
    // The backend endpoint is POST /messages/rollback?after_message_id=...
    // We'll construct the path manually since HttpClient::post doesn't support QUrlQuery.

    QString path = QString("/messages/rollback?after_message_id=%1").arg(afterMessageId);
    auto result = http_->post(path, QJsonObject{});
    if (!result)
        return tl::make_unexpected(result.error());

    return *result;
}

// ---------------------------------------------------------------------------
// Local DB pass-through
// ---------------------------------------------------------------------------

int64_t ChatRepository::insertMessage(const MessageDTO &msg, const QString &aiUserId) {
    if (!db_) return -1;
    return db_->insertMessage(msg, aiUserId);
}

void ChatRepository::upsertMessage(const MessageDTO &msg, const QString &aiUserId) {
    if (db_) db_->upsertMessage(msg, aiUserId);
}

void ChatRepository::updateMessage(const QString &clientUuid, const QVariantMap &values) {
    if (db_) db_->updateMessage(clientUuid, values);
}

QVector<MessageDTO> ChatRepository::getLocalMessages(const QString &aiUserId, int limit) {
    if (!db_) return {};
    return db_->getLocalMessages(aiUserId, limit);
}

QVector<MessageDTO> ChatRepository::getLocalMessagesBefore(const QString &aiUserId,
                                                            const QString &beforeTimestamp,
                                                            int limit) {
    if (!db_) return {};
    return db_->getLocalMessagesBefore(aiUserId, beforeTimestamp, limit);
}

void ChatRepository::deleteMessagesSince(const QString &aiUserId, const QString &since) {
    if (db_) db_->deleteMessagesSince(aiUserId, since);
}

void ChatRepository::deleteStaleUnsynced(const QString &aiUserId, int staleMinutes) {
    if (db_) db_->deleteStaleUnsynced(aiUserId, staleMinutes);
}

void ChatRepository::markAsRead(const QString &aiUserId) {
    if (db_) db_->markAsRead(aiUserId);
}

bool ChatRepository::hasLocalMessages(const QString &aiUserId) {
    if (!db_) return false;
    return db_->hasLocalMessages(aiUserId);
}

std::optional<QString> ChatRepository::getSyncTime(const QString &aiUserId) {
    if (!db_) return std::nullopt;
    return db_->getSyncTime(aiUserId);
}

void ChatRepository::setSyncTime(const QString &aiUserId, const QString &time) {
    if (db_) db_->setSyncTime(aiUserId, time);
}
