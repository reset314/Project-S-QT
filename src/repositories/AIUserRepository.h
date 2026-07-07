#pragma once
#include <QObject>
#include <QVector>
#include <QJsonObject>
#include "../core/utils/ApiError.h"
#include "../core/models/AIUserDTO.h"

class HttpClient;

class AIUserRepository : public QObject {
    Q_OBJECT
public:
    explicit AIUserRepository(HttpClient *http, QObject *parent = nullptr);

    /// GET /aiusers/
    Result<QVector<AIUserDTO>> listAIUsers();

    /// GET /aiusers/{id}
    Result<AIUserDTO> getAIUser(const QString &aiUserId);

    /// POST /aiusers/
    Result<AIUserDTO> createAIUser(const QString &name, const QString &description = {},
                                    const QString &systemPrompt = {}, const QString &llmProvider = {},
                                    const QString &llmModel = {}, const QString &avatar = {},
                                    const QJsonObject &chatConfig = {});

    /// PUT /aiusers/{id}
    Result<AIUserDTO> updateAIUser(const QString &aiUserId, const QJsonObject &fields);

    /// QML-friendly: GET /aiusers/{id} → QJsonObject
    Q_INVOKABLE QJsonObject getAIUserJson(const QString &aiUserId);

    /// QML-friendly: PUT /aiusers/{id} → QJsonObject
    Q_INVOKABLE QJsonObject updateAIUserJson(const QString &aiUserId, const QJsonObject &fields);

    /// QML-friendly: POST /aiusers/ → QJsonObject
    Q_INVOKABLE QJsonObject createAIUserJson(const QJsonObject &payload);

private:
    HttpClient *http_;
};
