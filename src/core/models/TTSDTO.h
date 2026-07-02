#pragma once
#include <QJsonObject>
#include <string>
#include <optional>
#include "../utils/JsonHelper.h"

struct TTSDTO {
    std::string id;
    std::string name;
    std::string provider;
    std::optional<std::string> voiceId;
    std::optional<std::string> audioPath;
    std::optional<std::string> apiModel;
    std::optional<std::string> description;
    bool isPreset = false;
    bool isDefault = false;
    std::optional<std::string> createdBy;
    std::string createdAt;

    static TTSDTO fromJson(const QJsonObject &obj) {
        TTSDTO d;
        d.id = JsonHelper::getString(obj, "id");
        d.name = JsonHelper::getString(obj, "name");
        d.provider = JsonHelper::getString(obj, "provider");
        auto vi = obj.value("voice_id");
        if (vi.isString()) d.voiceId = vi.toString().toStdString();
        auto ap = obj.value("audio_path");
        if (ap.isString()) d.audioPath = ap.toString().toStdString();
        auto am = obj.value("api_model");
        if (am.isString()) d.apiModel = am.toString().toStdString();
        auto desc = obj.value("description");
        if (desc.isString()) d.description = desc.toString().toStdString();
        d.isPreset = JsonHelper::getBool(obj, "is_preset", false);
        d.isDefault = JsonHelper::getBool(obj, "is_default", false);
        auto cb = obj.value("created_by");
        if (cb.isString()) d.createdBy = cb.toString().toStdString();
        d.createdAt = JsonHelper::getString(obj, "created_at");
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["name"] = QString::fromStdString(name);
        o["provider"] = QString::fromStdString(provider);
        if (voiceId) o["voice_id"] = QString::fromStdString(*voiceId);
        if (audioPath) o["audio_path"] = QString::fromStdString(*audioPath);
        if (apiModel) o["api_model"] = QString::fromStdString(*apiModel);
        if (description) o["description"] = QString::fromStdString(*description);
        o["is_preset"] = isPreset;
        o["is_default"] = isDefault;
        if (createdBy) o["created_by"] = QString::fromStdString(*createdBy);
        o["created_at"] = QString::fromStdString(createdAt);
        return o;
    }
};
