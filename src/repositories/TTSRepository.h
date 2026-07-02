#pragma once
#include <QObject>
#include <QByteArray>
#include <QString>
#include "../core/utils/ApiError.h"

class HttpClient;

class TTSRepository : public QObject {
    Q_OBJECT
public:
    explicit TTSRepository(HttpClient *http, QObject *parent = nullptr);

    /// POST /tts/synthesize — returns raw audio bytes on success.
    /// The response data.audio field contains base64-encoded audio.
    Result<QByteArray> synthesize(const QString &text,
                                   const QString &voice = {},
                                   double speed = 1.0);

private:
    HttpClient *http_;
};
