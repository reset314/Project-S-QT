#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>
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
#include <QSettings>
#include <QDateTime>
#include <QUuid>
#include <QtConcurrent>
#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dwmapi.h>
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif
#endif

// Core
#include "core/storage/Database.h"
#include "core/auth/Keychain.h"
#include "core/auth/TokenManager.h"
#include "core/network/HttpClient.h"
#include "core/network/ChatStreamClient.h"
#include "core/theme/ThemeManager.h"
#include "core/utils/SecondaryWindow.h"

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
// WindowHelper — Win32 native window drag for frameless windows
// ---------------------------------------------------------------------------
#ifdef Q_OS_WIN
class WindowHelper : public QObject {
    Q_OBJECT
public:
    explicit WindowHelper(QObject *parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE void startSystemDrag(QWindow *window) {
        if (window) {
            ReleaseCapture();
            SendMessage(reinterpret_cast<HWND>(window->winId()),
                        WM_NCLBUTTONDOWN, HTCAPTION, 0);
        }
    }
};
#endif

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    qputenv("QT_SSL_BACKEND", "schannel");

    QApplication app(argc, argv);
    qRegisterMetaType<MessageDTO>("MessageDTO");
    app.setApplicationName("Project-S");
    app.setOrganizationName("Project-S");
    app.setQuitOnLastWindowClosed(false);
    app.setWindowIcon(QIcon(":/icons/app.svg"));

    // Redirect Qt debug output to stderr so it's visible in the terminal
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &, const QString &msg) {
        const char *prefix = "";
        switch (type) {
        case QtDebugMsg:    prefix = "DEBUG"; break;
        case QtWarningMsg:  prefix = "WARN";  break;
        case QtCriticalMsg: prefix = "CRIT";  break;
        case QtFatalMsg:    prefix = "FATAL"; break;
        default: break;
        }
        fprintf(stderr, "[%s] %s\n", prefix, qPrintable(msg));
        fflush(stderr);
    });

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

    // Read / generate stable device ID
    QString deviceId = database->getDeviceInfo("device_id");
    if (deviceId.isEmpty()) {
        deviceId = QUuid::createUuid().toString(QUuid::Id128);
        database->setDeviceInfo("device_id", deviceId);
    }
    wsClient->setDeviceId(deviceId);

    // Read server URL from persistent settings (set in registry or .ini)
    QString serverUrl = QSettings("Project-S", "Project-S")
                            .value("settings/serverUrl", "https://www.p314.top").toString();
    tokenManager->setServerUrl(serverUrl);
    httpClient->setBaseUrl(serverUrl);

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
    auto convRepo     = new ConversationRepository(httpClient, database, &app);
    auto aiUserRepo   = new AIUserRepository(httpClient, &app);
    auto userRepo     = new UserRepository(httpClient, &app);
    auto memoryRepo   = new MemoryRepository(httpClient, &app);
    auto profileRepo  = new ProfileRepository(httpClient, &app);
    auto uploadRepo   = new UploadRepository(httpClient, &app);
    auto expansionRepo = new ExpansionRepository(httpClient, &app);
    auto callRepo     = new CallRepository(httpClient, &app);
    auto ttsRepo      = new TTSRepository(httpClient, &app);

    auto chatRepo     = new ChatRepository(httpClient, database, &app);

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
                         qDebug() << "AppState: WS connected, setting connectionStatus";
                         appState->setConnectionStatus(QStringLiteral("connected"));
                         qDebug() << "AppState: connectionStatus now" << appState->connectionStatus();
                     });
    QObject::connect(wsClient, &ChatStreamClient::disconnected,
                     appState, [appState]() {
                         qDebug() << "AppState: WS disconnected, setting connectionStatus";
                         appState->setConnectionStatus(QStringLiteral("disconnected"));
                     });

    // Wire ChatStreamClient → ready signal → sync on reconnect
    QObject::connect(wsClient, &ChatStreamClient::ready,
                     &app, [wsClient]() {
        QSettings settings;
        int lastSeq = settings.value("event/last_seq", 0).toInt();
        if (lastSeq > 0) {
            qDebug() << "Main: auth_ok, sending sync from seq" << lastSeq;
            wsClient->sendSync(lastSeq);
        }
    });

    // Wire UnreadTracker → tray icon
    QObject::connect(unreadTracker, &UnreadTracker::totalUnreadChanged,
                     notificationService, [notificationService](int total) {
                         notificationService->updateTrayIcon(total > 0);
                     });

    // ── System Tray Icon ───────────────────────────────────────
    auto trayIcon = new QSystemTrayIcon(&app);
    trayIcon->setToolTip("Project-S");

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

    // Tray icon: only show if a real icon is available
    QIcon appIcon(":/icons/app.svg");
    if (!appIcon.isNull()) {
        trayIcon->setIcon(appIcon);
        trayIcon->show();
    }

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

    // Wire ChatStreamClient → eventReceived → handle events
    QObject::connect(wsClient, &ChatStreamClient::eventReceived,
                     &app, [chatMessagesModel, chatRepo, database, unreadTracker](
                                int seq, const QString &eventType,
                                const QJsonObject &payload) {
        // Process event
        if (eventType == "message.sent") {
            QString aiUserId = payload.value("ai_user_id").toString();
            QString messageId = payload.value("message_id").toString();
            QString content = payload.value("content").toString();
            QString msgType = payload.value("msg_type").toString();

            if (!aiUserId.isEmpty() && !messageId.isEmpty()) {
                // 防重复：检查是否已有同内容的乐观消息（无 server_id）
                // 避免发消息者收到自己的广播后重复渲染
                if (chatMessagesModel->activeAiUserId() == aiUserId) {
                    bool found = false;
                    for (int i = 0; i < chatMessagesModel->rowCount(); ++i) {
                        const auto &m = chatMessagesModel->messageAt(i);
                        if (m.serverId.empty() &&
                            m.senderType == "user" &&
                            m.content == content.toStdString()) {
                            found = true;
                            break;
                        }
                    }
                    if (found) return; // 已有乐观消息，忽略事件
                }

                MessageDTO msg;
                msg.serverId = messageId.toStdString();
                msg.aiUserId = aiUserId.toStdString();
                msg.content = content.toStdString();
                msg.msgType = msgType.toStdString();
                msg.senderType = "user";
                msg.timestamp = QDateTime::currentDateTimeUtc()
                                    .toString(Qt::ISODate).toStdString();
                msg.isComplete = true;

                chatRepo->upsertMessage(msg, aiUserId);
                if (chatMessagesModel->activeAiUserId() == aiUserId)
                    chatMessagesModel->appendMessage(msg);
            }
        }
        else if (eventType == "message.revoked") {
            QString messageId = payload.value("message_id").toString();
            if (!messageId.isEmpty())
                chatMessagesModel->markRevoked(messageId);
        }
        else if (eventType == "message.edited") {
            QString messageId = payload.value("message_id").toString();
            QString newContent = payload.value("new_content").toString();
            if (!messageId.isEmpty() && !newContent.isEmpty())
                chatMessagesModel->updateContentByServerId(messageId, newContent);
        }
        else if (eventType == "message.deleted") {
            QString messageId = payload.value("message_id").toString();
            if (!messageId.isEmpty())
                chatMessagesModel->markDeleted(messageId);
        }
        else if (eventType == "messages.rolled_back") {
            QString anchorId = payload.value("anchor_message_id").toString();
            if (!anchorId.isEmpty())
                chatMessagesModel->rollbackToAnchor(anchorId);
        }
        else if (eventType == "aiuser.created" ||
                 eventType == "aiuser.updated" ||
                 eventType == "aiuser.deleted") {
            qDebug() << "Main: AI user changed (" << eventType << "), will refresh on next page load";
        }

        // Update event seq in QSettings
        if (seq > 0) {
            QSettings settings;
            settings.setValue("event/last_seq", seq);
        }
    });

    // ── Post-login initialization pipeline ────────────────────────
    QObject::connect(loginViewModel, &LoginViewModel::loginSuccess,
                     &app, [wsClient, tokenManager, httpClient, database,
                            convRepo, aiUserRepo,
                            conversationListModel, contactListModel,
                            chatService, syncService, unreadTracker]() {
        // 1. Connect WebSocket
        QString httpUrl = tokenManager->serverUrl();
        QString wsUrl = httpUrl;
        wsUrl.replace("https://", "wss://").replace("http://", "ws://");
        wsUrl += "/ws/connect";
        wsClient->connectToServer(wsUrl, tokenManager->accessToken(),
                                    database->getDeviceInfo("device_id"), "qt");

        // 2. Load conversations — populate sidebar
        auto convResult = convRepo->listConversations();
        if (convResult) {
            conversationListModel->replaceAll(*convResult);
            qDebug() << "Loaded" << convResult->size() << "conversations";
        }

        // 3. Load AI users (contacts) — populate sidebar
        auto aiResult = aiUserRepo->listAIUsers();
        if (aiResult) {
            contactListModel->replaceAll(*aiResult);
            qDebug() << "Loaded" << aiResult->size() << "AI users";
        } else {
            qWarning() << "Failed to load AI users:" << aiResult.error().code.c_str();
        }
    });

    // ── ChatService → ChatMessagesModel ───────────────────────────
    // Incremental updates — append/update single rows instead of full
    // model reset (replaceAll) which causes ListView flicker.

    QObject::connect(chatService, &ChatService::messageAppended,
                     &app, [chatMessagesModel](
                                const QString &aiUserId, const MessageDTO &msg) {
        if (chatMessagesModel->activeAiUserId() == aiUserId)
            chatMessagesModel->appendMessage(msg);
    });

    QObject::connect(chatService, &ChatService::messageContentUpdated,
                     &app, [chatMessagesModel](
                                const QString &aiUserId, const QString &clientUuid,
                                const QJsonObject &content) {
        if (chatMessagesModel->activeAiUserId() == aiUserId) {
            int row = chatMessagesModel->findByClientUuid(clientUuid);
            if (row >= 0)
                chatMessagesModel->updateContent(row, content);
        }
    });

    QObject::connect(chatService, &ChatService::messageMarkedComplete,
                     &app, [chatMessagesModel](
                                const QString &aiUserId, const QString &clientUuid,
                                const QString &serverId) {
        if (chatMessagesModel->activeAiUserId() == aiUserId) {
            int row = chatMessagesModel->findByClientUuid(clientUuid);
            if (row >= 0)
                chatMessagesModel->markComplete(row, serverId);
        }
    });

    // ── ChatMessagesModel load wiring ───────────────────────────
    // When QML requests switching AI users, load history from server
    // and populate the model. Runs synchronously on main thread to
    // avoid QSqlDatabase / QNetworkAccessManager cross-thread issues.
    QObject::connect(chatMessagesModel, &ChatMessagesModel::loadRequested,
                     &app, [chatMessagesModel, chatRepo, syncService, unreadTracker](
                                const QString &aiUserId) {
        auto messages = chatRepo->getHistory(aiUserId, QString{}, 50);
        if (messages) {
            chatMessagesModel->onHistoryLoaded(*messages);
            qDebug() << "ChatMessagesModel: loaded" << messages->size()
                     << "messages for" << aiUserId;
        } else {
            qWarning() << "ChatMessagesModel: failed to load history for"
                       << aiUserId;
        }
        if (unreadTracker)
            unreadTracker->clearAndMarkRead(aiUserId);
    });

    // ── Pagination: load older messages on scroll-to-top ───────────
    QObject::connect(chatMessagesModel, &ChatMessagesModel::loadOlderRequested,
                     &app, [chatMessagesModel, chatRepo](
                                const QString &aiUserId,
                                const QString &beforeTimestamp) {
        qDebug() << "ChatMessagesModel: loadOlder before" << beforeTimestamp;
        // Use the oldest message timestamp as the "before" cursor
        auto messages = chatRepo->getHistory(aiUserId, beforeTimestamp, 30);
        if (messages) {
            chatMessagesModel->onOlderMessagesLoaded(*messages);
            qDebug() << "ChatMessagesModel: loaded" << messages->size()
                     << "older messages for" << aiUserId;
        }
    });

    // ── QML Engine ─────────────────────────────────────────────
    QQmlApplicationEngine engine;

    // Register ThemeConfig as a singleton via C++ — more reliable than
    // pragma Singleton + qmldir in Qt 6.
    QQmlComponent themeComponent(&engine, QUrl("qrc:/qml/theme/ThemeConfig.qml"));
    QObject *themeConfig = themeComponent.create();
    if (themeConfig) {
        engine.rootContext()->setContextProperty("Theme", themeConfig);
    } else {
        qWarning() << "Failed to load ThemeConfig:" << themeComponent.errorString();
    }

    auto ctx = engine.rootContext();

    // ── ThemeManager ─────────────────────────────────────────────
    auto themeManager = new ThemeManager(&app);
    ctx->setContextProperty("themeManager", themeManager);

    // Read saved theme from QSettings, fall back to built-in light.css
    QString savedTheme = QSettings("Project-S", "Project-S")
        .value("settings/themePath", ":/themes/light.css").toString();
    themeManager->setTheme(savedTheme);

    auto secondaryWindow = new SecondaryWindow(&engine, &app);
    ctx->setContextProperty("secondaryWindow", secondaryWindow);

#ifdef Q_OS_WIN
    auto windowHelper = new WindowHelper(&app);
    ctx->setContextProperty("windowHelper", windowHelper);
#endif

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

#ifdef Q_OS_WIN
    // Windows 11 native rounded corners for frameless window
    if (auto *window = qobject_cast<QWindow *>(engine.rootObjects().first())) {
        int cornerPref = DWMWCP_ROUND;
        DwmSetWindowAttribute(reinterpret_cast<HWND>(window->winId()),
                              DWMWA_WINDOW_CORNER_PREFERENCE,
                              &cornerPref, sizeof(cornerPref));
    }
#endif

    return app.exec();
}

#include "main.moc"
