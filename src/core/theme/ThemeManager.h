#pragma once
#include <QObject>
#include <QString>

namespace aqt { namespace stylesheets { class StyleEngineSetup; } }

// Theme path tracker + CSS engine integration.
// Owns a StyleEngineSetup that loads CSS when themeUrl changes.
class ThemeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString themeUrl READ themeUrl NOTIFY themeChanged)
public:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    Q_INVOKABLE void setTheme(const QString &url);
    Q_INVOKABLE void reload();
    QString themeUrl() const { return currentUrl_; }

signals:
    void themeChanged();

private:
    QString currentUrl_;
    aqt::stylesheets::StyleEngineSetup *engine_ = nullptr;
};
