#pragma once
#include <QObject>
#include <QHash>
#include <QVariantMap>
#include <QSet>

class Database;
class ConversationRepository;

/// Tracks per-AI-user unread message counts.
///
/// Exposes a QVariantMap so QML can bind directly to per-conversation
/// badges.  Messages arriving for the currently-active AI user are
/// automatically excluded (the user is looking at that chat).
///
/// Usage:
///   1. Call init() once after construction to load persisted counts.
///   2. Call setActiveAiUser() whenever the user switches chats.
///   3. Call onIncomingMessage() for each incoming WS message.
///   4. Call clearAndMarkRead() when the user opens a chat.
class UnreadTracker : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap unreadMap READ unreadMap NOTIFY unreadMapChanged)
public:
    explicit UnreadTracker(Database *db,
                           ConversationRepository *convRepo = nullptr,
                           QObject *parent = nullptr);

    /// Load initial counts from the database.
    void init();

    /// Record an incoming message for a conversation.
    /// Increments the unread count for the associated AI user unless
    /// that AI user is currently active.
    void onIncomingMessage(const QString &conversationId);

    /// Clear the unread count for an AI user and persist the read marker.
    void clearAndMarkRead(const QString &aiUserId);

    /// Set which AI user chat is currently visible.
    /// Messages arriving for this user will not increment the badge.
    void setActiveAiUser(const QString &aiUserId);

    /// Return the current unread map (aiUserId → count).
    QVariantMap unreadMap() const;

    /// Return the total unread count across all AI users.
    int totalUnread() const;

signals:
    void unreadMapChanged();
    void totalUnreadChanged(int total);

private:
    Database *db_;
    ConversationRepository *convRepo_;
    QHash<QString, int> counts_;       // aiUserId → unread count
    QString activeAiUserId_;           // currently visible chat
};
