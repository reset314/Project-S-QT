#pragma once
#include <QObject>
#include <QString>

class AuthRepository;
class TokenManager;

class LoginViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit LoginViewModel(AuthRepository *authRepo, TokenManager *tokenManager,
                            QObject *parent = nullptr);

    QString username() const;
    void setUsername(const QString &value);

    QString password() const;
    void setPassword(const QString &value);

    bool isLoading() const;

    QString errorMessage() const;

    Q_INVOKABLE void login();

signals:
    void usernameChanged();
    void passwordChanged();
    void loadingChanged();
    void errorMessageChanged();
    void loginSuccess();

private:
    AuthRepository *authRepo_;
    TokenManager *tokenManager_;
    QString username_;
    QString password_;
    bool loading_ = false;
    QString errorMessage_;
};
