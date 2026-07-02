#include "HttpClient.h"
#include "../auth/TokenManager.h"

#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

HttpClient::HttpClient(QObject *parent)
    : QObject(parent)
    , nam_(new QNetworkAccessManager(this))
{
}

void HttpClient::setTokenManager(TokenManager *tm)
{
    tokenManager_ = tm;
}

void HttpClient::setBaseUrl(const QString &url)
{
    baseUrl_ = url;
    // Strip trailing slash for consistent composition
    while (baseUrl_.endsWith(QLatin1Char('/'))) {
        baseUrl_.chop(1);
    }
}

QUrl HttpClient::buildUrl(const QString &path, const QUrlQuery &params) const
{
    QString full = baseUrl_;
    if (!path.startsWith(QLatin1Char('/'))) {
        full += QLatin1Char('/');
    }
    full += path;

    QUrl url(full);
    if (!params.isEmpty()) {
        url.setQuery(params);
    }
    return url;
}

Result<QJsonObject> HttpClient::get(const QString &path, const QUrlQuery &params)
{
    return sendWithRetry("GET", path, {}, nullptr, params);
}

Result<QJsonObject> HttpClient::post(const QString &path, const QJsonObject &body)
{
    return sendWithRetry("POST", path, body);
}

Result<QJsonObject> HttpClient::put(const QString &path, const QJsonObject &body)
{
    return sendWithRetry("PUT", path, body);
}

Result<QJsonObject> HttpClient::del(const QString &path, const QJsonObject &body)
{
    return sendWithRetry("DELETE", path, body);
}

Result<QJsonObject> HttpClient::postFormData(const QString &path,
                                              const QMap<QString, QString> &fields,
                                              const QString &filePath)
{
    // Build multipart body
    auto buildMultipart = [&]() -> QHttpMultiPart * {
        auto *mp = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        for (auto it = fields.begin(); it != fields.end(); ++it) {
            QHttpPart part;
            part.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QString("form-data; name=\"%1\"").arg(it.key()));
            part.setBody(it.value().toUtf8());
            mp->append(part);
        }

        if (!filePath.isEmpty()) {
            QFile *file = new QFile(filePath);
            if (file->open(QIODevice::ReadOnly)) {
                QHttpPart filePart;
                filePart.setHeader(
                    QNetworkRequest::ContentDispositionHeader,
                    QString("form-data; name=\"file\"; filename=\"%1\"")
                        .arg(QFileInfo(filePath).fileName()));
                filePart.setBodyDevice(file);
                file->setParent(mp); // mp owns the file, closed when mp is destroyed
                mp->append(filePart);
            } else {
                delete file;
                delete mp;
                return nullptr;
            }
        }

        return mp;
    };

    // First attempt
    QHttpMultiPart *mp = buildMultipart();
    if (!mp) {
        return tl::make_unexpected(
            ApiError::networkError("Failed to open file: " + filePath.toStdString()));
    }

    Result<QJsonObject> result = doRequest("POST", path, {}, mp);

    // If 401, refresh token and retry once
    if (!result && result.error().code == "UNAUTHORIZED") {
        if (tokenManager_) {
            auto refreshResult = tokenManager_->refreshAccessToken();
            if (!refreshResult) {
                return tl::make_unexpected(
                    ApiError::networkError("Token refresh failed"));
            }
        }

        QHttpMultiPart *mp2 = buildMultipart();
        if (!mp2) {
            return tl::make_unexpected(
                ApiError::networkError("Failed to open file: " + filePath.toStdString()));
        }
        result = doRequest("POST", path, {}, mp2);
    }

    return result;
}

Result<QJsonObject> HttpClient::sendWithRetry(const QString &method,
                                               const QString &path,
                                               const QJsonObject &body,
                                               QHttpMultiPart *mp,
                                               const QUrlQuery &params)
{
    Result<QJsonObject> result = doRequest(method, path, body, mp, params);

    // On 401, refresh token and retry once
    if (!result && result.error().code == "UNAUTHORIZED") {
        if (tokenManager_) {
            auto refreshResult = tokenManager_->refreshAccessToken();
            if (!refreshResult) {
                return tl::make_unexpected(
                    ApiError::networkError("Token refresh failed"));
            }
        }
        // Retry (without multipart — caller handles multipart retry separately)
        result = doRequest(method, path, body, nullptr, params);
    }

    return result;
}

Result<QJsonObject> HttpClient::doRequest(const QString &method,
                                           const QString &path,
                                           const QJsonObject &body,
                                           QHttpMultiPart *multiPart,
                                           const QUrlQuery &params)
{
    QUrl url = buildUrl(path, params);

    QNetworkRequest request(url);

    // Inject Authorization header
    if (tokenManager_ && tokenManager_->isLoggedIn()) {
        QString token = tokenManager_->accessToken();
        if (!token.isEmpty()) {
            request.setRawHeader(
                "Authorization",
                QString("Bearer %1").arg(token).toUtf8());
        }
    }

    // Send request
    QNetworkReply *reply = nullptr;
    if (method == "GET") {
        reply = nam_->get(request);
    } else if (method == "POST") {
        if (multiPart) {
            reply = nam_->post(request, multiPart);
        } else {
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                              QStringLiteral("application/json"));
            QByteArray jsonBytes = QJsonDocument(body).toJson(QJsonDocument::Compact);
            reply = nam_->post(request, jsonBytes);
        }
    } else if (method == "PUT") {
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          QStringLiteral("application/json"));
        QByteArray jsonBytes = QJsonDocument(body).toJson(QJsonDocument::Compact);
        reply = nam_->put(request, jsonBytes);
    } else if (method == "DELETE") {
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          QStringLiteral("application/json"));
        QByteArray jsonBytes = QJsonDocument(body).toJson(QJsonDocument::Compact);
        reply = nam_->deleteResource(request);
    } else {
        return tl::make_unexpected(
            ApiError::networkError("Unsupported HTTP method: " + method.toStdString()));
    }

    // Block until reply finishes
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // Read response body
    QByteArray responseBytes = reply->readAll();
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QNetworkReply::NetworkError netError = reply->error();
    reply->deleteLater();

    // Handle network error
    if (netError != QNetworkReply::NoError) {
        if (httpStatus == 401) {
            return tl::make_unexpected(
                ApiError{.code = "UNAUTHORIZED", .message = "Access token expired"});
        }
        return tl::make_unexpected(
            ApiError::networkError("Network request failed: HTTP " +
                                   std::to_string(httpStatus)));
    }

    // Parse JSON response
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseBytes, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return tl::make_unexpected(
            ApiError::networkError("JSON parse error: " +
                                   parseError.errorString().toStdString()));
    }

    if (!doc.isObject()) {
        return tl::make_unexpected(
            ApiError::networkError("Response is not a JSON object"));
    }

    QJsonObject root = doc.object();

    // Check success field
    if (root.value("success").toBool(false)) {
        // Extract data field
        QJsonValue dataValue = root.value("data");
        if (dataValue.isObject()) {
            return dataValue.toObject();
        }
        // If data is not an object, return the whole root
        return root;
    }

    // Error response
    int statusCode = httpStatus;
    if (statusCode == 401) {
        return tl::make_unexpected(
            ApiError{.code = "UNAUTHORIZED", .message = "Access token expired"});
    }

    QJsonValue errorValue = root.value("error");
    if (errorValue.isObject()) {
        return tl::make_unexpected(ApiError::fromJson(errorValue.toObject()));
    }

    return tl::make_unexpected(
        ApiError::networkError("Unknown server error: HTTP " +
                               std::to_string(statusCode)));
}
