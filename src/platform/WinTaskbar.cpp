#include "WinTaskbar.h"

#ifdef Q_OS_WIN

#include <QDebug>
#include <QApplication>

// ---------------------------------------------------------------------------
// When a full ITaskbarList3 implementation is needed, uncomment and link
// against ole32 and uuid:
//
// #include <windows.h>
// #include <shobjidl.h>
// #pragma comment(lib, "ole32.lib")
// #pragma comment(lib, "uuid.lib")
//
// class WinTaskbarPrivate { ... ITaskbarList3 *pTaskbarList = nullptr; };
// ---------------------------------------------------------------------------

WinTaskbar::WinTaskbar(QObject *parent)
    : PlatformIntegration(parent)
{
    qDebug() << "WinTaskbar: initialised (stub — ITaskbarList3 not linked)";
}

WinTaskbar::~WinTaskbar()
{
}

void WinTaskbar::updateBadge(int count)
{
    if (currentBadge_ == count)
        return;
    currentBadge_ = count;

    // TODO: Full ITaskbarList3 implementation
    //  1. Get HWND from QApplication::topLevelWindows()
    //  2. CoCreateInstance(CLSID_TaskbarList, ...)
    //  3. pTaskbarList->HrInit()
    //  4. If count > 0:
    //       pTaskbarList->SetOverlayIcon(hwnd, icon, L"3")
    //     Else:
    //       pTaskbarList->SetOverlayIcon(hwnd, nullptr, nullptr)

    qDebug() << "WinTaskbar: badge" << count << "(stub)";
}

void WinTaskbar::showNotification(const QString &title, const QString &body)
{
    // Notifications are handled by QSystemTrayIcon in NotificationService.
    // This stub exists so callers can go through a single PlatformIntegration
    // interface.  For Win32-native toast notifications, use the Windows
    // ToastNotification API with an AUMID-registered shortcut.
    qDebug() << "WinTaskbar: notification" << title << "-" << body << "(stub)";
}

void WinTaskbar::requestAttention()
{
    // TODO: FlashWindowEx with FLASHW_TRAY | FLASHW_TIMERNOFIG
    qDebug() << "WinTaskbar: requestAttention (stub)";
}

#endif // Q_OS_WIN
