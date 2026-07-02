#include "UserRepository.h"
#include "../core/network/HttpClient.h"
#include <QJsonObject>
#include <QJsonArray>

UserRepository::UserRepository(HttpClient *http, QObject *parent)
    : QObject(parent)
    , http_(http)
{
}

Result<UserDTO> UserRepository::getCurrentUser()
{
    auto result = http_->get("/users/me");
    if (!result)
        return std::unexpected(result.error());

    return UserDTO::fromJson(*result);
}

Result<QVector<UserDTO>> UserRepository::listUsers()
{
    auto result = http_->get("/users");
    if (!result)
        return std::unexpected(result.error());

    // GET /users returns {success: true, data: [...]}
    // HttpClient passes through the root when data is an array.
    QJsonValue dataVal = result->value("data");
    QJsonArray arr;
    if (dataVal.isArray()) {
        arr = dataVal.toArray();
    } else if (result->contains("success")) {
        arr = result->value("data").toArray();
    }

    QVector<UserDTO> users;
    for (const auto &item : arr) {
        if (item.isObject())
            users.append(UserDTO::fromJson(item.toObject()));
    }
    return users;
}

Result<UserDTO> UserRepository::updateUser(const QString &userId,
                                            const QJsonObject &fields)
{
    auto result = http_->put(QString("/users/%1").arg(userId), fields);
    if (!result)
        return std::unexpected(result.error());

    return UserDTO::fromJson(*result);
}
