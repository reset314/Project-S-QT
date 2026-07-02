#pragma once
#include "PlatformIntegration.h"

#ifdef Q_OS_MACOS

/// macOS dock integration via NSApplication dockTile.
///
/// Sets the application dock tile badge label using the Cocoa
/// NSApp dockTile API.  A stub implementation that logs badge
/// changes is provided; a full implementation requires
/// Objective-C++ interop.
class MacDock : public PlatformIntegration {
    Q_OBJECT
public:
    explicit MacDock(QObject *parent = nullptr);
    ~MacDock() override;

    void updateBadge(int count) override;
    void showNotification(const QString &title, const QString &body) override;
    void requestAttention() override;

private:
    int currentBadge_ = 0;
};

#else
// Non-macOS stub — MacDock is not available on other platforms.
class MacDock : public PlatformIntegration {
    Q_OBJECT
public:
    explicit MacDock(QObject *parent = nullptr)
        : PlatformIntegration(parent) {}
};
#endif // Q_OS_MACOS
