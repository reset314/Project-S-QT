#include "AIUserRepository.h"
#include "../core/network/HttpClient.h"
#include <QJsonObject>
#include <QJsonArray>

static QJsonValue unwrapData(const QJsonObject &resp) {
    if (resp.contains("success")) return resp.value("data");
    return QJsonValue(resp);
}

AIUserRepository::AIUserRepository(HttpClient *http, QObject *parent)
    : QObject(parent), http_(http) {}

Result<QVector<AIUserDTO>> AIUserRepository::listAIUsers() {
    auto result = http_->get("/aiusers/");
    if (!result) return tl::make_unexpected(result.error());
    QJsonObject data = *result;
    QJsonArray items = data.value("items").toArray();
    QVector<AIUserDTO> users;
    for (const auto &item : items)
        if (item.isObject()) users.append(AIUserDTO::fromJson(item.toObject()));
    return users;
}

Result<AIUserDTO> AIUserRepository::getAIUser(const QString &aiUserId) {
    auto result = http_->get(QString("/aiusers/%1").arg(aiUserId));
    if (!result) return tl::make_unexpected(result.error());
    return AIUserDTO::fromJson(*result);
}

Result<AIUserDTO> AIUserRepository::createAIUser(const QString &name,
    const QString &description, const QString &systemPrompt,
    const QString &llmProvider, const QString &llmModel,
    const QString &avatar, const QJsonObject &chatConfig) {
    QJsonObject body;
    body["name"] = name;
    if (!description.isEmpty()) body["description"] = description;
    if (!systemPrompt.isEmpty()) body["system_prompt"] = systemPrompt;
    if (!llmProvider.isEmpty()) body["llm_provider"] = llmProvider;
    if (!llmModel.isEmpty()) body["llm_model"] = llmModel;
    if (!avatar.isEmpty()) body["avatar"] = avatar;
    if (!chatConfig.isEmpty()) body["chat_config"] = chatConfig;
    auto result = http_->post("/aiusers/", body);
    if (!result) return tl::make_unexpected(result.error());
    return AIUserDTO::fromJson(*result);
}

Result<AIUserDTO> AIUserRepository::updateAIUser(const QString &aiUserId, const QJsonObject &fields) {
    auto result = http_->put(QString("/aiusers/%1").arg(aiUserId), fields);
    if (!result) return tl::make_unexpected(result.error());
    return AIUserDTO::fromJson(*result);
}

// ── QML-friendly wrappers ──────────────────────────────────────────
QJsonObject AIUserRepository::getAIUserJson(const QString &aiUserId) {
    auto r = getAIUser(aiUserId);
    if (r) return r->toJson();
    QJsonObject err; err["error"] = QString::fromStdString(r.error().message); return err;
}
QJsonObject AIUserRepository::updateAIUserJson(const QString &aiUserId, const QJsonObject &fields) {
    auto r = updateAIUser(aiUserId, fields);
    if (r) return r->toJson();
    QJsonObject err; err["error"] = QString::fromStdString(r.error().message); return err;
}
QJsonObject AIUserRepository::createAIUserJson(const QJsonObject &payload) {
    auto result = http_->post("/aiusers/", payload);
    if (!result) {
        QJsonObject err; err["error"] = QString::fromStdString(result.error().message); return err;
    }
    return AIUserDTO::fromJson(*result).toJson();
}
