#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <string>
#include <optional>
#include "../utils/JsonHelper.h"

struct AIUserDTO {
    std::string id;
    std::string name;
    std::optional<std::string> description;
    std::string createdBy;
    std::optional<std::string> llmProvider;
    std::optional<std::string> llmModel;
    std::optional<std::string> llmImageProvider;
    std::optional<std::string> llmImageModel;
    std::optional<std::string> llmMultimodalProvider;
    std::optional<std::string> llmMultimodalModel;
    std::optional<std::string> llmAudioTranscribeProvider;
    std::optional<std::string> llmAudioTranscribeModel;
    std::optional<std::string> llmAudioUnderstandingProvider;
    std::optional<std::string> llmAudioUnderstandingModel;
    std::optional<std::string> systemPrompt;
    std::optional<std::string> ttsId;
    std::optional<std::string> avatar;
    std::string createdAt;
    std::optional<std::string> updatedAt;
    QJsonObject chatConfig;

    static AIUserDTO fromJson(const QJsonObject &obj) {
        AIUserDTO d;
        d.id = JsonHelper::getString(obj, "id");
        d.name = JsonHelper::getString(obj, "name");
        auto desc = obj.value("description");
        if (desc.isString()) d.description = desc.toString().toStdString();
        d.createdBy = JsonHelper::getString(obj, "created_by");
        auto lp = obj.value("llm_provider");
        if (lp.isString()) d.llmProvider = lp.toString().toStdString();
        auto lm = obj.value("llm_model");
        if (lm.isString()) d.llmModel = lm.toString().toStdString();
        auto lip = obj.value("llm_image_provider");
        if (lip.isString()) d.llmImageProvider = lip.toString().toStdString();
        auto lim = obj.value("llm_image_model");
        if (lim.isString()) d.llmImageModel = lim.toString().toStdString();
        auto lmp = obj.value("llm_multimodal_provider");
        if (lmp.isString()) d.llmMultimodalProvider = lmp.toString().toStdString();
        auto lmm = obj.value("llm_multimodal_model");
        if (lmm.isString()) d.llmMultimodalModel = lmm.toString().toStdString();
        auto latp = obj.value("llm_audio_transcribe_provider");
        if (latp.isString()) d.llmAudioTranscribeProvider = latp.toString().toStdString();
        auto latm = obj.value("llm_audio_transcribe_model");
        if (latm.isString()) d.llmAudioTranscribeModel = latm.toString().toStdString();
        auto laup = obj.value("llm_audio_understanding_provider");
        if (laup.isString()) d.llmAudioUnderstandingProvider = laup.toString().toStdString();
        auto laum = obj.value("llm_audio_understanding_model");
        if (laum.isString()) d.llmAudioUnderstandingModel = laum.toString().toStdString();
        auto sp = obj.value("system_prompt");
        if (sp.isString()) d.systemPrompt = sp.toString().toStdString();
        auto ti = obj.value("tts_id");
        if (ti.isString()) d.ttsId = ti.toString().toStdString();
        auto av = obj.value("avatar");
        if (av.isString()) d.avatar = av.toString().toStdString();
        d.createdAt = JsonHelper::getString(obj, "created_at");
        auto ua = obj.value("updated_at");
        if (ua.isString()) d.updatedAt = ua.toString().toStdString();
        auto cc = obj.value("chat_config");
        d.chatConfig = cc.isObject() ? cc.toObject() : QJsonObject{};
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["name"] = QString::fromStdString(name);
        if (description) o["description"] = QString::fromStdString(*description);
        o["created_by"] = QString::fromStdString(createdBy);
        if (llmProvider) o["llm_provider"] = QString::fromStdString(*llmProvider);
        if (llmModel) o["llm_model"] = QString::fromStdString(*llmModel);
        if (llmImageProvider) o["llm_image_provider"] = QString::fromStdString(*llmImageProvider);
        if (llmImageModel) o["llm_image_model"] = QString::fromStdString(*llmImageModel);
        if (llmMultimodalProvider) o["llm_multimodal_provider"] = QString::fromStdString(*llmMultimodalProvider);
        if (llmMultimodalModel) o["llm_multimodal_model"] = QString::fromStdString(*llmMultimodalModel);
        if (llmAudioTranscribeProvider) o["llm_audio_transcribe_provider"] = QString::fromStdString(*llmAudioTranscribeProvider);
        if (llmAudioTranscribeModel) o["llm_audio_transcribe_model"] = QString::fromStdString(*llmAudioTranscribeModel);
        if (llmAudioUnderstandingProvider) o["llm_audio_understanding_provider"] = QString::fromStdString(*llmAudioUnderstandingProvider);
        if (llmAudioUnderstandingModel) o["llm_audio_understanding_model"] = QString::fromStdString(*llmAudioUnderstandingModel);
        if (systemPrompt) o["system_prompt"] = QString::fromStdString(*systemPrompt);
        if (ttsId) o["tts_id"] = QString::fromStdString(*ttsId);
        if (avatar) o["avatar"] = QString::fromStdString(*avatar);
        o["created_at"] = QString::fromStdString(createdAt);
        if (updatedAt) o["updated_at"] = QString::fromStdString(*updatedAt);
        if (!chatConfig.isEmpty()) o["chat_config"] = chatConfig;
        return o;
    }
};
