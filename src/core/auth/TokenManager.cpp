#include "TokenManager.h"
#include "Keychain.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>

static const char *KEY_ACCESS_TOKEN = "access_token";
static const char *KEY_REFRESH_TOKEN = "refresh_token";

// --- JWT decoding -------------------------------------------------------

static QByteArray base64UrlDecode(const QString &input) {
    // Base64url -> Base64 (replace -_ with +/, pad to multiple of 4)
    QByteArray b64 = input.toUtf8();
    for (auto &c : b64) {
        if (c == '-') c = '+';
        if (c == '_') c = '/';
    }
    while (b64.size() % 4 != 0)
        b64.append('=');
    return QByteArray::fromBase64(b64);
}

std::optional<QDateTime> TokenManager::decodeJwtExp(const QString &token) {
    auto parts = token.split('.');
    if (parts.size() < 3)
        return std::nullopt;

    auto payloadJson = QJsonDocument::fromJson(base64UrlDecode(parts[1]));
    if (!payloadJson.isObject())
        return std::nullopt;

    auto expVal = payloadJson.object().value("exp");
    if (expVal.isUndefined())
        return std::nullopt;

    // exp is seconds since epoch (int or double)
    qint64 secs;
    if (expVal.isDouble())
        secs = static_cast<qint64>(expVal.toDouble());
    else
        return std::nullopt;

    return QDateTime::fromSecsSinceEpoch(secs, Qt::UTC);
}

bool TokenManager::isAccessTokenExpired() const {
    if (accessToken_.isEmpty())
        return true;
    auto exp = decodeJwtExp(accessToken_);
    if (!exp.has_value())
        return true;
    // Treat as expired if less than 60 seconds remain
    return exp->secsTo(QDateTime::currentDateTimeUtc()) >= -60;
}

// --- Constructor ---------------------------------------------------------

TokenManager::TokenManager(QObject *parent)
    : QObject(parent)
{
}

// --- Server URL ----------------------------------------------------------

void TokenManager::setServerUrl(const QString &url) {
    serverUrl_ = url;
}

QString TokenManager::serverUrl() const {
    return serverUrl_;
}

// --- State ---------------------------------------------------------------

bool TokenManager::isLoggedIn() const {
    return loggedIn_;
}

QString TokenManager::accessToken() const {
    return accessToken_;
}

QString TokenManager::refreshToken() const {
    return refreshToken_;
}

// --- Login ---------------------------------------------------------------

Result<void> TokenManager::login(const QString &username, const QString &password) {
    if (serverUrl_.isEmpty())
        return std::unexpected(ApiError{.code = "CONFIG_ERROR", .message = "Server URL not set"});

    QNetworkAccessManager mgr;
    QNetworkRequest request(QUrl(serverUrl_ + "/auth/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");

    QJsonObject body;
    body["username"] = username;
    body["password"] = password;

    QNetworkReply *reply = mgr.post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError && statusCode == 0) {
        return std::unexpected(ApiError::networkError(
            reply->errorString().toStdString()));
    }

    auto doc = QJsonDocument::fromJson(responseData);
    if (!doc.isObject())
        return std::unexpected(ApiError{.code = "PARSE_ERROR", .message = "Invalid JSON response"});

    auto root = doc.object();
    bool success = root.value("success").toBool(false);

    if (!success || statusCode >= 400) {
        auto dataObj = root.value("data").toObject();
        return std::unexpected(ApiError{
            .code = dataObj.value("code").toString("LOGIN_FAILED").toStdString(),
            .message = dataObj.value("message").toString("Login failed").toStdString(),
        });
    }

    auto data = root.value("data").toObject();
    QString access = data.value("access_token").toString();
    QString refresh = data.value("refresh_token").toString();

    if (access.isEmpty() || refresh.isEmpty())
        return std::unexpected(ApiError{.code = "PARSE_ERROR", .message = "Missing tokens in response"});

    accessToken_ = access;
    refreshToken_ = refresh;

    // Persist tokens
    auto w1 = Keychain::write(KEY_ACCESS_TOKEN, access);
    auto w2 = Keychain::write(KEY_REFRESH_TOKEN, refresh);
    if (!w1.has_value())
        return std::unexpected(w1.error());
    if (!w2.has_value())
        return std::unexpected(w2.error());

    if (!loggedIn_) {
        loggedIn_ = true;
        emit loginStateChanged(true);
    }
    return {};
}

// --- Refresh -------------------------------------------------------------

Result<QString> TokenManager::refreshAccessToken() {
    if (serverUrl_.isEmpty())
        return std::unexpected(ApiError{.code = "CONFIG_ERROR", .message = "Server URL not set"});

    if (refreshToken_.isEmpty())
        return std::unexpected(ApiError{.code = "NO_REFRESH_TOKEN", .message = "No refresh token available"});

    QNetworkAccessManager mgr;
    QNetworkRequest request(QUrl(serverUrl_ + "/auth/refresh"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");

    QJsonObject body;
    body["refresh_token"] = refreshToken_;

    QNetworkReply *reply = mgr.post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError && statusCode == 0) {
        return std::unexpected(ApiError::networkError(
            reply->errorString().toStdString()));
    }

    auto doc = QJsonDocument::fromJson(responseData);
    if (!doc.isObject())
        return std::unexpected(ApiError{.code = "PARSE_ERROR", .message = "Invalid JSON response"});

    auto root = doc.object();
    bool success = root.value("success").toBool(false);

    if (!success || statusCode >= 400) {
        auto dataObj = root.value("data").toObject();
        // Refresh failed — clear auth state
        logout();
        return std::unexpected(ApiError{
            .code = dataObj.value("code").toString("REFRESH_FAILED").toStdString(),
            .message = dataObj.value("message").toString("Token refresh failed").toStdString(),
        });
    }

    auto data = root.value("data").toObject();
    QString access = data.value("access_token").toString();

    if (access.isEmpty())
        return std::unexpected(ApiError{.code = "PARSE_ERROR", .message = "Missing access_token in response"});

    accessToken_ = access;
    auto w = Keychain::write(KEY_ACCESS_TOKEN, access);
    if (!w.has_value())
        return std::unexpected(w.error());

    return access;
}

// --- Restore from keychain -----------------------------------------------

void TokenManager::restoreFromKeychain() {
    auto accessResult = Keychain::read(KEY_ACCESS_TOKEN);
    auto refreshResult = Keychain::read(KEY_REFRESH_TOKEN);

    if (!accessResult.has_value() || !refreshResult.has_value()) {
        // No stored credentials — remain logged out
        return;
    }

    accessToken_ = accessResult.value();
    refreshToken_ = refreshResult.value();

    // Check if the access token is still valid
    if (isAccessTokenExpired()) {
        // Token is expired — try to refresh
        auto result = refreshAccessToken();
        if (!result.has_value()) {
            // Refresh failed — clear everything
            accessToken_.clear();
            refreshToken_.clear();
            Keychain::remove(KEY_ACCESS_TOKEN);
            Keychain::remove(KEY_REFRESH_TOKEN);
            return;
        }
    }

    if (!loggedIn_) {
        loggedIn_ = true;
        emit loginStateChanged(true);
    }
}

// --- Logout --------------------------------------------------------------

void TokenManager::logout() {
    accessToken_.clear();
    refreshToken_.clear();
    Keychain::remove(KEY_ACCESS_TOKEN);
    Keychain::remove(KEY_REFRESH_TOKEN);

    if (loggedIn_) {
        loggedIn_ = false;
        emit loginStateChanged(false);
    }
}
