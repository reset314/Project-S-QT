#include "LinuxDesktop.h"

#ifdef Q_OS_LINUX

#include <QDebug>

// ---------------------------------------------------------------------------
// When a full D-Bus implementation is needed, link against Qt6::DBus or
// libdbus-1 and uncomment the sections below.
//
// Unity Launcher badge (com.canonical.Unity.LauncherEntry):
//   QDBusMessage msg = QDBusMessage::createMethodCall(
//       "com.canonical.Unity", "/", "com.canonical.Unity.LauncherEntry",
//       "Update");
//   msg << "application://aichat.desktop"
//       << QVariantMap{{"count", count}, {"count-visible", count > 0}};
//   QDBusConnection::sessionBus().send(msg);
//
// org.freedesktop.Notifications:
//   See the Desktop Notifications Specification for the full interface.
// ---------------------------------------------------------------------------

LinuxDesktop::LinuxDesktop(QObject *parent)
    : PlatformIntegration(parent)
{
    qDebug() << "LinuxDesktop: initialised (stub — D-Bus not linked)";
}

LinuxDesktop::~LinuxDesktop()
{
}

void LinuxDesktop::updateBadge(int count)
{
    if (currentBadge_ == count)
        return;
    currentBadge_ = count;

    // TODO: Full D-Bus Unity Launcher implementation (see comment above)
    // Send dbus message with count and count-visible properties.

    qDebug() << "LinuxDesktop: badge" << count << "(stub)";
}

void LinuxDesktop::showNotification(const QString &title, const QString &body)
{
    // TODO: org.freedesktop.Notifications.Notify via D-Bus
    // The desktop shell typically also handles tray icon notifications,
    // so this is a stub for direct D-Bus notification delivery.
    qDebug() << "LinuxDesktop: notification" << title << "-" << body << "(stub)";
}

void LinuxDesktop::requestAttention()
{
    // TODO: D-Bus attention request — sends a
    // org.freedesktop.Notifications.Notify with urgency=critical,
    // or KDE-specific / GNOME-specific D-Bus calls.
    qDebug() << "LinuxDesktop: requestAttention (stub)";
}

#endif // Q_OS_LINUX
