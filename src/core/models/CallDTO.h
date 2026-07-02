#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <string>
#include <optional>
#include "../utils/JsonHelper.h"

/// Represents a WebSocket call signaling message (WebRTC relay).
/// Types: "offer" | "answer" | "ice_candidate" | "hangup" | "ping" | "pong" | "relay" | "error"
struct CallSignalDTO {
    std::string type;
    QJsonObject payload;

    static CallSignalDTO fromJson(const QJsonObject &obj) {
        CallSignalDTO d;
        d.type = JsonHelper::getString(obj, "type");
        auto p = obj.value("payload");
        d.payload = p.isObject() ? p.toObject() : QJsonObject{};
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["type"] = QString::fromStdString(type);
        if (!payload.isEmpty()) o["payload"] = payload;
        return o;
    }
};

/// Represents a call session summary (e.g. from call history).
/// No persistent Call table on the server; this is a client-side convenience model.
struct CallDTO {
    std::string conversationId;
    std::string callType;       // "voice" | "video"
    std::string status;         // "missed" | "rejected" | "accepted" | "ended"
    std::optional<std::string> callerId;
    std::optional<std::string> startedAt;
    std::optional<std::string> endedAt;
    std::optional<std::string> createdAt;

    static CallDTO fromJson(const QJsonObject &obj) {
        CallDTO d;
        d.conversationId = JsonHelper::getString(obj, "conversation_id");
        d.callType = JsonHelper::getString(obj, "call_type", "voice");
        d.status = JsonHelper::getString(obj, "status", "ended");
        auto ci = obj.value("caller_id");
        if (ci.isString()) d.callerId = ci.toString().toStdString();
        auto sa = obj.value("started_at");
        if (sa.isString()) d.startedAt = sa.toString().toStdString();
        auto ea = obj.value("ended_at");
        if (ea.isString()) d.endedAt = ea.toString().toStdString();
        auto ca = obj.value("created_at");
        if (ca.isString()) d.createdAt = ca.toString().toStdString();
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["conversation_id"] = QString::fromStdString(conversationId);
        o["call_type"] = QString::fromStdString(callType);
        o["status"] = QString::fromStdString(status);
        if (callerId) o["caller_id"] = QString::fromStdString(*callerId);
        if (startedAt) o["started_at"] = QString::fromStdString(*startedAt);
        if (endedAt) o["ended_at"] = QString::fromStdString(*endedAt);
        if (createdAt) o["created_at"] = QString::fromStdString(*createdAt);
        return o;
    }
};
