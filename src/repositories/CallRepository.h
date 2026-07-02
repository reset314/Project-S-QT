#pragma once
#include <QObject>
#include <QString>
#include "../core/utils/ApiError.h"
#include "../core/models/CallDTO.h"

class HttpClient;

struct CallStartResponse {
    std::string conversationId;
    std::string callType;
    std::string status;

    static CallStartResponse fromJson(const QJsonObject &obj);
};

class CallRepository : public QObject {
    Q_OBJECT
public:
    explicit CallRepository(HttpClient *http, QObject *parent = nullptr);

    /// POST /call/start
    Result<CallStartResponse> startCall(const QString &conversationId,
                                         const QString &callType = "voice");

private:
    HttpClient *http_;
};
