#include "NotificationService.h"
#include <QDebug>
#include <QApplication>

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

NotificationService::NotificationService(QObject *parent)
    : QObject(parent)
{
    // Try to load default icons from resources — these may not exist yet
    // and will be set later by the application.
    iconNormal_ = QIcon(QStringLiteral(":/icons/tray_normal.png"));
    iconUnread_ = QIcon(QStringLiteral(":/icons/tray_unread.png"));
}

NotificationService::~NotificationService()
{
    // tray_ is not owned by us — it was passed in via init()
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------

void NotificationService::init(QSystemTrayIcon *trayIcon)
{
    if (trayIcon) {
        tray_ = trayIcon;
    } else {
        // Create a default tray icon if none was provided
        if (!tray_) {
            tray_ = new QSystemTrayIcon(this);
        }
    }

    if (tray_ && !tray_->isVisible()) {
        tray_->setIcon(iconNormal_);
        tray_->show();
    }
}

// ---------------------------------------------------------------------------
// showNotification
// ---------------------------------------------------------------------------

void NotificationService::showNotification(const QString &title,
                                           const QString &body)
{
    if (!tray_) {
        qWarning() << "NotificationService: tray not initialised, "
                       "cannot show notification";
        return;
    }

    if (!QSystemTrayIcon::supportsMessages()) {
        qDebug() << "NotificationService: system tray messages not "
                    "supported on this platform";
        return;
    }

    tray_->showMessage(title, body,
                       QSystemTrayIcon::Information,
                       5000); // 5-second auto-dismiss
}

// ---------------------------------------------------------------------------
// updateTaskbarBadge
// ---------------------------------------------------------------------------

void NotificationService::updateTaskbarBadge(int count)
{
    if (currentBadge_ == count)
        return;

    currentBadge_ = count;

#ifdef Q_OS_WIN
    // TODO: ITaskbarList3 on Windows
    // Requires CoInitialize, HWND from QWindow, and ITaskbarList3 COM interface.
    // Stub for now — taskbar overlay icon with count badge.
    Q_UNUSED(count)
    qDebug() << "NotificationService: taskbar badge stubbed on Windows (count="
             << count << ")";
#elif defined(Q_OS_MACOS)
    // TODO: NSApplication dock badge
    // [[NSApp dockTile] setBadgeLabel: count > 0 ? @(count).stringValue : nil];
    Q_UNUSED(count)
    qDebug() << "NotificationService: dock badge stubbed on macOS (count="
             << count << ")";
#else
    // Linux: Unity launcher API or DBus
    Q_UNUSED(count)
    qDebug() << "NotificationService: badge stubbed on Linux (count="
             << count << ")";
#endif
}

// ---------------------------------------------------------------------------
// updateTrayIcon
// ---------------------------------------------------------------------------

void NotificationService::updateTrayIcon(bool hasUnread)
{
    if (!tray_)
        return;

    if (hasUnread) {
        tray_->setIcon(iconUnread_.isNull() ? iconNormal_ : iconUnread_);
    } else {
        tray_->setIcon(iconNormal_);
    }
}
