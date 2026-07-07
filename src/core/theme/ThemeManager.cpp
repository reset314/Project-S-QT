#include "ThemeManager.h"
#include "../vendor/aqt-stylesheets/src/StyleEngineSetup.hpp"
#include <QDebug>
#include <QSettings>
#include <QQmlEngine>
#include <QJSEngine>

ThemeManager::ThemeManager(QObject *parent) : QObject(parent) {
    engine_ = new aqt::stylesheets::StyleEngineSetup(this);
    // Set initial theme from saved settings
    QString saved = QSettings("Project-S", "Project-S")
        .value("settings/themePath", "qrc:/themes/light.css").toString();
    currentUrl_ = saved;
    if (!saved.isEmpty()) {
        engine_->setStyleSheetSource(QUrl(saved));
        qDebug() << "ThemeManager: initial theme" << saved;
    }
}

ThemeManager::~ThemeManager() = default;

void ThemeManager::setTheme(const QString &url) {
    if (url == currentUrl_) return;
    currentUrl_ = url;
    QSettings("Project-S", "Project-S").setValue("settings/themePath", url);
    if (engine_) {
        engine_->setStyleSheetSource(QUrl(url));
        qDebug() << "ThemeManager: CSS loaded from" << url;
    }
    emit themeChanged();
}

void ThemeManager::reload() {
    if (!currentUrl_.isEmpty()) {
        if (engine_) {
            engine_->setStyleSheetSource(QUrl(currentUrl_));
            qDebug() << "ThemeManager: reloaded CSS from" << currentUrl_;
        }
        emit themeChanged();
    }
}
