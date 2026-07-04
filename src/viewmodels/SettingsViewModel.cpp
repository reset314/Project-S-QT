#include "SettingsViewModel.h"

static const QString kThemeKey     = QStringLiteral("settings/theme");
static const QString kServerUrlKey = QStringLiteral("settings/serverUrl");
static const QString kTtsVoiceKey  = QStringLiteral("settings/ttsVoice");
static const QString kTtsSpeedKey  = QStringLiteral("settings/ttsSpeed");

static const QString kDefaultTheme     = QStringLiteral("system");
static const QString kDefaultServerUrl = QStringLiteral("https://www.p314.top");
static const QString kDefaultTtsVoice  = QStringLiteral("default");
static const double  kDefaultTtsSpeed  = 1.0;

SettingsViewModel::SettingsViewModel(QObject *parent)
    : QObject(parent)
    , settings_(QStringLiteral("AIChat"), QStringLiteral("AIChatDesktop"))
{
}

QString SettingsViewModel::theme() const
{
    return settings_.value(kThemeKey, kDefaultTheme).toString();
}

void SettingsViewModel::setTheme(const QString &value)
{
    if (theme() != value) {
        settings_.setValue(kThemeKey, value);
        emit themeChanged();
    }
}

QString SettingsViewModel::serverUrl() const
{
    return settings_.value(kServerUrlKey, kDefaultServerUrl).toString();
}

void SettingsViewModel::setServerUrl(const QString &value)
{
    if (serverUrl() != value) {
        settings_.setValue(kServerUrlKey, value);
        emit serverUrlChanged();
    }
}

QString SettingsViewModel::ttsVoice() const
{
    return settings_.value(kTtsVoiceKey, kDefaultTtsVoice).toString();
}

void SettingsViewModel::setTtsVoice(const QString &value)
{
    if (ttsVoice() != value) {
        settings_.setValue(kTtsVoiceKey, value);
        emit ttsVoiceChanged();
    }
}

double SettingsViewModel::ttsSpeed() const
{
    return settings_.value(kTtsSpeedKey, kDefaultTtsSpeed).toDouble();
}

void SettingsViewModel::setTtsSpeed(double value)
{
    if (!qFuzzyCompare(ttsSpeed(), value)) {
        settings_.setValue(kTtsSpeedKey, value);
        emit ttsSpeedChanged();
    }
}
