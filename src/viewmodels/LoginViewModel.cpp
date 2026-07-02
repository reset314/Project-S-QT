#include "LoginViewModel.h"
#include "../repositories/AuthRepository.h"
#include "../core/auth/TokenManager.h"
#include "../core/utils/ApiError.h"

LoginViewModel::LoginViewModel(AuthRepository *authRepo, TokenManager *tokenManager,
                               QObject *parent)
    : QObject(parent)
    , authRepo_(authRepo)
    , tokenManager_(tokenManager)
{
}

QString LoginViewModel::username() const
{
    return username_;
}

void LoginViewModel::setUsername(const QString &value)
{
    if (username_ != value) {
        username_ = value;
        emit usernameChanged();
    }
}

QString LoginViewModel::password() const
{
    return password_;
}

void LoginViewModel::setPassword(const QString &value)
{
    if (password_ != value) {
        password_ = value;
        emit passwordChanged();
    }
}

bool LoginViewModel::isLoading() const
{
    return loading_;
}

QString LoginViewModel::errorMessage() const
{
    return errorMessage_;
}

void LoginViewModel::login()
{
    if (loading_) return;
    if (username_.trimmed().isEmpty()) {
        errorMessage_ = QStringLiteral("请输入用户名");
        emit errorMessageChanged();
        return;
    }
    if (password_.isEmpty()) {
        errorMessage_ = QStringLiteral("请输入密码");
        emit errorMessageChanged();
        return;
    }

    loading_ = true;
    emit loadingChanged();

    // AuthRepository::login calls TokenManager::login internally,
    // then fetches /users/me and returns Result<UserDTO>
    auto result = authRepo_->login(username_.trimmed(), password_);

    if (result) {
        errorMessage_.clear();
        emit errorMessageChanged();
        emit loginSuccess();
    } else {
        errorMessage_ = QString::fromStdString(result.error().message);
        emit errorMessageChanged();
    }

    loading_ = false;
    emit loadingChanged();
}
