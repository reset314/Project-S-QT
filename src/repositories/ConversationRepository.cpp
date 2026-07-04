#include "ConversationRepository.h"
#include "../core/network/HttpClient.h"
#include "../core/storage/Database.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

// ---------------------------------------------------------------------------
// Helper — unwrap envelope when HttpClient passed through the root because
// data was an array (HttpClient only extracts data when it's an object).
// ---------------------------------------------------------------------------

static QJsonValue unwrapData(const QJsonObject &resp) {
    if (resp.contains("success")) {
        return resp.value("data");
    }
    return QJsonValue(resp);
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

ConversationRepository::ConversationRepository(HttpClient *http, Database *db,
                                                 QObject *parent)
    : QObject(parent)
    , http_(http)
    , db_(db)
{
}

// ---------------------------------------------------------------------------
// Server API
// ---------------------------------------------------------------------------

Result<QVector<ConversationDTO>> ConversationRepository::listConversations()
{
    auto result = http_->get("/conversations");
    if (!result)
        return tl::make_unexpected(result.error());

    // GET /conversations returns {success: true, data: [...]}
    // HttpClient can't return an array as QJsonObject, so it passes the root through.
    QJsonValue dv = unwrapData(*result);
    QJsonArray arr;
    if (dv.isArray()) {
        arr = dv.toArray();
    } else if (dv.isObject()) {
        // data was already an object? Check for array inside
        QJsonObject obj = dv.toObject();
        auto innerArr = obj.value("data");
        if (innerArr.isArray())
            arr = innerArr.toArray();
        else if (obj.contains("success"))
            arr = obj.value("data").toArray();
    }

    QVector<ConversationDTO> conversations;
    for (const auto &item : arr) {
        if (item.isObject())
            conversations.append(ConversationDTO::fromJson(item.toObject()));
    }

    // Cache locally
    if (db_) {
        for (const auto &conv : conversations)
            upsertConversation(conv);
    }

    return conversations;
}

Result<ConversationDTO> ConversationRepository::createConversation(
    const QString &aiUserId, const QString &title)
{
    QJsonObject body;
    body["ai_user_id"] = aiUserId;
    if (!title.isEmpty())
        body["title"] = title;

    auto result = http_->post("/conversations", body);
    if (!result)
        return tl::make_unexpected(result.error());

    ConversationDTO conv = ConversationDTO::fromJson(*result);

    if (db_)
        upsertConversation(conv);

    return conv;
}

Result<void> ConversationRepository::endConversation(const QString &conversationId,
                                                      const QString &aiUserId)
{
    QJsonObject body;
    body["conversation_id"] = conversationId;
    body["ai_user_id"] = aiUserId;

    auto result = http_->post("/end", body);
    if (!result)
        return tl::make_unexpected(result.error());

    return {};
}

// ---------------------------------------------------------------------------
// Local DB
// ---------------------------------------------------------------------------

QVector<ConversationDTO> ConversationRepository::getCachedConversations()
{
    QVector<ConversationDTO> result;
    if (!db_)
        return result;

    QSqlQuery q(db_->db());
    q.prepare("SELECT * FROM conversations ORDER BY updated_at DESC");
    if (!q.exec()) {
        qWarning() << "getCachedConversations failed:" << q.lastError().text();
        return result;
    }

    while (q.next()) {
        auto rec = q.record();
        QJsonObject obj;
        for (int i = 0; i < rec.count(); ++i) {
            obj[rec.fieldName(i)] = QJsonValue::fromVariant(rec.value(i));
        }
        result.append(ConversationDTO::fromJson(obj));
    }
    return result;
}

void ConversationRepository::upsertConversation(const ConversationDTO &conv)
{
    if (!db_)
        return;

    QSqlQuery q(db_->db());
    // conversations table uses id as TEXT primary key (server UUID)
    q.prepare(R"(
        INSERT OR REPLACE INTO conversations
            (id, ai_user_id, title, last_message_preview, created_at, updated_at)
        VALUES (?, ?, ?, ?, ?, ?)
    )");
    q.addBindValue(QString::fromStdString(conv.id));
    q.addBindValue(conv.aiUserId ? QString::fromStdString(*conv.aiUserId) : QString{});
    q.addBindValue(QString::fromStdString(conv.title));
    q.addBindValue(conv.lastMessagePreview ? QString::fromStdString(*conv.lastMessagePreview) : QVariant{});
    q.addBindValue(QString::fromStdString(conv.createdAt));
    q.addBindValue(QString::fromStdString(conv.updatedAt));

    if (!q.exec())
        qWarning() << "upsertConversation failed:" << q.lastError().text();
}

std::optional<QString> ConversationRepository::getAiUserIdForConversation(
    const QString &conversationId)
{
    if (!db_)
        return std::nullopt;

    QSqlQuery q(db_->db());
    q.prepare("SELECT ai_user_id FROM conversations WHERE id = ?");
    q.addBindValue(conversationId);
    if (!q.exec()) {
        qWarning() << "getAiUserIdForConversation failed:" << q.lastError().text();
        return std::nullopt;
    }

    if (!q.next())
        return std::nullopt;

    QString val = q.value(0).toString();
    if (val.isEmpty())
        return std::nullopt;
    return val;
}
