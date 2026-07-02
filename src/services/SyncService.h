#pragma once
#include <QObject>
#include <QString>

class ChatRepository;
class Database;

/// Recovers the local message store for a given AI user by re-fetching
/// from the server and reconciling differences.
///
/// Three-layer cleanup:
///   Layer 1 — deleteMessagesSince(effectiveSync): clear the local window
///            that we are about to re-fetch.
///   Layer 2 — deleteStaleUnsynced(30 min): remove phantom rows that were
///            optimistically inserted but never confirmed.
///   Layer 3 — Reconcile: any local message whose serverId is not present
///            in the server response is soft-deleted (deletedAt timestamp).
class SyncService : public QObject {
    Q_OBJECT
public:
    explicit SyncService(ChatRepository *repo,
                         Database *db = nullptr,
                         QObject *parent = nullptr);

    /// Full sync for an AI user.
    /// \param aiUserId   The AI user whose messages should be synced.
    /// \param anchorHours How far back to look (default 0.1 h = 6 min).
    ///                    Pass 0 to use the stored sync timestamp.
    void syncMessages(const QString &aiUserId, double anchorHours = 0.1);

signals:
    /// Emitted when a sync cycle completes (success or failure).
    void syncComplete(const QString &aiUserId);

    /// Emitted when the sync fails with an error.
    void syncError(const QString &aiUserId, const QString &errorMessage);

private:
    ChatRepository *repo_;
    Database *db_;
};
