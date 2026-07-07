#include "Database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

Database::Database() {}
Database::~Database() { if (db_.isOpen()) db_.close(); }

// ---------------------------------------------------------------------------
// open() — connect, pragmas, run migrations
// ---------------------------------------------------------------------------

bool Database::open(const QString &path) {
    db_ = QSqlDatabase::addDatabase("QSQLITE", "AIChatMain");
    db_.setDatabaseName(path);
    if (!db_.open()) {
        qCritical() << "Database open failed:" << db_.lastError().text();
        return false;
    }
    QSqlQuery q(db_);
    q.exec("PRAGMA journal_mode=WAL");
    q.exec("PRAGMA foreign_keys=ON");
    runMigrations();
    return true;
}

// ---------------------------------------------------------------------------
// Schema migrations
// ---------------------------------------------------------------------------

void Database::runMigrations() {
    QSqlQuery q(db_);
    q.exec("CREATE TABLE IF NOT EXISTS schema_version (version INTEGER)");
    q.exec("SELECT MAX(version) FROM schema_version");
    int currentVersion = q.next() ? q.value(0).toInt() : 0;

    struct Migration { int version; QString sql; };
    QVector<Migration> migrations = {
        {1, R"(
            CREATE TABLE IF NOT EXISTS messages (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                server_id TEXT,
                client_uuid TEXT NOT NULL,
                ai_user_id TEXT NOT NULL,
                conversation_id TEXT,
                sender_type TEXT NOT NULL,
                msg_type TEXT NOT NULL,
                content TEXT,
                media_list TEXT,
                is_complete INTEGER DEFAULT 1,
                is_read INTEGER DEFAULT 0,
                source_function TEXT,
                revoked_at TEXT,
                deleted_at TEXT,
                timestamp TEXT NOT NULL
            )
        )"},
        {2, "CREATE INDEX IF NOT EXISTS idx_messages_ai_user ON messages(ai_user_id)"},
        {3, "CREATE INDEX IF NOT EXISTS idx_messages_client ON messages(client_uuid)"},
        {4, "CREATE INDEX IF NOT EXISTS idx_messages_server ON messages(server_id)"},
        // Note: if migration 10 fails (table locked), server_id stays as migration 1
        // created it.  We now default to TEXT in both places for safety.
        {5, R"(
            CREATE TABLE IF NOT EXISTS conversations (
                id TEXT PRIMARY KEY,
                ai_user_id TEXT NOT NULL,
                title TEXT,
                summary TEXT,
                last_message_preview TEXT,
                unread_count INTEGER DEFAULT 0,
                created_at TEXT,
                updated_at TEXT
            )
        )"},
        {6, "CREATE INDEX IF NOT EXISTS idx_conv_ai_user ON conversations(ai_user_id)"},
        {7, R"(
            CREATE TABLE IF NOT EXISTS sync_meta (
                ai_user_id TEXT PRIMARY KEY,
                last_sync_time TEXT
            )
        )"},
        // Fix conversations.id to TEXT — server uses UUID strings, not integers.
        // SQLite has no ALTER COLUMN; recreate the table.
        {8, R"(
            CREATE TABLE IF NOT EXISTS conversations_new (
                id TEXT PRIMARY KEY,
                ai_user_id TEXT NOT NULL,
                title TEXT,
                summary TEXT,
                last_message_preview TEXT,
                unread_count INTEGER DEFAULT 0,
                created_at TEXT,
                updated_at TEXT
            );
            INSERT OR IGNORE INTO conversations_new
                (id, ai_user_id, title, summary, last_message_preview,
                 unread_count, created_at, updated_at)
            SELECT CAST(id AS TEXT), ai_user_id, title, summary,
                   last_message_preview, unread_count, created_at, updated_at
            FROM conversations;
            DROP TABLE IF EXISTS conversations;
            ALTER TABLE conversations_new RENAME TO conversations;
        )"},
        {9, "CREATE INDEX IF NOT EXISTS idx_conv_ai_user ON conversations(ai_user_id)"},
        // Fix messages.conversation_id to TEXT for UUID compatibility.
        {10, R"(
            CREATE TABLE IF NOT EXISTS messages_new (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                server_id TEXT,
                client_uuid TEXT NOT NULL,
                ai_user_id TEXT NOT NULL,
                conversation_id TEXT,
                sender_type TEXT NOT NULL,
                msg_type TEXT NOT NULL,
                content TEXT,
                media_list TEXT,
                is_complete INTEGER DEFAULT 1,
                is_read INTEGER DEFAULT 0,
                source_function TEXT,
                revoked_at TEXT,
                deleted_at TEXT,
                timestamp TEXT NOT NULL
            );
            INSERT OR IGNORE INTO messages_new
                (id, server_id, client_uuid, ai_user_id, conversation_id,
                 sender_type, msg_type, content, media_list, is_complete, is_read,
                 source_function, revoked_at, deleted_at, timestamp)
            SELECT id, server_id, client_uuid, ai_user_id,
                   CAST(conversation_id AS TEXT),
                   sender_type, msg_type, content, media_list, is_complete, is_read,
                   source_function, revoked_at, deleted_at, timestamp
            FROM messages;
            DROP TABLE IF EXISTS messages;
            ALTER TABLE messages_new RENAME TO messages;
        )"},
        {11, "CREATE INDEX IF NOT EXISTS idx_messages_ai_user ON messages(ai_user_id)"},
        {12, "CREATE INDEX IF NOT EXISTS idx_messages_client ON messages(client_uuid)"},
        {13, "CREATE INDEX IF NOT EXISTS idx_messages_server ON messages(server_id)"},
        // Memory table
        {14, R"(
            CREATE TABLE IF NOT EXISTS memories (
                id TEXT PRIMARY KEY,
                ai_user_id TEXT NOT NULL,
                content TEXT NOT NULL,
                summary TEXT,
                memory_type TEXT DEFAULT 'STM',
                category TEXT DEFAULT 'general',
                importance INTEGER DEFAULT 5,
                confidence REAL DEFAULT 1.0,
                is_archived INTEGER DEFAULT 0,
                source_memory_id TEXT,
                created_at TEXT NOT NULL,
                updated_at TEXT NOT NULL
            );
            CREATE INDEX IF NOT EXISTS idx_mem_ai_user ON memories(ai_user_id);
            CREATE INDEX IF NOT EXISTS idx_mem_type ON memories(memory_type);
        )"},
        // Migration 11: device_info table (stable device_id)
        {11, R"(
            CREATE TABLE IF NOT EXISTS device_info (
                key TEXT PRIMARY KEY,
                value TEXT NOT NULL
            )
        )"},
        // Migration 12: event_seq_tracker (per-user last event seq)
        {12, R"(
            CREATE TABLE IF NOT EXISTS event_seq_tracker (
                user_id TEXT PRIMARY KEY,
                last_seq INTEGER DEFAULT 0,
                updated_at TEXT
            )
        )"},
    };

    for (const auto &m : migrations) {
        if (m.version > currentVersion) {
            // Split multi-statement SQL by ';' — QSqlQuery::exec() only
            // executes one statement at a time.
            bool ok = true;
            const auto statements = m.sql.split(';');
            for (const auto &stmt : statements) {
                auto trimmed = stmt.trimmed();
                if (trimmed.isEmpty())
                    continue;
                QSqlQuery stmtQ(db_);
                if (!stmtQ.exec(trimmed)) {
                    qWarning() << "Migration" << m.version
                               << "statement failed:" << stmtQ.lastError().text()
                               << "- SQL:" << trimmed.left(80);
                    ok = false;
                }
                stmtQ.finish();  // release table locks before next statement
            }
            if (ok) {
                QSqlQuery ins(db_);
                ins.prepare("INSERT OR REPLACE INTO schema_version VALUES(?)");
                ins.addBindValue(m.version);
                ins.exec();
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Row → DTO helper
// ---------------------------------------------------------------------------

std::optional<MessageDTO> Database::messageFromQuery(QSqlQuery &q) {
    if (!q.isActive() || !q.isSelect())
        return std::nullopt;

    MessageDTO m;

    // server_id — stored as TEXT (UUID), may be NULL for optimistic inserts
    QVariant sv = q.value("server_id");
    if (!sv.isNull())
        m.serverId = sv.toString().toStdString();

    m.clientUuid     = q.value("client_uuid").toString().toStdString();
    m.aiUserId       = q.value("ai_user_id").toString().toStdString();
    m.conversationId = q.value("conversation_id").toString().toStdString();
    m.senderType     = q.value("sender_type").toString().toStdString();
    m.msgType        = q.value("msg_type").toString().toStdString();

    // content — stored as compact JSON text
    QString contentStr = q.value("content").toString();
    if (!contentStr.isEmpty()) {
        m.content = contentStr.toStdString();
    }

    // media_list — stored as compact JSON array text
    QString mediaStr = q.value("media_list").toString();
    if (!mediaStr.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(mediaStr.toUtf8());
        if (doc.isArray()) {
            for (const auto &item : doc.array()) {
                if (item.isObject())
                    m.mediaList.push_back(MediaDTO::fromJson(item.toObject()));
            }
        }
    }

    m.isComplete = q.value("is_complete").toInt() != 0;
    m.isRead     = q.value("is_read").toInt() != 0;

    QString sf = q.value("source_function").toString();
    if (!sf.isEmpty())
        m.sourceFunction = sf.toStdString();

    QString ra = q.value("revoked_at").toString();
    if (!ra.isEmpty())
        m.revokedAt = QDateTime::fromString(ra, Qt::ISODate);

    QString da = q.value("deleted_at").toString();
    if (!da.isEmpty())
        m.deletedAt = QDateTime::fromString(da, Qt::ISODate);

    m.timestamp = q.value("timestamp").toString().toStdString();

    return m;
}

// ---------------------------------------------------------------------------
// insertMessage
// ---------------------------------------------------------------------------

int64_t Database::insertMessage(const MessageDTO &msg, const QString &aiUserId) {
    QSqlQuery q(db_);
    q.prepare(R"(
        INSERT INTO messages (server_id, client_uuid, ai_user_id, conversation_id,
            sender_type, msg_type, content, media_list, is_complete, is_read,
            source_function, revoked_at, timestamp)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    q.addBindValue(msg.serverId.empty() ? QVariant() : QString::fromStdString(msg.serverId));
    q.addBindValue(QString::fromStdString(msg.clientUuid));
    q.addBindValue(aiUserId);
    q.addBindValue(QString::fromStdString(msg.conversationId));
    q.addBindValue(QString::fromStdString(msg.senderType));
    q.addBindValue(QString::fromStdString(msg.msgType));
    q.addBindValue(QString::fromStdString(msg.content));
    QJsonArray ml;
    for (const auto &m : msg.mediaList)
        ml.append(m.toJson());
    q.addBindValue(QString::fromUtf8(QJsonDocument(ml).toJson(QJsonDocument::Compact)));
    q.addBindValue(msg.isComplete ? 1 : 0);
    q.addBindValue(msg.isRead ? 1 : 0);
    q.addBindValue(msg.sourceFunction ? QString::fromStdString(*msg.sourceFunction) : QVariant());
    q.addBindValue(msg.revokedAt ? msg.revokedAt->toString(Qt::ISODate) : QVariant());
    q.addBindValue(QString::fromStdString(msg.timestamp));
    if (!q.exec()) {
        qWarning() << "insertMessage failed:" << q.lastError().text();
        return -1;
    }
    return q.lastInsertId().toLongLong();
}

// ---------------------------------------------------------------------------
// upsertMessage — insert or replace by client_uuid
// ---------------------------------------------------------------------------

void Database::upsertMessage(const MessageDTO &msg, const QString &aiUserId) {
    // Try to find an existing row by client_uuid
    QSqlQuery sel(db_);
    sel.prepare("SELECT id FROM messages WHERE client_uuid = ?");
    sel.addBindValue(QString::fromStdString(msg.clientUuid));
    sel.exec();

    if (sel.next()) {
        // Update existing row
        QSqlQuery up(db_);
        up.prepare(R"(
            UPDATE messages SET
                server_id = ?, ai_user_id = ?, conversation_id = ?,
                sender_type = ?, msg_type = ?, content = ?, media_list = ?,
                is_complete = ?, is_read = ?, source_function = ?,
                revoked_at = ?, timestamp = ?
            WHERE client_uuid = ?
        )");
        up.addBindValue(msg.serverId.empty() ? QVariant() : QString::fromStdString(msg.serverId));
        up.addBindValue(aiUserId);
        up.addBindValue(QString::fromStdString(msg.conversationId));
        up.addBindValue(QString::fromStdString(msg.senderType));
        up.addBindValue(QString::fromStdString(msg.msgType));
        up.addBindValue(QString::fromUtf8(QJsonDocument(msg.content).toJson(QJsonDocument::Compact)));
        QJsonArray ml;
        for (const auto &m : msg.mediaList)
            ml.append(m.toJson());
        up.addBindValue(QString::fromUtf8(QJsonDocument(ml).toJson(QJsonDocument::Compact)));
        up.addBindValue(msg.isComplete ? 1 : 0);
        up.addBindValue(msg.isRead ? 1 : 0);
        up.addBindValue(msg.sourceFunction ? QString::fromStdString(*msg.sourceFunction) : QVariant());
        up.addBindValue(msg.revokedAt ? msg.revokedAt->toString(Qt::ISODate) : QVariant());
        up.addBindValue(QString::fromStdString(msg.timestamp));
        up.addBindValue(QString::fromStdString(msg.clientUuid));
        if (!up.exec())
            qWarning() << "upsertMessage update failed:" << up.lastError().text();
    } else {
        // Insert new row
        insertMessage(msg, aiUserId);
    }
}

// ---------------------------------------------------------------------------
// findByServerId
// ---------------------------------------------------------------------------

std::optional<MessageDTO> Database::findByServerId(const QString &serverId) {
    QSqlQuery q(db_);
    q.prepare(R"(
        SELECT * FROM messages
        WHERE server_id = ? AND (deleted_at IS NULL OR deleted_at = '')
    )");
    q.addBindValue(serverId);
    if (!q.exec()) {
        qWarning() << "findByServerId failed:" << q.lastError().text();
        return std::nullopt;
    }
    if (!q.next())
        return std::nullopt;
    return messageFromQuery(q);
}

// ---------------------------------------------------------------------------
// findByClientUuid
// ---------------------------------------------------------------------------

std::optional<MessageDTO> Database::findByClientUuid(const QString &clientUuid) {
    QSqlQuery q(db_);
    q.prepare(R"(
        SELECT * FROM messages
        WHERE client_uuid = ? AND (deleted_at IS NULL OR deleted_at = '')
    )");
    q.addBindValue(clientUuid);
    if (!q.exec()) {
        qWarning() << "findByClientUuid failed:" << q.lastError().text();
        return std::nullopt;
    }
    if (!q.next())
        return std::nullopt;
    return messageFromQuery(q);
}

// ---------------------------------------------------------------------------
// getLocalMessages
// ---------------------------------------------------------------------------

QVector<MessageDTO> Database::getLocalMessages(const QString &aiUserId, int limit) {
    QVector<MessageDTO> result;

    // Diagnostic: count total rows and rows for this aiUserId
    {
        QSqlQuery diag(db_);
        diag.exec("SELECT COUNT(*) FROM messages");
        int total = diag.next() ? diag.value(0).toInt() : -1;
        diag.exec("SELECT COUNT(*) FROM messages WHERE deleted_at IS NULL OR deleted_at = ''");
        int nonDeleted = diag.next() ? diag.value(0).toInt() : -1;
        QSqlQuery diag2(db_);
        diag2.prepare("SELECT COUNT(*) FROM messages WHERE ai_user_id = ?");
        diag2.addBindValue(aiUserId);
        diag2.exec();
        int forUser = diag2.next() ? diag2.value(0).toInt() : -1;
        qDebug() << "getLocalMessages: total rows =" << total
                 << "non-deleted =" << nonDeleted
                 << "for aiUserId" << aiUserId << "=" << forUser;
    }

    QSqlQuery q(db_);
    q.prepare(R"(
        SELECT * FROM messages
        WHERE ai_user_id = ? AND (deleted_at IS NULL OR deleted_at = '')
        ORDER BY timestamp DESC
        LIMIT ?
    )");
    q.addBindValue(aiUserId);
    q.addBindValue(limit);
    if (!q.exec()) {
        qWarning() << "getLocalMessages failed:" << q.lastError().text();
        return result;
    }
    while (q.next()) {
        auto m = messageFromQuery(q);
        if (m) result.append(*m);
    }
    return result;
}

// ---------------------------------------------------------------------------
// getLocalMessagesBefore — pagination helper
// ---------------------------------------------------------------------------

QVector<MessageDTO> Database::getLocalMessagesBefore(const QString &aiUserId,
                                                      const QString &beforeTimestamp,
                                                      int limit) {
    QVector<MessageDTO> result;
    QSqlQuery q(db_);
    q.prepare(R"(
        SELECT * FROM messages
        WHERE ai_user_id = ? AND (deleted_at IS NULL OR deleted_at = '')
          AND timestamp < ?
        ORDER BY timestamp DESC
        LIMIT ?
    )");
    q.addBindValue(aiUserId);
    q.addBindValue(beforeTimestamp);
    q.addBindValue(limit);
    if (!q.exec()) {
        qWarning() << "getLocalMessagesBefore failed:" << q.lastError().text();
        return result;
    }
    while (q.next()) {
        auto m = messageFromQuery(q);
        if (m) result.append(*m);
    }
    return result;
}

// ---------------------------------------------------------------------------
// updateMessage — field-level partial update by client_uuid
// ---------------------------------------------------------------------------

void Database::updateMessage(const QString &clientUuid, const QVariantMap &values) {
    if (values.isEmpty())
        return;

    QStringList setClauses;
    QVariantList bindValues;

    for (auto it = values.cbegin(); it != values.cend(); ++it) {
        setClauses.append(it.key() + " = ?");
        bindValues.append(it.value());
    }
    bindValues.append(clientUuid);

    QString sql = "UPDATE messages SET " + setClauses.join(", ") + " WHERE client_uuid = ?";
    QSqlQuery q(db_);
    q.prepare(sql);
    for (const auto &v : bindValues)
        q.addBindValue(v);
    if (!q.exec())
        qWarning() << "updateMessage failed:" << q.lastError().text();
}

// ---------------------------------------------------------------------------
// deleteMessage — soft-delete by client_uuid
// ---------------------------------------------------------------------------

void Database::deleteMessage(const QString &clientUuid) {
    QSqlQuery q(db_);
    q.prepare("UPDATE messages SET deleted_at = ? WHERE client_uuid = ?");
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    q.addBindValue(clientUuid);
    if (!q.exec())
        qWarning() << "deleteMessage failed:" << q.lastError().text();
}

// ---------------------------------------------------------------------------
// deleteMessagesSince
// ---------------------------------------------------------------------------

void Database::deleteMessagesSince(const QString &aiUserId, const QString &since) {
    QSqlQuery q(db_);
    q.prepare("UPDATE messages SET deleted_at = ? "
              "WHERE ai_user_id = ? AND timestamp >= ? "
              "AND (deleted_at IS NULL OR deleted_at = '')");
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    q.addBindValue(aiUserId);
    q.addBindValue(since);
    if (!q.exec())
        qWarning() << "deleteMessagesSince failed:" << q.lastError().text();
}

// ---------------------------------------------------------------------------
// deleteStaleUnsynced — removes optimistic inserts that never got a server_id
// ---------------------------------------------------------------------------

void Database::deleteStaleUnsynced(const QString &aiUserId, int staleMinutes) {
    QSqlQuery q(db_);
    auto threshold = QDateTime::currentDateTimeUtc()
        .addSecs(-staleMinutes * 60).toString(Qt::ISODate);
    q.prepare("DELETE FROM messages WHERE ai_user_id = ? AND server_id IS NULL AND timestamp < ?");
    q.addBindValue(aiUserId);
    q.addBindValue(threshold);
    if (!q.exec())
        qWarning() << "deleteStaleUnsynced failed:" << q.lastError().text();
}

// ---------------------------------------------------------------------------
// deleteIncompleteMessages
// ---------------------------------------------------------------------------

void Database::deleteIncompleteMessages(const QString &aiUserId) {
    QSqlQuery q(db_);
    q.prepare("DELETE FROM messages WHERE ai_user_id = ? AND is_complete = 0");
    q.addBindValue(aiUserId);
    if (!q.exec())
        qWarning() << "deleteIncompleteMessages failed:" << q.lastError().text();
}

// ---------------------------------------------------------------------------
// hasLocalMessages
// ---------------------------------------------------------------------------

bool Database::hasLocalMessages(const QString &aiUserId) {
    QSqlQuery q(db_);
    q.prepare("SELECT EXISTS(SELECT 1 FROM messages WHERE ai_user_id = ? "
              "AND (deleted_at IS NULL OR deleted_at = ''))");
    q.addBindValue(aiUserId);
    if (!q.exec()) {
        qWarning() << "hasLocalMessages failed:" << q.lastError().text();
        return false;
    }
    return q.next() && q.value(0).toInt() != 0;
}

// ---------------------------------------------------------------------------
// getUnreadCounts
// ---------------------------------------------------------------------------

QHash<QString, int> Database::getUnreadCounts() {
    QHash<QString, int> counts;
    QSqlQuery q(db_);
    q.exec("SELECT m.ai_user_id, COUNT(*) as cnt FROM messages m "
           "WHERE m.is_read = 0 AND m.ai_user_id IS NOT NULL "
           "AND (m.deleted_at IS NULL OR m.deleted_at = '') "
           "AND (m.revoked_at IS NULL OR m.revoked_at = '') "
           "GROUP BY m.ai_user_id");
    while (q.next())
        counts[q.value(0).toString()] = q.value(1).toInt();
    return counts;
}

// ---------------------------------------------------------------------------
// markAsRead
// ---------------------------------------------------------------------------

void Database::markAsRead(const QString &aiUserId) {
    QSqlQuery q(db_);
    q.prepare("UPDATE messages SET is_read = 1 WHERE ai_user_id = ? AND is_read = 0");
    q.addBindValue(aiUserId);
    if (!q.exec())
        qWarning() << "markAsRead failed:" << q.lastError().text();
}

// ---------------------------------------------------------------------------
// Sync metadata
// ---------------------------------------------------------------------------

std::optional<QString> Database::getSyncTime(const QString &aiUserId) {
    QSqlQuery q(db_);
    q.prepare("SELECT last_sync_time FROM sync_meta WHERE ai_user_id = ?");
    q.addBindValue(aiUserId);
    if (!q.exec()) {
        qWarning() << "getSyncTime failed:" << q.lastError().text();
        return std::nullopt;
    }
    if (!q.next())
        return std::nullopt;
    QString val = q.value(0).toString();
    if (val.isEmpty())
        return std::nullopt;
    return val;
}

void Database::setSyncTime(const QString &aiUserId, const QString &time) {
    QSqlQuery q(db_);
    q.prepare("INSERT OR REPLACE INTO sync_meta (ai_user_id, last_sync_time) VALUES (?, ?)");
    q.addBindValue(aiUserId);
    q.addBindValue(time);
    if (!q.exec())
        qWarning() << "setSyncTime failed:" << q.lastError().text();
}

// ── Memories ────────────────────────────────────────────────────────
QVector<MemoryDTO> Database::getMemories(const QString &aiUserId,
                                          const QStringList &types,
                                          const QString &beforeId, int limit) {
    QVector<MemoryDTO> result;
    QString sql = "SELECT * FROM memories WHERE ai_user_id = ?";
    if (!types.isEmpty()) {
        QStringList placeholders;
        for (const auto &t : types) placeholders.append("?");
        sql += " AND memory_type IN (" + placeholders.join(",") + ")";
    }
    if (!beforeId.isEmpty()) sql += " AND id < ?";
    sql += " ORDER BY created_at DESC LIMIT ?";

    QSqlQuery q(db_); q.prepare(sql);
    q.addBindValue(aiUserId);
    for (const auto &t : types) q.addBindValue(t);
    if (!beforeId.isEmpty()) q.addBindValue(beforeId);
    q.addBindValue(limit);

    if (!q.exec()) { qWarning() << "getMemories failed:" << q.lastError().text(); return result; }
    while (q.next()) {
        MemoryDTO m;
        m.id = q.value("id").toString().toStdString();
        m.aiUserId = q.value("ai_user_id").toString().toStdString();
        m.content = q.value("content").toString().toStdString();
        if (!q.value("summary").isNull()) m.summary = q.value("summary").toString().toStdString();
        m.memoryType = q.value("memory_type").toString().toStdString();
        m.category = q.value("category").toString().toStdString();
        m.importance = q.value("importance").toInt();
        m.confidence = q.value("confidence").toDouble();
        m.isArchived = q.value("is_archived").toBool();
        if (!q.value("source_memory_id").isNull()) m.sourceMemoryId = q.value("source_memory_id").toString().toStdString();
        m.createdAt = q.value("created_at").toString().toStdString();
        m.updatedAt = q.value("updated_at").toString().toStdString();
        result.append(m);
    }
    return result;
}

// ---------------------------------------------------------------------------
// Device info
// ---------------------------------------------------------------------------

QString Database::getDeviceInfo(const QString &key, const QString &defaultValue) {
    QSqlQuery q(db_);
    q.prepare("SELECT value FROM device_info WHERE key = ?");
    q.addBindValue(key);
    if (q.exec() && q.next())
        return q.value(0).toString();
    return defaultValue;
}

void Database::setDeviceInfo(const QString &key, const QString &value) {
    QSqlQuery q(db_);
    q.prepare("INSERT OR REPLACE INTO device_info (key, value) VALUES (?, ?)");
    q.addBindValue(key);
    q.addBindValue(value);
    q.exec();
}

// ---------------------------------------------------------------------------
// Event seq tracking
// ---------------------------------------------------------------------------

int Database::getLastEventSeq(const QString &userId) {
    QSqlQuery q(db_);
    q.prepare("SELECT last_seq FROM event_seq_tracker WHERE user_id = ?");
    q.addBindValue(userId);
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return 0;
}

void Database::setLastEventSeq(const QString &userId, int seq) {
    QSqlQuery q(db_);
    q.prepare("INSERT OR REPLACE INTO event_seq_tracker (user_id, last_seq, updated_at) "
              "VALUES (?, ?, ?)");
    q.addBindValue(userId);
    q.addBindValue(seq);
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    q.exec();
}
