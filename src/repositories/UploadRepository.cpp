#include "UploadRepository.h"
#include "../core/network/HttpClient.h"
#include "../core/utils/JsonHelper.h"
#include <QJsonObject>
#include <QMap>

// ---------------------------------------------------------------------------
// UploadResult
// ---------------------------------------------------------------------------

UploadResult UploadResult::fromJson(const QJsonObject &obj) {
    UploadResult r;
    r.mediaId = static_cast<int>(JsonHelper::getInt(obj, "media_id"));
    r.storedFilename = JsonHelper::getString(obj, "filename");
    r.originalFilename = JsonHelper::getString(obj, "original_filename");
    r.size = JsonHelper::getInt(obj, "size");
    r.mimeType = JsonHelper::getString(obj, "mime_type");
    return r;
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

UploadRepository::UploadRepository(HttpClient *http, QObject *parent)
    : QObject(parent)
    , http_(http)
{
}

// ---------------------------------------------------------------------------
// uploadFile  — POST /upload  (multipart form-data)
// ---------------------------------------------------------------------------

Result<UploadResult> UploadRepository::uploadFile(const QString &filePath)
{
    QMap<QString, QString> fields; // no extra fields needed

    auto result = http_->postFormData("/upload", fields, filePath);
    if (!result)
        return tl::make_unexpected(result.error());

    return UploadResult::fromJson(*result);
}
