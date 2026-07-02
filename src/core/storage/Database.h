#pragma once
#include <QSqlDatabase>
#include <QVector>
#include <QHash>
#include <QString>
#include <QVariantMap>
#include <optional>
#include "../models/MessageDTO.h"

class Database {
public:
    Database();
    ~Database();

    bool open(const QString &path);
    QSqlDatabase &db() { return db_; }

    // Messages
    int64_t insertMessage(const MessageDTO &msg, const QString &aiUserId);
    void upsertMessage(const MessageDTO &msg, const QString &aiUserId);
    std::optional<MessageDTO> findByServerId(int64_t serverId);
    std::optional<MessageDTO> findByClientUuid(const QString &clientUuid);
    QVector<MessageDTO> getLocalMessages(const QString &aiUserId, int limit = 200);
    QVector<MessageDTO> getLocalMessagesBefore(const QString &aiUserId,
                                                const QString &beforeTimestamp,
                                                int limit = 50);
    void updateMessage(const QString &clientUuid, const QVariantMap &values);
    void deleteMessage(const QString &clientUuid);
    void deleteMessagesSince(const QString &aiUserId, const QString &since);
    void deleteStaleUnsynced(const QString &aiUserId, int staleMinutes = 30);
    void deleteIncompleteMessages(const QString &aiUserId);
    bool hasLocalMessages(const QString &aiUserId);

    // Unread
    QHash<QString, int> getUnreadCounts();
    void markAsRead(const QString &aiUserId);

    // Sync tracking
    std::optional<QString> getSyncTime(const QString &aiUserId);
    void setSyncTime(const QString &aiUserId, const QString &time);

private:
    void runMigrations();
    std::optional<MessageDTO> messageFromQuery(QSqlQuery &q);

    QSqlDatabase db_;
};
