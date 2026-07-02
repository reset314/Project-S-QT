#include "CallSignalingClient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

CallSignalingClient::CallSignalingClient(QObject *parent)
    : QObject(parent)
{
    QObject::connect(&ws_, &QWebSocket::connected,
                     this, &CallSignalingClient::onConnected);
    QObject::connect(&ws_, &QWebSocket::disconnected,
                     this, &CallSignalingClient::onDisconnected);
    QObject::connect(&ws_, &QWebSocket::textMessageReceived,
                     this, &CallSignalingClient::onTextMessageReceived);
}

CallSignalingClient::~CallSignalingClient()
{
    intentionalDisconnect_ = true;
    if (ws_.state() != QAbstractSocket::UnconnectedState) {
        ws_.close();
    }
}

// ---------------------------------------------------------------------------
// Connection
// ---------------------------------------------------------------------------

void CallSignalingClient::connectToServer(const QString &wsUrl, const QString &token)
{
    token_ = token;
    intentionalDisconnect_ = false;
    authenticated_ = false;

    qDebug() << "CallSignalingClient: connecting to" << wsUrl;
    ws_.open(QUrl(wsUrl));
}

void CallSignalingClient::disconnect()
{
    qDebug() << "CallSignalingClient: intentional disconnect";
    intentionalDisconnect_ = true;
    authenticated_ = false;
    if (ws_.state() != QAbstractSocket::UnconnectedState) {
        ws_.close();
    }
}

bool CallSignalingClient::isConnected() const
{
    return ws_.state() == QAbstractSocket::ConnectedState && authenticated_;
}

// ---------------------------------------------------------------------------
// Outgoing signaling
// ---------------------------------------------------------------------------

void CallSignalingClient::sendOffer(const QString &sdp)
{
    if (!isConnected()) {
        qWarning() << "CallSignalingClient: not connected, cannot send offer";
        return;
    }
    QJsonObject frame;
    frame["type"] = QStringLiteral("offer");
    frame["sdp"] = sdp;
    ws_.sendTextMessage(QJsonDocument(frame).toJson(QJsonDocument::Compact));
}

void CallSignalingClient::sendAnswer(const QString &sdp)
{
    if (!isConnected()) {
        qWarning() << "CallSignalingClient: not connected, cannot send answer";
        return;
    }
    QJsonObject frame;
    frame["type"] = QStringLiteral("answer");
    frame["sdp"] = sdp;
    ws_.sendTextMessage(QJsonDocument(frame).toJson(QJsonDocument::Compact));
}

void CallSignalingClient::sendIceCandidate(const QString &candidate, const QString &mid)
{
    if (!isConnected()) {
        qWarning() << "CallSignalingClient: not connected, cannot send ICE candidate";
        return;
    }
    QJsonObject frame;
    frame["type"] = QStringLiteral("ice_candidate");
    frame["candidate"] = candidate;
    frame["mid"] = mid;
    ws_.sendTextMessage(QJsonDocument(frame).toJson(QJsonDocument::Compact));
}

void CallSignalingClient::sendHangup()
{
    if (!isConnected()) {
        qWarning() << "CallSignalingClient: not connected, cannot send hangup";
        return;
    }
    QJsonObject frame;
    frame["type"] = QStringLiteral("hangup");
    ws_.sendTextMessage(QJsonDocument(frame).toJson(QJsonDocument::Compact));
}

// ---------------------------------------------------------------------------
// Private slots
// ---------------------------------------------------------------------------

void CallSignalingClient::onConnected()
{
    qDebug() << "CallSignalingClient: WebSocket connected, sending auth";
    QJsonObject auth;
    auth["type"] = QStringLiteral("auth");
    auth["token"] = token_;
    ws_.sendTextMessage(QJsonDocument(auth).toJson(QJsonDocument::Compact));

    authenticated_ = true;
    emit connected();
}

void CallSignalingClient::onDisconnected()
{
    qDebug() << "CallSignalingClient: WebSocket disconnected, closeCode:" << ws_.closeCode();
    authenticated_ = false;
    emit disconnected();
}

void CallSignalingClient::onTextMessageReceived(const QString &text)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "CallSignalingClient: JSON parse error:" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qDebug() << "CallSignalingClient: received non-object JSON frame";
        return;
    }

    handleFrame(doc.object());
}

// ---------------------------------------------------------------------------
// Frame dispatch
// ---------------------------------------------------------------------------

void CallSignalingClient::handleFrame(const QJsonObject &frame)
{
    QString type = frame.value("type").toString();

    if (type == "offer") {
        QString sdp = frame.value("sdp").toString();
        emit offerReceived(sdp);
    } else if (type == "answer") {
        QString sdp = frame.value("sdp").toString();
        emit answerReceived(sdp);
    } else if (type == "ice_candidate") {
        QString candidate = frame.value("candidate").toString();
        QString mid = frame.value("mid").toString();
        emit iceCandidateReceived(candidate, mid);
    } else if (type == "hangup") {
        emit hangupReceived();
    } else if (type == "error") {
        QString code = frame.value("code").toString();
        QString message = frame.value("message").toString();
        emit errorReceived(code, message);
    } else {
        qDebug() << "CallSignalingClient: unknown frame type:" << type;
    }
}
