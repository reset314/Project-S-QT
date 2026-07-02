#pragma once
#include <QObject>
#include <QSystemTrayIcon>
#include <QIcon>

/// Cross-platform desktop notifications and taskbar integration.
///
/// Wraps QSystemTrayIcon for toast notifications and provides stubs
/// for platform-specific taskbar badge APIs (Win32 ITaskbarList3,
/// macOS NSApplication dock badge, Linux Unity launcher).
class NotificationService : public QObject {
    Q_OBJECT
public:
    explicit NotificationService(QObject *parent = nullptr);
    ~NotificationService() override;

    /// Initialise the system tray icon.  Must be called once before
    /// showNotification / updateTrayIcon.
    void init(QSystemTrayIcon *trayIcon = nullptr);

    /// Show a desktop toast notification.
    void showNotification(const QString &title, const QString &body);

    /// Update the taskbar / dock badge with a numeric count.
    /// 0 hides the badge.  Platform stubs for now — TODO: native impl.
    void updateTaskbarBadge(int count);

    /// Swap the tray icon between "has unread" and "no unread" states.
    void updateTrayIcon(bool hasUnread);

private:
    QSystemTrayIcon *tray_ = nullptr;
    QIcon iconNormal_;
    QIcon iconUnread_;
    int currentBadge_ = 0;
};
