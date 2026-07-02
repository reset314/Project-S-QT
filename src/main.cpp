#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QShortcut>
#include <QKeySequence>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QWindow>

// Core
#include "core/storage/Database.h"
#include "core/auth/Keychain.h"
#include "core/auth/TokenManager.h"
#include "core/network/HttpClient.h"
#include "core/network/ChatStreamClient.h"

// Repositories
#include "repositories/AuthRepository.h"
#include "repositories/ChatRepository.h"
#include "repositories/ConversationRepository.h"
#include "repositories/AIUserRepository.h"
#include "repositories/UserRepository.h"
#include "repositories/MemoryRepository.h"
#include "repositories/ProfileRepository.h"
#include "repositories/UploadRepository.h"
#include "repositories/ExpansionRepository.h"
#include "repositories/CallRepository.h"
#include "repositories/TTSRepository.h"

// Services
#include "services/ChatService.h"
#include "services/SyncService.h"
#include "services/UnreadTracker.h"
#include "services/NotificationService.h"
#include "services/CallManager.h"

// Platform
#include "platform/WinTaskbar.h"
#include "platform/MacDock.h"
#include "platform/LinuxDesktop.h"

// ViewModels
#include "viewmodels/ChatMessagesModel.h"
#include "viewmodels/ConversationListModel.h"
#include "viewmodels/ContactListModel.h"
#include "viewmodels/LoginViewModel.h"
#include "viewmodels/SettingsViewModel.h"

// ---------------------------------------------------------------------------
// AppState — a small QObject that bridges C++ signals to QML properties
// ---------------------------------------------------------------------------

class AppState : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ isLoggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString currentUsername READ currentUsername NOTIFY currentUsernameChanged)
    Q_PROPERTY(int totalUnread READ totalUnread NOTIFY totalUnreadChanged)
    Q_PROPERTY(bool windowVisible READ isWindowVisible NOTIFY windowVisibleChanged)

public:
    explicit AppState(QObject *parent = nullptr) : QObject(parent) {}

    bool isLoggedIn() const { return loggedIn_; }
    void setLoggedIn(bool v) {
        if (loggedIn_ != v) { loggedIn_ = v; emit loggedInChanged(); }
    }

    QString connectionStatus() const { return connectionStatus_; }
    void setConnectionStatus(const QString &v) {
        if (connectionStatus_ != v) { connectionStatus_ = v; emit connectionStatusChanged(); }
    }

    QString currentUsername() const { return currentUsername_; }
    void setCurrentUsername(const QString &v) {
        if (currentUsername_ != v) { currentUsername_ = v; emit currentUsernameChanged(); }
    }

    int totalUnread() const { return totalUnread_; }
    void setTotalUnread(int v) {
        if (totalUnread_ != v) { totalUnread_ = v; emit totalUnreadChanged(); }
    }

    bool isWindowVisible() const { return windowVisible_; }
    void setWindowVisible(bool v) {
        if (windowVisible_ != v) { windowVisible_ = v; emit windowVisibleChanged(); }
    }

signals:
    void loggedInChanged();
    void connectionStatusChanged();
    void currentUsernameChanged();
    void totalUnreadChanged();
    void windowVisibleChanged();

private:
    bool loggedIn_ = false;
    QString connectionStatus_ = QStringLiteral("disconnected");
    QString currentUsername_;
    int totalUnread_ = 0;
    bool windowVisible_ = true;
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("AI Chat");
    app.setOrganizationName("AIChat");
    app.setQuitOnLastWindowClosed(false);

    // ── Core infrastructure ────────────────────────────────────
    auto database = new Database();
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                     + QDir::separator() + "ai_chat_desktop.db";
    QDir().mkpath(QFileInfo(dbPath).absolutePath());
    database->open(dbPath);

    auto tokenManager = new TokenManager(&app);
    auto httpClient = new HttpClient(&app);
    httpClient->setTokenManager(tokenManager);

    auto wsClient = new ChatStreamClient(&app);
    // WS URL is constructed at connectToServer() time from the server URL
    // (e.g. http://localhost:8000 → ws://localhost:8000/ws/connect)

    // ── Platform integration ───────────────────────────────────
    PlatformIntegration *platform = nullptr;
#ifdef Q_OS_WIN
    platform = new WinTaskbar(&app);
#elif defined(Q_OS_MACOS)
    platform = new MacDock(&app);
#else
    platform = new LinuxDesktop(&app);
#endif

    // ── Repositories ───────────────────────────────────────────
    auto authRepo     = new AuthRepository(httpClient, tokenManager, &app);
    auto convRepo     = new ConversationRepository(database, httpClient, &app);
    auto aiUserRepo   = new AIUserRepository(database, httpClient, &app);
    auto userRepo     = new UserRepository(database, httpClient, &app);
    auto memoryRepo   = new MemoryRepository(database, httpClient, &app);
    auto profileRepo  = new ProfileRepository(database, httpClient, &app);
    auto uploadRepo   = new UploadRepository(httpClient, &app);
    auto expansionRepo = new ExpansionRepository(database, httpClient, &app);
    auto callRepo     = new CallRepository(database, httpClient, &app);
    auto ttsRepo      = new TTSRepository(database, httpClient, &app);

    auto chatRepo     = new ChatRepository(database, httpClient, wsClient, &app);

    // ── Services ───────────────────────────────────────────────
    auto notificationService = new NotificationService(&app);
    auto syncService  = new SyncService(chatRepo, database, &app);

    auto chatService  = new ChatService(chatRepo, wsClient, syncService, &app);
    auto unreadTracker = new UnreadTracker(database, convRepo, &app);
    unreadTracker->init();

    auto callManager  = new CallManager(callRepo, tokenManager, &app);

    // ── AppState bridge ────────────────────────────────────────
    auto appState = new AppState(&app);

    // Wire TokenManager → AppState
    QObject::connect(tokenManager, &TokenManager::loginStateChanged,
                     appState, [appState](bool loggedIn) {
                         appState->setLoggedIn(loggedIn);
                     });

    // Wire UnreadTracker → AppState + Platform badge
    QObject::connect(unreadTracker, &UnreadTracker::totalUnreadChanged,
                     appState, [appState, platform](int total) {
                         appState->setTotalUnread(total);
                         if (platform)
                             platform->updateBadge(total);
                     });

    // Wire ChatStreamClient → AppState connection status
    QObject::connect(wsClient, &ChatStreamClient::connected,
                     appState, [appState]() {
                         appState->setConnectionStatus(QStringLiteral("connected"));
                     });
    QObject::connect(wsClient, &ChatStreamClient::disconnected,
                     appState, [appState]() {
                         appState->setConnectionStatus(QStringLiteral("disconnected"));
                     });

    // Wire UnreadTracker → tray icon
    QObject::connect(unreadTracker, &UnreadTracker::totalUnreadChanged,
                     notificationService, [notificationService](int total) {
                         notificationService->updateTrayIcon(total > 0);
                     });

    // ── System Tray Icon ───────────────────────────────────────
    auto trayIcon = new QSystemTrayIcon(&app);
    trayIcon->setToolTip("AI Chat");

    auto trayMenu = new QMenu();

    auto showAction = trayMenu->addAction(QObject::tr("Show / Hide"));
    QObject::connect(showAction, &QAction::triggered, &app, [&app, appState]() {
        if (app.topLevelWindows().isEmpty()) return;
        auto *window = app.topLevelWindows().first();
        if (window->isVisible()) {
            window->hide();
            appState->setWindowVisible(false);
        } else {
            window->show();
            window->raise();
            window->requestActivate();
            appState->setWindowVisible(true);
        }
    });

    trayMenu->addSeparator();

    auto quitAction = trayMenu->addAction(QObject::tr("Quit"));
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);

    // Use a default icon; callers should replace with a real application icon
    QIcon fallbackIcon(":/icons/tray_normal.png");
    if (!fallbackIcon.isNull())
        trayIcon->setIcon(fallbackIcon);

    trayIcon->show();

    // Wire tray activation (click) → show window
    QObject::connect(trayIcon, &QSystemTrayIcon::activated,
                     &app, [&app, appState](QSystemTrayIcon::ActivationReason reason) {
                         if (reason == QSystemTrayIcon::Trigger ||
                             reason == QSystemTrayIcon::DoubleClick) {
                             if (app.topLevelWindows().isEmpty()) return;
                             auto *window = app.topLevelWindows().first();
                             if (!window->isVisible()) {
                                 window->show();
                                 window->raise();
                                 window->requestActivate();
                                 appState->setWindowVisible(true);
                             }
                         }
                     });

    // Initialise the notification service with our tray icon
    notificationService->init(trayIcon);

    // ── Global Shortcut ─ Ctrl+Shift+A → show window ──────────
    auto globalShortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+A")), &app);
    QObject::connect(globalShortcut, &QShortcut::activated, &app, [&app, appState]() {
        if (app.topLevelWindows().isEmpty()) return;
        auto *window = app.topLevelWindows().first();
        if (!window->isVisible()) {
            window->show();
            window->raise();
            window->requestActivate();
            appState->setWindowVisible(true);
        } else {
            // Already visible — bring to front
            window->raise();
            window->requestActivate();
        }
    });

    // ── Minimise-to-tray — intercept window close ──────────────
    // QApplication::setQuitOnLastWindowClosed(false) above means
    // closing the window only hides it instead of quitting the app.
    // When the user explicitly clicks Quit from the tray menu,
    // QApplication::quit() will still terminate the process.

    // ── ViewModels ─────────────────────────────────────────────
    auto chatMessagesModel    = new ChatMessagesModel(&app);
    auto conversationListModel = new ConversationListModel(&app);
    auto contactListModel     = new ContactListModel(&app);
    auto loginViewModel       = new LoginViewModel(authRepo, tokenManager, &app);
    auto settingsViewModel    = new SettingsViewModel(&app);

    // ── QML Engine ─────────────────────────────────────────────
    QQmlApplicationEngine engine;

    auto ctx = engine.rootContext();
    ctx->setContextProperty("chatMessagesModel",    chatMessagesModel);
    ctx->setContextProperty("conversationListModel", conversationListModel);
    ctx->setContextProperty("contactListModel",     contactListModel);
    ctx->setContextProperty("loginViewModel",       loginViewModel);
    ctx->setContextProperty("settingsViewModel",    settingsViewModel);
    ctx->setContextProperty("unreadTracker",        unreadTracker);
    ctx->setContextProperty("chatService",          chatService);

    // AppState singleton for QML
    ctx->setContextProperty("appState", appState);

    // Repositories exposed for pages that need direct access
    ctx->setContextProperty("aiUserRepo",    aiUserRepo);
    ctx->setContextProperty("profileRepo",   profileRepo);
    ctx->setContextProperty("ttsRepo",       ttsRepo);
    ctx->setContextProperty("expansionRepo", expansionRepo);
    ctx->setContextProperty("tokenManager",  tokenManager);
    ctx->setContextProperty("userRepo",      userRepo);
    ctx->setContextProperty("callManager",   callManager);

    engine.load(QUrl("qrc:/qml/main.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

#include "main.moc"
