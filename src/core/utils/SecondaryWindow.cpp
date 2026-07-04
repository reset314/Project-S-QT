#include "SecondaryWindow.h"
#include <QQuickView>

SecondaryWindow::SecondaryWindow(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent), engine_(engine) {}

QQuickWindow *SecondaryWindow::open(const QString &qmlPath, const QVariantMap &props) {
    auto *view = new QQuickView(engine_, nullptr);
    view->setTitle("Project-S");
    view->setMinimumSize(QSize(400, 300));
    view->resize(600, 500);
    // Copy context properties from main engine
    auto *rootCtx = engine_->rootContext();
    for (const auto &k : {"themeManager", "chatService", "chatMessagesModel", "contactListModel",
                           "conversationListModel", "unreadTracker", "settingsViewModel",
                           "aiUserRepo", "profileRepo", "ttsRepo", "expansionRepo",
                           "tokenManager", "userRepo", "callManager", "windowHelper", "appState"})
        view->rootContext()->setContextProperty(k, rootCtx->contextProperty(k));
    view->setSource(QUrl(qmlPath));
    if (view->status() == QQuickView::Error) {
        qWarning() << "SecondaryWindow: failed" << qmlPath; delete view; return nullptr;
    }
    view->show();
    return view;
}
