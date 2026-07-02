#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QMap>
#include "../utils/ApiError.h"

class TokenManager;

class HttpClient : public QObject {
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = nullptr);

    void setTokenManager(TokenManager *tm);
    void setBaseUrl(const QString &url);

    Result<QJsonObject> get(const QString &path, const QUrlQuery &params = {});
    Result<QJsonObject> post(const QString &path, const QJsonObject &body = {});
    Result<QJsonObject> put(const QString &path, const QJsonObject &body = {});
    Result<QJsonObject> del(const QString &path, const QJsonObject &body = {});
    Result<QJsonObject> postFormData(const QString &path,
                                      const QMap<QString, QString> &fields,
                                      const QString &filePath = {});

private:
    Result<QJsonObject> doRequest(const QString &method, const QString &path,
                                   const QJsonObject &body,
                                   QHttpMultiPart *multiPart = nullptr,
                                   const QUrlQuery &params = {});
    Result<QJsonObject> sendWithRetry(const QString &method, const QString &path,
                                       const QJsonObject &body = {},
                                       QHttpMultiPart *mp = nullptr,
                                       const QUrlQuery &params = {});

    QUrl buildUrl(const QString &path, const QUrlQuery &params = {}) const;

    QNetworkAccessManager *nam_;
    TokenManager *tokenManager_ = nullptr;
    QString baseUrl_;
};
