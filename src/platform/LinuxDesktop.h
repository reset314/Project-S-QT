#pragma once
#include "PlatformIntegration.h"

#ifdef Q_OS_LINUX

/// Linux desktop integration via D-Bus.
///
/// Communicates with the desktop environment using D-Bus protocols:
///   - Unity Launcher API (com.canonical.Unity.LauncherEntry) for badges
///   - org.freedesktop.Notifications for desktop notifications
///   - KDE / GNOME specific attention requests
///
/// This is a stub implementation that logs actions.  A full
/// implementation requires QtDBus or a libdbus-1 integration.
class LinuxDesktop : public PlatformIntegration {
    Q_OBJECT
public:
    explicit LinuxDesktop(QObject *parent = nullptr);
    ~LinuxDesktop() override;

    void updateBadge(int count) override;
    void showNotification(const QString &title, const QString &body) override;
    void requestAttention() override;

private:
    int currentBadge_ = 0;
};

#else
// Non-Linux stub — LinuxDesktop is not available on other platforms.
class LinuxDesktop : public PlatformIntegration {
    Q_OBJECT
public:
    explicit LinuxDesktop(QObject *parent = nullptr)
        : PlatformIntegration(parent) {}
};
#endif // Q_OS_LINUX
