#pragma once
#include <QObject>
#include <QVector>
#include <QJsonObject>
#include "../core/utils/ApiError.h"
#include "../core/models/UserDTO.h"

class HttpClient;

class UserRepository : public QObject {
    Q_OBJECT
public:
    explicit UserRepository(HttpClient *http, QObject *parent = nullptr);

    /// GET /users/me
    Result<UserDTO> getCurrentUser();

    /// GET /users  (admin only)
    Result<QVector<UserDTO>> listUsers();

    /// PUT /users/{id}
    Result<UserDTO> updateUser(const QString &userId, const QJsonObject &fields);

private:
    HttpClient *http_;
};
