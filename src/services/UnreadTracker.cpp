#include "UnreadTracker.h"
#include "../core/storage/Database.h"
#include "../repositories/ConversationRepository.h"
#include <QDebug>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

UnreadTracker::UnreadTracker(Database *db,
                             ConversationRepository *convRepo,
                             QObject *parent)
    : QObject(parent)
    , db_(db)
    , convRepo_(convRepo)
{
}

// ---------------------------------------------------------------------------
// init — load persisted unread counts from the database
// ---------------------------------------------------------------------------

void UnreadTracker::init()
{
    if (!db_)
        return;

    counts_ = db_->getUnreadCounts();

    qDebug() << "UnreadTracker::init loaded" << counts_.size() << "entries";
    emit unreadMapChanged();
    emit totalUnreadChanged(totalUnread());
}

// ---------------------------------------------------------------------------
// onIncomingMessage — increment unread for the AI user owning this
//                     conversation, unless that user is currently active
// ---------------------------------------------------------------------------

void UnreadTracker::onIncomingMessage(const QString &conversationId)
{
    if (conversationId.isEmpty())
        return;

    // Resolve conversationId → aiUserId
    QString aiUserId;

    if (convRepo_) {
        auto resolved = convRepo_->getAiUserIdForConversation(conversationId);
        if (resolved)
            aiUserId = *resolved;
    }

    if (aiUserId.isEmpty()) {
        // We cannot resolve the AI user — drop the event rather than
        // attributing it to the wrong conversation.
        qDebug() << "UnreadTracker: cannot resolve aiUserId for conversation"
                 << conversationId;
        return;
    }

    // Skip if this is the currently active chat
    if (aiUserId == activeAiUserId_)
        return;

    int &count = counts_[aiUserId];
    ++count;

    emit unreadMapChanged();
    emit totalUnreadChanged(totalUnread());
}

// ---------------------------------------------------------------------------
// clearAndMarkRead — clear unread count for an AI user and persist
// ---------------------------------------------------------------------------

void UnreadTracker::clearAndMarkRead(const QString &aiUserId)
{
    if (!counts_.contains(aiUserId) || counts_.value(aiUserId) == 0)
        return;

    counts_[aiUserId] = 0;

    if (db_)
        db_->markAsRead(aiUserId);

    emit unreadMapChanged();
    emit totalUnreadChanged(totalUnread());
}

// ---------------------------------------------------------------------------
// setActiveAiUser — track which chat is currently visible
// ---------------------------------------------------------------------------

void UnreadTracker::setActiveAiUser(const QString &aiUserId)
{
    if (activeAiUserId_ == aiUserId)
        return;

    activeAiUserId_ = aiUserId;

    // When switching to a chat, automatically clear its unread count
    if (!aiUserId.isEmpty()) {
        clearAndMarkRead(aiUserId);
    }
}

// ---------------------------------------------------------------------------
// unreadMap / totalUnread — property accessors
// ---------------------------------------------------------------------------

QVariantMap UnreadTracker::unreadMap() const
{
    QVariantMap map;
    for (auto it = counts_.constBegin(); it != counts_.constEnd(); ++it) {
        if (it.value() > 0)
            map[it.key()] = it.value();
    }
    return map;
}

int UnreadTracker::totalUnread() const
{
    int total = 0;
    for (auto it = counts_.constBegin(); it != counts_.constEnd(); ++it) {
        total += it.value();
    }
    return total;
}
