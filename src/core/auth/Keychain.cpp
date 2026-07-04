#include "Keychain.h"
#include <QSettings>
#include <QCryptographicHash>

// Cross-platform keychain via QSettings with base64 obfuscation.
// For production, integrate platform APIs (Windows Credential Store,
// macOS Keychain via Security framework, Linux libsecret).

static QByteArray obfuscate(const QByteArray &data) {
    // Simple XOR with a fixed key — not cryptographic, just deters
    // plain-text reading of QSettings files.  Replace with platform
    // keychain APIs for production.
    const QByteArray key = "AiChatDesktop2026";
    QByteArray result = data;
    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ key[i % key.size()];
    return result.toBase64();
}

static QByteArray deobfuscate(const QByteArray &data) {
    auto decoded = QByteArray::fromBase64(data);
    const QByteArray key = "AiChatDesktop2026";
    QByteArray result = decoded;
    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ key[i % key.size()];
    return result;
}

Result<void> Keychain::write(const QString &key, const QString &value) {
    QSettings settings("AIChat", "AIChatDesktop");
    settings.setValue("keychain/" + key, QString::fromUtf8(obfuscate(value.toUtf8())));
    settings.sync();
    if (settings.status() != QSettings::NoError)
        return tl::make_unexpected(ApiError::networkError("Failed to write to keychain"));
    return {};
}

Result<QString> Keychain::read(const QString &key) {
    QSettings settings("AIChat", "AIChatDesktop");
    auto stored = settings.value("keychain/" + key).toString();
    if (stored.isEmpty())
        return tl::make_unexpected(ApiError{.code="NOT_FOUND", .message="Key not found"});
    return QString::fromUtf8(deobfuscate(stored.toUtf8()));
}

Result<void> Keychain::remove(const QString &key) {
    QSettings settings("AIChat", "AIChatDesktop");
    settings.remove("keychain/" + key);
    settings.sync();
    return {};
}
