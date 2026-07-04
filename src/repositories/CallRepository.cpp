#include "CallRepository.h"
#include "../core/network/HttpClient.h"
#include "../core/utils/JsonHelper.h"
#include <QJsonObject>

// ---------------------------------------------------------------------------
// CallStartResponse
// ---------------------------------------------------------------------------

CallStartResponse CallStartResponse::fromJson(const QJsonObject &obj) {
    CallStartResponse r;
    r.conversationId = JsonHelper::getString(obj, "conversation_id");
    r.callType = JsonHelper::getString(obj, "call_type", "voice");
    r.status = JsonHelper::getString(obj, "status", "started");
    return r;
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

CallRepository::CallRepository(HttpClient *http, QObject *parent)
    : QObject(parent)
    , http_(http)
{
}

// ---------------------------------------------------------------------------
// startCall  — POST /call/start
// ---------------------------------------------------------------------------

Result<CallStartResponse> CallRepository::startCall(const QString &conversationId,
                                                      const QString &callType)
{
    QJsonObject body;
    body["conversation_id"] = conversationId;
    body["call_type"] = callType;

    auto result = http_->post("/call/start", body);
    if (!result)
        return tl::make_unexpected(result.error());

    return CallStartResponse::fromJson(*result);
}
