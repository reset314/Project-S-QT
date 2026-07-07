#pragma once
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <string>
#include <optional>
#include <vector>
#include "MediaDTO.h"
#include "../utils/JsonHelper.h"

struct MessageDTO {
    std::string             serverId;        // Server uses UUID strings
    std::string             clientUuid;
    std::string             aiUserId;
    std::string             senderType;      // "user" | "ai" | "system"
    std::string             msgType;         // "text" | "image" | "video" | "file" | "audio" | "sticker"
    std::string             content;         // 纯文本或 JSON 字符串 {"think":"...","response":"..."}
    std::vector<MediaDTO>   mediaList;
    bool                    isComplete = true;
    bool                    isRead = false;
    std::optional<QDateTime> revokedAt;
    std::optional<QDateTime> deletedAt;
    std::optional<std::string> sourceFunction;
    std::string             timestamp;       // ISO 8601

    static MessageDTO fromJson(const QJsonObject &obj) {
        MessageDTO m;
        m.serverId = JsonHelper::getString(obj, "id");
        m.clientUuid = JsonHelper::getString(obj, "client_uuid");
        m.aiUserId = JsonHelper::getString(obj, "ai_user_id");
        m.conversationId = JsonHelper::getString(obj, "conversation_id");
        m.senderType = JsonHelper::getString(obj, "sender_type");
        m.msgType = JsonHelper::getString(obj, "msg_type");
        // Content is a raw string from the server. It may be plain text or
        // a JSON string like {"think":"...","emotion":"...","response":"..."}.
        // Callers parse the structure when reading; we store whatever the server sends.
        auto c = obj.value("content");
        if (c.isString()) {
            m.content = c.toString().toStdString();
        }
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

    MessageDTO() = default;
    MessageDTO(const MessageDTO &) = default;
    MessageDTO &operator=(const MessageDTO &) = default;

    QJsonObject toJson() const {
        QJsonObject o;
        if (!serverId.empty()) o["id"] = QString::fromStdString(serverId);
        o["client_uuid"] = QString::fromStdString(clientUuid);
        o["ai_user_id"] = QString::fromStdString(aiUserId);
        o["conversation_id"] = QString::fromStdString(conversationId);
        o["sender_type"] = QString::fromStdString(senderType);
        o["msg_type"] = QString::fromStdString(msgType);
        o["content"] = QString::fromStdString(content);
        QJsonArray ml;
        for (const auto &d : mediaList) ml.append(d.toJson());
        o["media_list"] = ml;
        o["is_complete"] = isComplete;
        o["timestamp"] = QString::fromStdString(timestamp);
        return o;
    }
};

Q_DECLARE_METATYPE(MessageDTO)
