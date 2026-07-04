#include "SecondaryWindow.h"

SecondaryWindow::SecondaryWindow(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent), engine_(engine) {}

QQuickWindow *SecondaryWindow::open(const QString &qmlPath, const QVariantMap &props) {
    QQmlComponent comp(engine_, QUrl(qmlPath));
    if (comp.isError()) {
        qWarning() << "SecondaryWindow: failed to load" << qmlPath << comp.errorString();
        return nullptr;
    }
    QObject *obj = comp.createWithInitialProperties(props);
    QQuickWindow *win = qobject_cast<QQuickWindow *>(obj);
    if (win) {
        win->setTitle("Project-S");
        win->setMinimumSize(QSize(400, 300));
        win->show();
    }
    return win;
}
