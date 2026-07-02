#pragma once
#include <QJsonObject>
#include <string>
#include <optional>
#include "../utils/JsonHelper.h"

struct MemoryDTO {
    std::string id;
    std::string aiUserId;
    std::string content;
    std::optional<std::string> summary;
    std::string memoryType = "STM";
    std::string category = "general";
    int importance = 1;
    double confidence = 1.0;
    bool isArchived = false;
    std::optional<std::string> sourceMemoryId;
    std::string createdAt;
    std::string updatedAt;

    static MemoryDTO fromJson(const QJsonObject &obj) {
        MemoryDTO d;
        d.id = JsonHelper::getString(obj, "id");
        d.aiUserId = JsonHelper::getString(obj, "ai_user_id");
        d.content = JsonHelper::getString(obj, "content");
        auto s = obj.value("summary");
        if (s.isString()) d.summary = s.toString().toStdString();
        d.memoryType = JsonHelper::getString(obj, "memory_type", "STM");
        d.category = JsonHelper::getString(obj, "category", "general");
        auto imp = obj.value("importance");
        if (imp.isDouble()) d.importance = imp.toInt();
        auto conf = obj.value("confidence");
        if (conf.isDouble()) d.confidence = conf.toDouble();
        d.isArchived = JsonHelper::getBool(obj, "is_archived", false);
        auto smi = obj.value("source_memory_id");
        if (smi.isString()) d.sourceMemoryId = smi.toString().toStdString();
        d.createdAt = JsonHelper::getString(obj, "created_at");
        d.updatedAt = JsonHelper::getString(obj, "updated_at", d.createdAt);
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["ai_user_id"] = QString::fromStdString(aiUserId);
        o["content"] = QString::fromStdString(content);
        if (summary) o["summary"] = QString::fromStdString(*summary);
        o["memory_type"] = QString::fromStdString(memoryType);
        o["category"] = QString::fromStdString(category);
        o["importance"] = importance;
        o["confidence"] = confidence;
        o["is_archived"] = isArchived;
        if (sourceMemoryId) o["source_memory_id"] = QString::fromStdString(*sourceMemoryId);
        o["created_at"] = QString::fromStdString(createdAt);
        o["updated_at"] = QString::fromStdString(updatedAt);
        return o;
    }
};
