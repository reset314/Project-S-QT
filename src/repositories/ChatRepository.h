#pragma once
#include <QObject>
#include <QVector>
#include <QVariantMap>
#include "../core/utils/ApiError.h"
#include "../core/models/MessageDTO.h"

class HttpClient;
class Database;

// ---------------------------------------------------------------------------
// Response structs
// ---------------------------------------------------------------------------

struct ChatSendResponse {
    std::string userMessageId;
    std::string conversationId;
    bool conversationReplaced = false;

    static ChatSendResponse fromJson(const QJsonObject &obj);
};

struct ChatMediaResponse {
    std::string content;
    std::string conversationId;
    std::string messageId;
    std::string msgType = "text";
    bool conversationReplaced = false;

    static ChatMediaResponse fromJson(const QJsonObject &obj);
};

struct SyncResult {
    QVector<MessageDTO> messages;
    bool hasMore = false;
    std::string syncTimestamp;

    static SyncResult fromJson(const QJsonObject &obj);
};

// ---------------------------------------------------------------------------
// ChatRepository
// ---------------------------------------------------------------------------

class ChatRepository : public QObject {
    Q_OBJECT
public:
    explicit ChatRepository(HttpClient *http, Database *db = nullptr,
                            QObject *parent = nullptr);

    // -- Server API ----------------------------------------------------------

    /// POST /chat  (JSON body — text messages)
    Result<ChatSendResponse> sendMessage(const QString &aiUserId,
                                         const QString &content,
                                         const QString &conversationId = {});

    /// POST /chat  (multipart form-data — media messages)
    Result<ChatMediaResponse> sendFormDataMessage(const QString &aiUserId,
                                                   const QString &filePath,
                                                   const QString &content = {},
                                                   const QString &conversationId = {},
                                                   const QString &msgType = {});

    /// GET /aiusers/{id}/messages?before_id=&limit=
    Result<QVector<MessageDTO>> getHistory(const QString &aiUserId,
                                            const QString &beforeId = {},
                                            int limit = 50);

    /// GET /aiusers/{id}/messages?since=&after_id=&limit=  (incremental sync)
    Result<SyncResult> syncMessages(const QString &aiUserId,
                                     const QString &since,
                                     const QString &afterId = {},
                                     int limit = 50);

    /// POST /messages/{id}/revoke
    Result<MessageDTO> revokeMessage(const QString &messageId);

    /// POST /messages/rollback?after_message_id=
    Result<QJsonObject> rollbackMessages(const QString &afterMessageId);

    // -- Local DB pass-through (delegates to Database) -----------------------

    int64_t insertMessage(const MessageDTO &msg, const QString &aiUserId);
    void upsertMessage(const MessageDTO &msg, const QString &aiUserId);
    QVector<MessageDTO> getLocalMessages(const QString &aiUserId, int limit = 200);
    QVector<MessageDTO> getLocalMessagesBefore(const QString &aiUserId,
                                                const QString &beforeTimestamp,
                                                int limit = 50);
    void updateMessage(const QString &clientUuid, const QVariantMap &values);
    void deleteMessagesSince(const QString &aiUserId, const QString &since);
    void deleteStaleUnsynced(const QString &aiUserId, int staleMinutes = 30);
    void markAsRead(const QString &aiUserId);
    bool hasLocalMessages(const QString &aiUserId);
    std::optional<QString> getSyncTime(const QString &aiUserId);
    void setSyncTime(const QString &aiUserId, const QString &time);

private:
    HttpClient *http_;
    Database *db_;
};
