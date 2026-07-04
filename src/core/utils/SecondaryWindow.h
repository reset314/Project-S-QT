#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QVariantMap>

class SecondaryWindow : public QObject {
    Q_OBJECT
public:
    explicit SecondaryWindow(QQmlApplicationEngine *engine, QObject *parent = nullptr);
    Q_INVOKABLE QQuickWindow *open(const QString &qmlPath, const QVariantMap &props = {});

private:
    QQmlApplicationEngine *engine_;
};
