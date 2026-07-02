#include "MacDock.h"

#ifdef Q_OS_MACOS

#include <QDebug>

// ---------------------------------------------------------------------------
// When a full NSApp dockTile implementation is needed, rename this file
// to MacDock.mm and link against Cocoa:
//
// #import <Cocoa/Cocoa.h>
//
// void MacDock::updateBadge(int count) {
//     NSString *label = count > 0
//         ? [NSString stringWithFormat:@"%d", count]
//         : nil;
//     [[NSApp dockTile] setBadgeLabel:label];
// }
// ---------------------------------------------------------------------------

MacDock::MacDock(QObject *parent)
    : PlatformIntegration(parent)
{
    qDebug() << "MacDock: initialised (stub — NSApp not linked)";
}

MacDock::~MacDock()
{
}

void MacDock::updateBadge(int count)
{
    if (currentBadge_ == count)
        return;
    currentBadge_ = count;

    // TODO: Full NSApp dockTile implementation (see comment above)
    // [[NSApp dockTile] setBadgeLabel: count > 0 ? @(count).stringValue : nil];

    qDebug() << "MacDock: badge" << count << "(stub)";
}

void MacDock::showNotification(const QString &title, const QString &body)
{
    // macOS UserNotifications are handled by QSystemTrayIcon or the
    // NSUserNotificationCenter.  This is a stub for API consistency.
    qDebug() << "MacDock: notification" << title << "-" << body << "(stub)";
}

void MacDock::requestAttention()
{
    // TODO: [NSApp requestUserAttention:NSInformationalRequest]
    // or [NSApp requestUserAttention:NSCriticalRequest]
    qDebug() << "MacDock: requestAttention (stub)";
}

#endif // Q_OS_MACOS
