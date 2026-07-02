#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <string>
#include <optional>
#include <vector>
#include "../utils/JsonHelper.h"

struct MediaDTO {
    std::string id;
    std::string mediaType;
    std::string source;
    std::string originalFilename;
    std::string storedFilename;
    std::optional<std::string> filePath;
    std::optional<std::string> thumbnailStoredFilename;
    std::optional<std::string> url;
    std::optional<int64_t> fileSize;
    std::optional<std::string> mimeType;
    std::optional<std::string> uploadedBy;
    std::optional<double> duration;
    std::optional<std::string> description;

    static MediaDTO fromJson(const QJsonObject &obj) {
        MediaDTO d;
        d.id = JsonHelper::getString(obj, "id");
        d.mediaType = JsonHelper::getString(obj, "media_type");
        d.source = JsonHelper::getString(obj, "source");
        d.originalFilename = JsonHelper::getString(obj, "original_filename");
        d.storedFilename = JsonHelper::getString(obj, "stored_filename");
        auto fp = obj.value("file_path");
        if (fp.isString()) d.filePath = fp.toString().toStdString();
        auto ts = obj.value("thumbnail_stored_filename");
        if (ts.isString()) d.thumbnailStoredFilename = ts.toString().toStdString();
        auto u = obj.value("url");
        if (u.isString()) d.url = u.toString().toStdString();
        d.fileSize = obj.contains("file_size") && !obj.value("file_size").isNull()
            ? std::optional(JsonHelper::getInt(obj, "file_size")) : std::nullopt;
        auto mt = obj.value("mime_type");
        if (mt.isString()) d.mimeType = mt.toString().toStdString();
        auto ub = obj.value("uploaded_by");
        if (ub.isString()) d.uploadedBy = ub.toString().toStdString();
        d.duration = obj.contains("duration") && !obj.value("duration").isNull()
            ? std::optional(obj.value("duration").toDouble()) : std::nullopt;
        auto desc = obj.value("description");
        if (desc.isString()) d.description = desc.toString().toStdString();
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["media_type"] = QString::fromStdString(mediaType);
        o["source"] = QString::fromStdString(source);
        o["original_filename"] = QString::fromStdString(originalFilename);
        o["stored_filename"] = QString::fromStdString(storedFilename);
        if (filePath) o["file_path"] = QString::fromStdString(*filePath);
        if (url) o["url"] = QString::fromStdString(*url);
        if (fileSize) o["file_size"] = static_cast<qint64>(*fileSize);
        return o;
    }
};
