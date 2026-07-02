#pragma once
#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <optional>
#include "../core/utils/ApiError.h"
#include "../core/models/ConversationDTO.h"

class HttpClient;
class Database;

class ConversationRepository : public QObject {
    Q_OBJECT
public:
    explicit ConversationRepository(HttpClient *http, Database *db = nullptr,
                                    QObject *parent = nullptr);

    // -- Server API ----------------------------------------------------------

    /// GET /conversations
    Result<QVector<ConversationDTO>> listConversations();

    /// POST /conversations
    Result<ConversationDTO> createConversation(const QString &aiUserId,
                                                const QString &title = {});

    /// POST /end  (end conversation + generate summary/memory)
    Result<void> endConversation(const QString &conversationId,
                                  const QString &aiUserId);

    // -- Local DB ------------------------------------------------------------

    QVector<ConversationDTO> getCachedConversations();
    void upsertConversation(const ConversationDTO &conv);
    std::optional<QString> getAiUserIdForConversation(const QString &conversationId);

private:
    HttpClient *http_;
    Database *db_;
};
