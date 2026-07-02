#pragma once
#include <QObject>
#include <QWebSocket>
#include <QJsonObject>
#include <QTimer>

class ChatStreamClient : public QObject {
    Q_OBJECT
public:
    explicit ChatStreamClient(QObject *parent = nullptr);
    ~ChatStreamClient() override;

    void connectToServer(const QString &wsUrl, const QString &token);
    void disconnect();
    bool isConnected() const;
    void sendPong();

signals:
    void connected();
    void disconnected();
    void streamInit(const QString &conversationId, const QString &messageId,
                    const QString &timestamp);
    void streamChunk(const QString &conversationId, const QString &chunk);
    void streamDone(const QString &conversationId, const QString &messageId,
                    const QString &content);
    void streamError(const QString &conversationId, const QString &code,
                     const QString &message);
    void proactiveMessage(const QString &conversationId, const QJsonObject &payload);

private slots:
    void onTextMessageReceived(const QString &text);
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void sendPing();
    void onReconnectTimer();

private:
    void handleFrame(const QJsonObject &frame);
    void startHeartbeat();
    void stopHeartbeat();
    void resetPongTimeout();
    void scheduleReconnect();

    QWebSocket ws_;
    QTimer heartbeatTimer_;
    QTimer pongTimeoutTimer_;
    QTimer reconnectTimer_;
    bool authenticated_ = false;
    bool intentionalDisconnect_ = false;
    int reconnectAttempts_ = 0;
    QString wsUrl_;
    QString token_;
};
