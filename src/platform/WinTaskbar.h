#pragma once
#include "PlatformIntegration.h"

#ifdef Q_OS_WIN

/// Windows taskbar integration via ITaskbarList3 COM interface.
///
/// Uses the Windows ITaskbarList3 COM API to set overlay icons and
/// badge counts on the taskbar button.
///
/// This is a stub implementation that logs badge changes.  A full
/// implementation requires:
///   - CoInitializeEx in the application startup
///   - A valid HWND from QWindow::winId()
///   - The ITaskbarList3 COM interface
class WinTaskbar : public PlatformIntegration {
    Q_OBJECT
public:
    explicit WinTaskbar(QObject *parent = nullptr);
    ~WinTaskbar() override;

    void updateBadge(int count) override;
    void showNotification(const QString &title, const QString &body) override;
    void requestAttention() override;

private:
    int currentBadge_ = 0;
};

#else
// Non-Windows stub — WinTaskbar is not available on other platforms.
class WinTaskbar : public PlatformIntegration {
    Q_OBJECT
public:
    explicit WinTaskbar(QObject *parent = nullptr)
        : PlatformIntegration(parent) {}
};
#endif // Q_OS_WIN
