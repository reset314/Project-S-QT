#include "AuthRepository.h"
#include "../core/network/HttpClient.h"
#include "../core/auth/TokenManager.h"
#include <QJsonObject>

AuthRepository::AuthRepository(HttpClient *http, TokenManager *tm, QObject *parent)
    : QObject(parent)
    , http_(http)
    , tokenManager_(tm)
{
}

Result<UserDTO> AuthRepository::login(const QString &username, const QString &password)
{
    // Delegate the full login flow to TokenManager, which handles the HTTP call,
    // JWT parsing, token persistence to keychain, and state management.
    auto tmResult = tokenManager_->login(username, password);
    if (!tmResult)
        return tl::make_unexpected(tmResult.error());

    // Fetch the authenticated user profile via HttpClient (which uses the now-
    // stored access token in its Authorization header).
    auto userResult = http_->get("/users/me");
    if (!userResult)
        return tl::make_unexpected(userResult.error());

    return UserDTO::fromJson(*userResult);
}

Result<UserDTO> AuthRepository::registerUser(const QString &username,
                                              const QString &email,
                                              const QString &password)
{
    QJsonObject body;
    body["username"] = username;
    body["email"] = email;
    body["password"] = password;

    auto result = http_->post("/auth/register", body);
    if (!result)
        return tl::make_unexpected(result.error());

    // HttpClient already unwrapped the {success, data} envelope — we get the
    // data object directly.
    return UserDTO::fromJson(*result);
}

Result<void> AuthRepository::logout()
{
    // Notify the server to invalidate the refresh token (best-effort).
    if (tokenManager_->isLoggedIn()) {
        QString refreshToken = tokenManager_->refreshToken();
        if (!refreshToken.isEmpty()) {
            QJsonObject body;
            body["refresh_token"] = refreshToken;
            http_->post("/auth/logout", body); // ignore errors
        }
    }

    tokenManager_->logout();
    return {};
}
