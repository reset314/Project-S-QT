#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

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

// ViewModels
#include "viewmodels/ChatMessagesModel.h"
#include "viewmodels/ConversationListModel.h"
#include "viewmodels/ContactListModel.h"
#include "viewmodels/LoginViewModel.h"
#include "viewmodels/SettingsViewModel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("AI Chat");
    app.setOrganizationName("AIChat");
    app.setQuitOnLastWindowClosed(false);

    // ── Core infrastructure ────────────────────────────────────
    auto database = new Database(&app);
    database->initialize();

    auto keychain = new Keychain(&app);
    auto tokenManager = new TokenManager(keychain, &app);
    auto httpClient = new HttpClient(tokenManager, &app);

    auto wsClient = new ChatStreamClient(tokenManager, &app);
    wsClient->setBaseUrl("ws://localhost:8000"); // Configure via settings

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

    // ── ViewModels ─────────────────────────────────────────────
    auto chatMessagesModel    = new ChatMessagesModel(&app);
    auto conversationListModel = new ConversationListModel(&app);
    auto contactListModel     = new ContactListModel(&app);
    auto loginViewModel       = new LoginViewModel(authRepo, tokenManager, &app);
    auto settingsViewModel    = new SettingsViewModel(&app);

    // ── QML Engine ─────────────────────────────────────────────
    QQmlApplicationEngine engine;

    // Expose context properties for QML
    auto ctx = engine.rootContext();
    ctx->setContextProperty("chatMessagesModel",    chatMessagesModel);
    ctx->setContextProperty("conversationListModel", conversationListModel);
    ctx->setContextProperty("contactListModel",     contactListModel);
    ctx->setContextProperty("loginViewModel",       loginViewModel);
    ctx->setContextProperty("settingsViewModel",    settingsViewModel);
    ctx->setContextProperty("unreadTracker",        unreadTracker);
    ctx->setContextProperty("chatService",          chatService);

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
