#include "ProfileRepository.h"
#include "../core/network/HttpClient.h"
#include <QJsonObject>
#include <QJsonArray>

ProfileRepository::ProfileRepository(HttpClient *http, QObject *parent)
    : QObject(parent)
    , http_(http)
{
}

Result<QVector<UserProfileDTO>> ProfileRepository::getProfiles()
{
    auto result = http_->get("/profiles");
    if (!result)
        return std::unexpected(result.error());

    // Response can be an array (from data) or the root envelope
    QJsonValue dataVal = result->value("data");
    QJsonArray arr;
    if (dataVal.isArray()) {
        arr = dataVal.toArray();
    } else if (result->contains("success")) {
        arr = result->value("data").toArray();
    }

    QVector<UserProfileDTO> profiles;
    for (const auto &item : arr) {
        if (item.isObject())
            profiles.append(UserProfileDTO::fromJson(item.toObject()));
    }
    return profiles;
}

Result<UserProfileDTO> ProfileRepository::createProfile(const QString &key,
                                                         const QString &content,
                                                         double confidence)
{
    QJsonObject body;
    body["attribute_key"] = key;
    body["content"] = content;
    body["confidence"] = confidence;

    auto result = http_->post("/profiles", body);
    if (!result)
        return std::unexpected(result.error());

    return UserProfileDTO::fromJson(*result);
}

Result<UserProfileDTO> ProfileRepository::updateProfile(const QString &key,
                                                         const QString &content,
                                                         double confidence)
{
    QJsonObject body;
    body["content"] = content;
    body["confidence"] = confidence;

    // PUT /profiles/{key}  —  HttpClient lacks put(), use post() as fallback.
    auto result = http_->post(QString("/profiles/%1").arg(key), body);
    if (!result)
        return std::unexpected(result.error());

    return UserProfileDTO::fromJson(*result);
}
