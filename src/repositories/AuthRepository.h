#pragma once
#include <QObject>
#include "../core/utils/ApiError.h"
#include "../core/models/UserDTO.h"

class HttpClient;
class TokenManager;

/// Wraps authentication endpoints and delegates token persistence to TokenManager.
class AuthRepository : public QObject {
    Q_OBJECT
public:
    explicit AuthRepository(HttpClient *http, TokenManager *tm, QObject *parent = nullptr);

    /// POST /auth/login — returns the logged-in UserDTO on success.
    /// Tokens are persisted via TokenManager.
    Result<UserDTO> login(const QString &username, const QString &password);

    /// POST /auth/register
    Result<UserDTO> registerUser(const QString &username, const QString &email,
                                 const QString &password);

    /// POST /auth/logout — invalidates the refresh token server-side,
    /// then clears local state via TokenManager.
    Result<void> logout();

private:
    HttpClient *http_;
    TokenManager *tokenManager_;
};
