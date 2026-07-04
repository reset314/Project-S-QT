#include "ThemeManager.h"
#include <StyleEngine.hpp> // aqt-stylesheets
#include <QFileInfo>
#include <QDir>
#include <QDebug>

ThemeManager::ThemeManager(QObject *parent) : QObject(parent) {
    engine_ = new StyleEngine(this);
}

void ThemeManager::loadTheme(const QString &cssPath) {
    QFileInfo fi(cssPath);
    if (!fi.exists()) {
        qWarning() << "ThemeManager: CSS file not found:" << cssPath;
        return;
    }
    engine_->loadStyleSheet(cssPath);
    currentThemePath_ = cssPath;
    emit themeChanged();
    qDebug() << "ThemeManager: loaded" << cssPath;
}

void ThemeManager::reload() {
    if (!currentThemePath_.isEmpty())
        loadTheme(currentThemePath_);
}
