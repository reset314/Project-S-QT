#pragma once
#include <QSqlDatabase>
#include <QVector>
#include <QHash>
#include <QString>
#include <QVariantMap>
#include <optional>
#include "../models/MessageDTO.h"
#include "../models/MemoryDTO.h"

class Database {
public:
    Database();
    ~Database();

    bool open(const QString &path);
    QSqlDatabase &db() { return db_; }

    // Messages
    int64_t insertMessage(const MessageDTO &msg, const QString &aiUserId);
    void upsertMessage(const MessageDTO &msg, const QString &aiUserId);
    std::optional<MessageDTO> findByServerId(const QString &serverId);
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

    // Device info
    QString getDeviceInfo(const QString &key, const QString &defaultValue = {});
    void setDeviceInfo(const QString &key, const QString &value);

    // Event seq tracking
    int getLastEventSeq(const QString &userId);
    void setLastEventSeq(const QString &userId, int seq);

    // Memories
    QVector<MemoryDTO> getMemories(const QString &aiUserId, const QStringList &types = {"MTM","LTM"},
                                    const QString &beforeId = {}, int limit = 30);

private:
    void runMigrations();
    std::optional<MessageDTO> messageFromQuery(QSqlQuery &q);

    QSqlDatabase db_;
};
