#include "ThemeManager.h"
#include <QDebug>
#include <QSettings>

ThemeManager::ThemeManager(QObject *parent) : QObject(parent) {}

void ThemeManager::setTheme(const QString &url) {
    if (url == currentUrl_) return;
    currentUrl_ = url;
    QSettings("Project-S", "Project-S").setValue("settings/themePath", url);
    emit themeChanged();
    qDebug() << "ThemeManager: set theme" << url;
}

void ThemeManager::reload() {
    if (!currentUrl_.isEmpty()) {
        QString saved = currentUrl_;
        currentUrl_.clear();
        setTheme(saved);
    }
}
