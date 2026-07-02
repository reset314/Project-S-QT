#pragma once
#include <QObject>
#include <QString>

/// Abstract base class for platform-specific desktop integration.
///
/// Subclasses implement native APIs for taskbar/dock badge overlays,
/// desktop notifications, and window attention requests.
///
/// Usage:
///   #ifdef Q_OS_WIN
///   auto platform = new WinTaskbar(&app);
///   #elif defined(Q_OS_MACOS)
///   auto platform = new MacDock(&app);
///   #else
///   auto platform = new LinuxDesktop(&app);
///   #endif
///   platform->updateBadge(3);
class PlatformIntegration : public QObject {
    Q_OBJECT
public:
    explicit PlatformIntegration(QObject *parent = nullptr)
        : QObject(parent) {}

    /// Show a numeric badge overlay on the taskbar / dock icon.
    /// Pass 0 to clear the badge.  Default implementation is a no-op.
    virtual void updateBadge(int count) { Q_UNUSED(count) }

    /// Show a platform-native desktop notification.
    /// Default implementation uses QSystemTrayIcon::showMessage if a tray
    /// icon is available; otherwise it is a no-op.
    virtual void showNotification(const QString &title, const QString &body)
    {
        Q_UNUSED(title)
        Q_UNUSED(body)
    }

    /// Flash the taskbar / dock icon to request user attention.
    /// Default implementation is a no-op.
    virtual void requestAttention() {}
};
