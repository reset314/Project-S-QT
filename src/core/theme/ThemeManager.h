#pragma once
#include <QObject>
#include <QString>

// Simple path tracker for the current CSS theme.
// StyleEngineSetup in QML handles actual CSS loading via styleSheetSource.
class ThemeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString themeUrl READ themeUrl NOTIFY themeChanged)
public:
    explicit ThemeManager(QObject *parent = nullptr);

    Q_INVOKABLE void setTheme(const QString &url);
    Q_INVOKABLE void reload();
    QString themeUrl() const { return currentUrl_; }

signals:
    void themeChanged();

private:
    QString currentUrl_;
};
