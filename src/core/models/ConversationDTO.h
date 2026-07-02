#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <string>
#include <optional>
#include "../utils/JsonHelper.h"

struct ConversationDTO {
    std::string id;
    std::string userId;
    std::optional<std::string> aiUserId;
    std::string title;
    std::string createdAt;
    std::string updatedAt;
    std::optional<std::string> endedAt;
    // Client-side convenience fields (may be populated by server JOINs)
    std::optional<std::string> aiUserName;
    std::optional<std::string> aiUserAvatar;
    std::optional<std::string> lastMessagePreview;

    static ConversationDTO fromJson(const QJsonObject &obj) {
        ConversationDTO d;
        d.id = JsonHelper::getString(obj, "id");
        d.userId = JsonHelper::getString(obj, "user_id");
        auto au = obj.value("ai_user_id");
        if (au.isString()) d.aiUserId = au.toString().toStdString();
        d.title = JsonHelper::getString(obj, "title", "新对话");
        d.createdAt = JsonHelper::getString(obj, "created_at");
        d.updatedAt = JsonHelper::getString(obj, "updated_at");
        auto ea = obj.value("ended_at");
        if (ea.isString()) d.endedAt = ea.toString().toStdString();
        auto aun = obj.value("ai_user_name");
        if (aun.isString()) d.aiUserName = aun.toString().toStdString();
        auto aua = obj.value("ai_user_avatar");
        if (aua.isString()) d.aiUserAvatar = aua.toString().toStdString();
        auto lmp = obj.value("last_message_preview");
        if (lmp.isString()) d.lastMessagePreview = lmp.toString().toStdString();
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["user_id"] = QString::fromStdString(userId);
        if (aiUserId) o["ai_user_id"] = QString::fromStdString(*aiUserId);
        o["title"] = QString::fromStdString(title);
        o["created_at"] = QString::fromStdString(createdAt);
        o["updated_at"] = QString::fromStdString(updatedAt);
        if (endedAt) o["ended_at"] = QString::fromStdString(*endedAt);
        if (aiUserName) o["ai_user_name"] = QString::fromStdString(*aiUserName);
        if (aiUserAvatar) o["ai_user_avatar"] = QString::fromStdString(*aiUserAvatar);
        if (lastMessagePreview) o["last_message_preview"] = QString::fromStdString(*lastMessagePreview);
        return o;
    }
};
