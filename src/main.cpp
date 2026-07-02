#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("AI Chat");
    app.setOrganizationName("AIChat");
    app.setQuitOnLastWindowClosed(false);

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
