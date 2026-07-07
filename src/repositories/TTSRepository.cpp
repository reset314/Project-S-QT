#include "TTSRepository.h"
#include "../core/network/HttpClient.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

TTSRepository::TTSRepository(HttpClient *http, QObject *parent)
    : QObject(parent)
    , http_(http)
{
}

QJsonObject TTSRepository::getVoicesJson()
{
    auto result = http_->get("/tts/voices");
    if (!result) {
        QJsonObject e; e["error"] = QString::fromStdString(result.error().message); return e;
    }
    // response data is an array of voice objects
    QJsonValue dataVal = result->value("data");
    QJsonArray arr;
    if (dataVal.isArray()) arr = dataVal.toArray();
    else if (result->contains("success")) arr = result->value("data").toArray();

    QJsonObject o; o["voices"] = arr; return o;
}

Result<QByteArray> TTSRepository::synthesize(const QString &text,
                                               const QString &voice,
                                               double speed)
{
    QJsonObject body;
    body["text"] = text;
    if (!voice.isEmpty())
        body["voice_id"] = voice;
    body["speed"] = speed;

    // Use HttpClient::post() to get the JSON envelope.
    // The backend returns {success: true, data: {audio: "<base64>", format: "wav"}}.
    auto result = http_->post("/tts/synthesize", body);
    if (!result)
        return tl::make_unexpected(result.error());

    QJsonObject data = *result;

    // Extract base64-encoded audio
    QString audioB64 = data.value("audio").toString();
    if (audioB64.isEmpty())
        return tl::make_unexpected(ApiError{
            .code = "PARSE_ERROR",
            .message = "No audio data in TTS response"
        });

    QByteArray audioBytes = QByteArray::fromBase64(audioB64.toUtf8());
    return audioBytes;
}
