#include "SyncService.h"
#include "../repositories/ChatRepository.h"
#include "../core/storage/Database.h"
#include <QDebug>
#include <QDateTime>
#include <QSet>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

SyncService::SyncService(ChatRepository *repo,
                         Database *db,
                         QObject *parent)
    : QObject(parent)
    , repo_(repo)
    , db_(db)
{
}

// ---------------------------------------------------------------------------
// syncMessages
// ---------------------------------------------------------------------------

void SyncService::syncMessages(const QString &aiUserId, double anchorHours)
{
    // Determine the effective sync timestamp
    QString effectiveSince;

    if (anchorHours > 0.0) {
        // Use a relative anchor: now minus anchorHours
        QDateTime anchor = QDateTime::currentDateTimeUtc()
                               .addSecs(-static_cast<qint64>(anchorHours * 3600.0));
        effectiveSince = anchor.toString(Qt::ISODate);
    } else {
        // Use the stored sync timestamp if available, otherwise default to
        // a short window (6 minutes ago)
        auto stored = repo_->getSyncTime(aiUserId);
        if (stored) {
            effectiveSince = *stored;
        } else {
            QDateTime fallback = QDateTime::currentDateTimeUtc().addSecs(-360);
            effectiveSince = fallback.toString(Qt::ISODate);
        }
    }

    // -----------------------------------------------------------------------
    // Layer 1 — Phantom cleanup: remove stale unsynced optimistic inserts
    //           that were never confirmed by the server (30 min threshold).
    //           This is safe to run before the fetch because it only affects
    //           messages without a server_id.
    // -----------------------------------------------------------------------
    repo_->deleteStaleUnsynced(aiUserId, 30);

    // -----------------------------------------------------------------------
    // Layer 2 — Fetch from server FIRST, before deleting any local data.
    //           If the fetch fails we preserve local state and bail out.
    // -----------------------------------------------------------------------
    auto result = repo_->syncMessages(aiUserId, effectiveSince);

    if (!result) {
        qWarning() << "SyncService::syncMessages failed for" << aiUserId
                   << ":" << QString::fromStdString(result.error().code)
                   << QString::fromStdString(result.error().message);

        emit syncError(aiUserId, QString::fromStdString(result.error().message));
        emit syncComplete(aiUserId);
        return;
    }

    const auto &sync = *result;

    // -----------------------------------------------------------------------
    // Layer 3 — Now that we have a successful server response, clear the local
    //           window we are about to replace with fresh server data.
    // -----------------------------------------------------------------------
    repo_->deleteMessagesSince(aiUserId, effectiveSince);

    // Build a set of server-side serverIds for reconciliation
    QSet<QString> serverIds;
    for (const auto &msg : sync.messages) {
        if (!msg.serverId.empty())
            serverIds.insert(QString::fromStdString(msg.serverId));
    }

    // Reconciliation: only for messages within the sync window (timestamp >=
    // effectiveSince).  Older messages are history — the sync response won't
    // include them and they should NOT be deleted.
    if (db_) {
        QVector<MessageDTO> localMessages = db_->getLocalMessages(aiUserId, 500);

        for (const auto &local : localMessages) {
            if (local.serverId.empty())
                continue; // No serverId yet — Layer 2 handles these
            if (local.deletedAt)
                continue; // Already soft-deleted

            // Only reconcile messages within the sync window
            if (QString::fromStdString(local.timestamp) < effectiveSince)
                continue;

            if (!serverIds.contains(QString::fromStdString(local.serverId))) {
                // This message exists locally but not on the server —
                // it was likely deleted server-side.  Soft-delete locally.
                QVariantMap values;
                values[QStringLiteral("deleted_at")] =
                    QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
                db_->updateMessage(
                    QString::fromStdString(local.clientUuid), values);

                qDebug() << "SyncService: soft-deleted local message"
                         << QString::fromStdString(local.clientUuid)
                         << "(serverId" << QString::fromStdString(local.serverId) << ")";
            }
        }
    }

    // Update the sync timestamp so the next incremental sync starts from here
    if (!sync.syncTimestamp.empty()) {
        repo_->setSyncTime(aiUserId, QString::fromStdString(sync.syncTimestamp));
    } else {
        repo_->setSyncTime(aiUserId,
            QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    }

    qDebug() << "SyncService: sync complete for" << aiUserId
             << "- fetched" << sync.messages.size() << "messages";

    emit syncComplete(aiUserId);
}
