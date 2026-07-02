#pragma once
#include <QObject>
#include <QString>
#include "../core/utils/ApiError.h"

class HttpClient;

struct UploadResult {
    int mediaId = 0;
    std::string storedFilename;
    std::string originalFilename;
    int64_t size = 0;
    std::string mimeType;

    static UploadResult fromJson(const QJsonObject &obj);
};

class UploadRepository : public QObject {
    Q_OBJECT
public:
    explicit UploadRepository(HttpClient *http, QObject *parent = nullptr);

    /// POST /upload  (multipart form-data — single file)
    Result<UploadResult> uploadFile(const QString &filePath);

private:
    HttpClient *http_;
};
