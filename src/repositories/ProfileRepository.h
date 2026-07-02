#pragma once
#include <QObject>
#include <QVector>
#include <QJsonObject>
#include "../core/utils/ApiError.h"
#include "../core/models/ProfileDTO.h"

class HttpClient;

class ProfileRepository : public QObject {
    Q_OBJECT
public:
    explicit ProfileRepository(HttpClient *http, QObject *parent = nullptr);

    /// GET /profiles
    Result<QVector<UserProfileDTO>> getProfiles();

    /// POST /profiles
    Result<UserProfileDTO> createProfile(const QString &key,
                                          const QString &content,
                                          double confidence = 1.0);

    /// PUT /profiles/{key}
    Result<UserProfileDTO> updateProfile(const QString &key,
                                          const QString &content,
                                          double confidence = 1.0);

private:
    HttpClient *http_;
};
