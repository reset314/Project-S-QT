#pragma once
#include <QObject>
#include <QString>
#include <QDateTime>
#include <optional>
#include <memory>
#include "../utils/ApiError.h"

class TokenManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ isLoggedIn NOTIFY loginStateChanged)

public:
    explicit TokenManager(QObject *parent = nullptr);

    bool isLoggedIn() const;

    Result<void> login(const QString &username, const QString &password);
    void logout();
    void restoreFromKeychain();

    QString accessToken() const;
    QString refreshToken() const;
    Result<QString> refreshAccessToken();

    void setServerUrl(const QString &url);
    QString serverUrl() const;

    /// Decode the "exp" claim from a JWT token and return it as QDateTime.
    /// Returns std::nullopt if the token is malformed or the claim is missing.
    static std::optional<QDateTime> decodeJwtExp(const QString &token);

signals:
    void loginStateChanged(bool loggedIn);

private:
    QString accessToken_;
    QString refreshToken_;
    QString serverUrl_;
    bool loggedIn_ = false;

    bool isAccessTokenExpired() const;
};
