#pragma once
#include <QJsonObject>
#include <string>
#include <optional>
#include "../utils/JsonHelper.h"

struct ProfileVersion {
    std::string id;
    std::string profileId;
    std::string content;
    double confidence = 1.0;
    std::optional<std::string> changedReason;
    std::string createdAt;

    static ProfileVersion fromJson(const QJsonObject &obj) {
        ProfileVersion d;
        d.id = JsonHelper::getString(obj, "id");
        d.profileId = JsonHelper::getString(obj, "profile_id");
        d.content = JsonHelper::getString(obj, "content");
        auto conf = obj.value("confidence");
        if (conf.isDouble()) d.confidence = conf.toDouble();
        auto cr = obj.value("changed_reason");
        if (cr.isString()) d.changedReason = cr.toString().toStdString();
        d.createdAt = JsonHelper::getString(obj, "created_at");
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["profile_id"] = QString::fromStdString(profileId);
        o["content"] = QString::fromStdString(content);
        o["confidence"] = confidence;
        if (changedReason) o["changed_reason"] = QString::fromStdString(*changedReason);
        o["created_at"] = QString::fromStdString(createdAt);
        return o;
    }
};

struct UserProfileDTO {
    std::string id;
    std::string userId;
    std::string attributeKey;
    std::string content;
    double confidence = 1.0;
    std::optional<std::string> source;
    std::optional<std::string> aiUserId;
    std::optional<std::string> deletedAt;
    std::string createdAt;
    std::string updatedAt;

    static UserProfileDTO fromJson(const QJsonObject &obj) {
        UserProfileDTO d;
        d.id = JsonHelper::getString(obj, "id");
        d.userId = JsonHelper::getString(obj, "user_id");
        d.attributeKey = JsonHelper::getString(obj, "attribute_key");
        d.content = JsonHelper::getString(obj, "content");
        auto conf = obj.value("confidence");
        if (conf.isDouble()) d.confidence = conf.toDouble();
        auto src = obj.value("source");
        if (src.isString()) d.source = src.toString().toStdString();
        auto aui = obj.value("ai_user_id");
        if (aui.isString()) d.aiUserId = aui.toString().toStdString();
        auto da = obj.value("deleted_at");
        if (da.isString()) d.deletedAt = da.toString().toStdString();
        d.createdAt = JsonHelper::getString(obj, "created_at");
        d.updatedAt = JsonHelper::getString(obj, "updated_at");
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["user_id"] = QString::fromStdString(userId);
        o["attribute_key"] = QString::fromStdString(attributeKey);
        o["content"] = QString::fromStdString(content);
        o["confidence"] = confidence;
        if (source) o["source"] = QString::fromStdString(*source);
        if (aiUserId) o["ai_user_id"] = QString::fromStdString(*aiUserId);
        if (deletedAt) o["deleted_at"] = QString::fromStdString(*deletedAt);
        o["created_at"] = QString::fromStdString(createdAt);
        o["updated_at"] = QString::fromStdString(updatedAt);
        return o;
    }
};
