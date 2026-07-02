#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <string>
#include <optional>
#include <vector>
#include "../utils/JsonHelper.h"

struct UserDTO {
    std::string id;
    std::string username;
    std::string email;
    bool isActive = true;
    bool emailVerified = false;
    std::string createdAt;
    std::optional<std::string> updatedAt;
    std::optional<std::string> avatar;
    std::optional<std::vector<std::string>> enabledExpansions;
    QJsonObject defaultChatConfig;
    QJsonObject defaultProviders;

    static UserDTO fromJson(const QJsonObject &obj) {
        UserDTO d;
        d.id = JsonHelper::getString(obj, "id");
        d.username = JsonHelper::getString(obj, "username");
        d.email = JsonHelper::getString(obj, "email");
        d.isActive = JsonHelper::getBool(obj, "is_active", true);
        d.emailVerified = JsonHelper::getBool(obj, "email_verified", false);
        d.createdAt = JsonHelper::getString(obj, "created_at");
        auto ua = obj.value("updated_at");
        if (ua.isString()) d.updatedAt = ua.toString().toStdString();
        auto av = obj.value("avatar");
        if (av.isString()) d.avatar = av.toString().toStdString();
        auto ee = obj.value("enabled_expansions");
        if (ee.isArray()) {
            std::vector<std::string> exps;
            for (const auto &item : ee.toArray()) {
                if (item.isString()) exps.push_back(item.toString().toStdString());
            }
            d.enabledExpansions = exps;
        }
        auto dcc = obj.value("default_chat_config");
        d.defaultChatConfig = dcc.isObject() ? dcc.toObject() : QJsonObject{};
        auto dp = obj.value("default_providers");
        d.defaultProviders = dp.isObject() ? dp.toObject() : QJsonObject{};
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["username"] = QString::fromStdString(username);
        o["email"] = QString::fromStdString(email);
        o["is_active"] = isActive;
        o["email_verified"] = emailVerified;
        o["created_at"] = QString::fromStdString(createdAt);
        if (updatedAt) o["updated_at"] = QString::fromStdString(*updatedAt);
        if (avatar) o["avatar"] = QString::fromStdString(*avatar);
        if (enabledExpansions) {
            QJsonArray arr;
            for (const auto &e : *enabledExpansions) arr.append(QString::fromStdString(e));
            o["enabled_expansions"] = arr;
        }
        if (!defaultChatConfig.isEmpty()) o["default_chat_config"] = defaultChatConfig;
        if (!defaultProviders.isEmpty()) o["default_providers"] = defaultProviders;
        return o;
    }
};
