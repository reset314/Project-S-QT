#include "ChatStreamClient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocket>
#include <QTimer>
#include <QDebug>

ChatStreamClient::ChatStreamClient(QObject *parent)
    : QObject(parent)
{
    heartbeatTimer_.setSingleShot(false);
    pongTimeoutTimer_.setSingleShot(true);
    reconnectTimer_.setSingleShot(true);

    QObject::connect(&ws_, &QWebSocket::connected,
                     this, &ChatStreamClient::onConnected);
    QObject::connect(&ws_, &QWebSocket::disconnected,
                     this, &ChatStreamClient::onDisconnected);
    QObject::connect(&ws_, &QWebSocket::textMessageReceived,
                     this, &ChatStreamClient::onTextMessageReceived);
    QObject::connect(&ws_,
                     QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                     this, &ChatStreamClient::onError);
    QObject::connect(&heartbeatTimer_, &QTimer::timeout,
                     this, &ChatStreamClient::sendPing);
    QObject::connect(&pongTimeoutTimer_, &QTimer::timeout,
                     this, [this]() {
        qDebug() << "ChatStreamClient: pong timeout, disconnecting";
        intentionalDisconnect_ = false; // trigger reconnect
        ws_.close();
    });
    QObject::connect(&reconnectTimer_, &QTimer::timeout,
                     this, &ChatStreamClient::onReconnectTimer);
}

ChatStreamClient::~ChatStreamClient()
{
    intentionalDisconnect_ = true;
    stopHeartbeat();
    reconnectTimer_.stop();
    if (ws_.state() != QAbstractSocket::UnconnectedState) {
        ws_.close();
    }
}

void ChatStreamClient::connectToServer(const QString &wsUrl, const QString &token)
{
    wsUrl_ = wsUrl;
    token_ = token;

    // Stop any pending reconnect
    reconnectTimer_.stop();

    // If already connected, disconnect first
    if (ws_.state() != QAbstractSocket::UnconnectedState) {
        intentionalDisconnect_ = true;
        ws_.close();
    }

    intentionalDisconnect_ = false;
    reconnectAttempts_ = 0;
    authenticated_ = false;

    qDebug() << "ChatStreamClient: connecting to" << wsUrl_;
    ws_.open(QUrl(wsUrl_));
}

void ChatStreamClient::disconnect()
{
    qDebug() << "ChatStreamClient: intentional disconnect";
    intentionalDisconnect_ = true;
    reconnectAttempts_ = 0;
    stopHeartbeat();
    reconnectTimer_.stop();
    authenticated_ = false;
    if (ws_.state() != QAbstractSocket::UnconnectedState) {
        ws_.close();
    }
}

bool ChatStreamClient::isConnected() const
{
    return ws_.state() == QAbstractSocket::ConnectedState && authenticated_;
}

void ChatStreamClient::sendPong()
{
    if (ws_.state() == QAbstractSocket::ConnectedState) {
        QJsonObject pong;
        pong["type"] = QStringLiteral("pong");
        ws_.sendTextMessage(QJsonDocument(pong).toJson(QJsonDocument::Compact));
    }
}

// --- Private slots ---

void ChatStreamClient::onConnected()
{
    qDebug() << "ChatStreamClient: WebSocket connected, sending auth";
    // Send auth frame
    QJsonObject auth;
    auth["type"] = QStringLiteral("auth");
    auth["token"] = token_;
    ws_.sendTextMessage(QJsonDocument(auth).toJson(QJsonDocument::Compact));

    reconnectAttempts_ = 0;

    // Start heartbeat
    startHeartbeat();

    // NOTE: authenticated_ and connected() are deferred until the server
    // responds with the first non-pong frame (see onTextMessageReceived).
}

void ChatStreamClient::onDisconnected()
{
    qDebug() << "ChatStreamClient: WebSocket disconnected, closeCode:" << ws_.closeCode();
    authenticated_ = false;
    stopHeartbeat();
    reconnectTimer_.stop();

    emit disconnected();

    // If server closed with 1008 (auth rejected), do not reconnect
    if (ws_.closeCode() == 1008) {
        qDebug() << "ChatStreamClient: auth rejected (close code 1008), not reconnecting";
        intentionalDisconnect_ = true;
        return;
    }

    // Schedule reconnect if not intentional
    if (!intentionalDisconnect_) {
        scheduleReconnect();
    }
}

void ChatStreamClient::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    qDebug() << "ChatStreamClient: WebSocket error:" << ws_.errorString();
}

void ChatStreamClient::onTextMessageReceived(const QString &text)
{
    // Log raw frame for debugging (first 300 chars)
    qDebug() << "ChatStreamClient: raw frame:" << text.left(300);

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "ChatStreamClient: JSON parse error:" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qDebug() << "ChatStreamClient: received non-object JSON frame";
        return;
    }

    // Mark as authenticated on any server frame after auth (including pong).
    // If the server rejected auth it sends close code 1008 — we never get here.
    if (!authenticated_) {
        authenticated_ = true;
        qDebug() << "ChatStreamClient: authenticated by server frame";
        emit connected();
    }

    handleFrame(doc.object());
}

void ChatStreamClient::sendPing()
{
    if (ws_.state() == QAbstractSocket::ConnectedState) {
        QJsonObject ping;
        ping["type"] = QStringLiteral("ping");
        ws_.sendTextMessage(QJsonDocument(ping).toJson(QJsonDocument::Compact));
    }
}

void ChatStreamClient::onReconnectTimer()
{
    if (intentionalDisconnect_) {
        return;
    }

    qDebug() << "ChatStreamClient: attempting reconnect (#" << reconnectAttempts_ << ")";
    ws_.open(QUrl(wsUrl_));
}

// --- Private helpers ---

void ChatStreamClient::handleFrame(const QJsonObject &frame)
{
    QString type = frame.value("type").toString();

    // Server wraps data in a "payload" sub-object — extract it.
    // Fall back to the top-level frame if no payload exists.
    QJsonObject data = frame.value("payload").toObject();
    if (data.isEmpty())
        data = frame;

    if (type == "pong") {
        resetPongTimeout();
        return;
    }

    if (type == "ping") {
        // Server-initiated heartbeat — respond with pong
        sendPong();
        return;
    }

    if (type == "stream_message_init") {
        QString conversationId = data.value("conversation_id").toString();
        QString messageId = data.value("message_id").toString();
        QString timestamp = data.value("timestamp").toString();
        emit streamInit(conversationId, messageId, timestamp);
        return;
    }

    if (type == "stream_chunk") {
        QString conversationId = data.value("conversation_id").toString();
        // The chunk field contains the full accumulated content, not just the delta
        QString chunk = data.value("chunk").toString();
        emit streamChunk(conversationId, chunk);
        return;
    }

    if (type == "stream_message_done") {
        QString conversationId = data.value("conversation_id").toString();
        QString messageId = data.value("message_id").toString();
        QString content = data.value("content").toString();
        qDebug() << "ChatStreamClient: stream_message_done conv:" << conversationId
                 << "msgId:" << messageId << "contentLen:" << content.size();
        emit streamDone(conversationId, messageId, content);
        return;
    }

    if (type == "stream_error") {
        QString conversationId = data.value("conversation_id").toString();
        QString code = data.value("code").toString();
        QString message = data.value("message").toString();
        emit streamError(conversationId, code, message);
        return;
    }

    if (type == "proactive_message") {
        QString conversationId = data.value("conversation_id").toString();
        emit proactiveMessage(conversationId, frame);
        return;
    }

    qDebug() << "ChatStreamClient: unknown frame type:" << type;
}

void ChatStreamClient::startHeartbeat()
{
    // Send ping every 30 seconds
    heartbeatTimer_.start(30000);
    resetPongTimeout();
}

void ChatStreamClient::stopHeartbeat()
{
    heartbeatTimer_.stop();
    pongTimeoutTimer_.stop();
}

void ChatStreamClient::resetPongTimeout()
{
    // Expect pong within 60 seconds
    pongTimeoutTimer_.start(60000);
}

void ChatStreamClient::scheduleReconnect()
{
    // Guard: don't schedule if already connecting/connected or intentionally closed
    if (ws_.state() == QAbstractSocket::ConnectingState ||
        ws_.state() == QAbstractSocket::ConnectedState) {
        return;
    }

    int delayMs;
    if (reconnectAttempts_ < 3) {
        delayMs = 1000; // 1s for first 3 attempts
    } else if (reconnectAttempts_ < 5) {
        delayMs = 3000; // 3s for next 2 attempts
    } else {
        delayMs = 30000; // 30s thereafter
    }

    qDebug() << "ChatStreamClient: scheduling reconnect in" << delayMs << "ms"
             << "(attempt" << reconnectAttempts_ << ")";

    reconnectAttempts_++;
    reconnectTimer_.start(delayMs);
}
