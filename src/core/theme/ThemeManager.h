#pragma once
#include <QObject>
#include <QString>

class StyleEngine; // aqt-stylesheets

class ThemeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentThemePath READ currentThemePath NOTIFY themeChanged)
public:
    explicit ThemeManager(QObject *parent = nullptr);

    Q_INVOKABLE void loadTheme(const QString &cssPath);
    Q_INVOKABLE void reload();
    Q_INVOKABLE QString currentThemePath() const { return currentThemePath_; }

signals:
    void themeChanged();

private:
    StyleEngine *engine_ = nullptr;
    QString currentThemePath_;
};
