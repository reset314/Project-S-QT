#pragma once
#include <QObject>
#include <QString>
#include <QSettings>

class SettingsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(QString ttsVoice READ ttsVoice WRITE setTtsVoice NOTIFY ttsVoiceChanged)
    Q_PROPERTY(double ttsSpeed READ ttsSpeed WRITE setTtsSpeed NOTIFY ttsSpeedChanged)

public:
    explicit SettingsViewModel(QObject *parent = nullptr);

    QString theme() const;
    void setTheme(const QString &value);

    QString serverUrl() const;
    void setServerUrl(const QString &value);

    QString ttsVoice() const;
    void setTtsVoice(const QString &value);

    double ttsSpeed() const;
    void setTtsSpeed(double value);

signals:
    void themeChanged();
    void serverUrlChanged();
    void ttsVoiceChanged();
    void ttsSpeedChanged();

private:
    QSettings settings_;
};
