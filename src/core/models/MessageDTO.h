#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <string>
#include <optional>
#include <vector>
#include "MediaDTO.h"
#include "../utils/JsonHelper.h"

struct MessageDTO {
    std::optional<int64_t>  serverId;
    std::string             clientUuid;
    std::string             aiUserId;
    std::string             conversationId;
    std::string             senderType;      // "user" | "ai" | "system"
    std::string             msgType;         // "text" | "image" | "video" | "file" | "audio" | "sticker"
    QJsonObject             content;         // {think, emotion, response, functions}
    std::vector<MediaDTO>   mediaList;
    bool                    isComplete = true;
    bool                    isRead = false;
    std::optional<QDateTime> revokedAt;
    std::optional<QDateTime> deletedAt;
    std::optional<std::string> sourceFunction;
    std::string             timestamp;       // ISO 8601

    static MessageDTO fromJson(const QJsonObject &obj) {
        MessageDTO m;
        m.serverId = obj.contains("id") && !obj.value("id").isNull()
            ? std::optional{JsonHelper::getInt(obj, "id")} : std::nullopt;
        m.clientUuid = JsonHelper::getString(obj, "client_uuid");
        m.aiUserId = JsonHelper::getString(obj, "ai_user_id");
        m.conversationId = JsonHelper::getString(obj, "conversation_id");
        m.senderType = JsonHelper::getString(obj, "sender_type");
        m.msgType = JsonHelper::getString(obj, "msg_type");
        auto c = obj.value("content");
        m.content = c.isObject() ? c.toObject() : QJsonObject{};
        auto ml = obj.value("media_list");
        if (ml.isArray()) {
            for (const auto &item : ml.toArray()) {
                if (item.isObject())
                    m.mediaList.push_back(MediaDTO::fromJson(item.toObject()));
            }
        }
        m.isComplete = JsonHelper::getBool(obj, "is_complete", true);
        m.isRead = JsonHelper::getBool(obj, "is_read", false);
        m.revokedAt = JsonHelper::getOptionalDateTime(obj, "revoked_at");
        m.deletedAt = JsonHelper::getOptionalDateTime(obj, "deleted_at");
        auto sf = obj.value("source_function");
        if (sf.isString()) m.sourceFunction = sf.toString().toStdString();
        m.timestamp = JsonHelper::getString(obj, "timestamp",
            QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString());
        return m;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        if (serverId) o["id"] = static_cast<qint64>(*serverId);
        o["client_uuid"] = QString::fromStdString(clientUuid);
        o["ai_user_id"] = QString::fromStdString(aiUserId);
        o["conversation_id"] = QString::fromStdString(conversationId);
        o["sender_type"] = QString::fromStdString(senderType);
        o["msg_type"] = QString::fromStdString(msgType);
        o["content"] = content;
        QJsonArray ml;
        for (const auto &d : mediaList) ml.append(d.toJson());
        o["media_list"] = ml;
        o["is_complete"] = isComplete;
        o["timestamp"] = QString::fromStdString(timestamp);
        return o;
    }
};
