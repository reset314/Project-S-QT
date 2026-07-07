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

    void connectToServer(const QString &wsUrl, const QString &token,
                         const QString &deviceId = {},
                         const QString &deviceType = "qt");
    void disconnect();
    bool isConnected() const;
    void sendPong();
    void sendSync(int lastSeq);

    /// Stable device ID — generated once and persisted.
    QString deviceId() const { return deviceId_; }
    void setDeviceId(const QString &id) { deviceId_ = id; }

signals:
    void connected();
    void disconnected();
    void streamInit(const QString &aiUserId, const QString &messageId,
                    const QString &timestamp);
    void streamChunk(const QString &aiUserId, const QString &chunk);
    void streamDone(const QString &aiUserId, const QString &messageId,
                    const QString &content);
    void streamError(const QString &aiUserId, const QString &code,
                     const QString &message);
    void proactiveMessage(const QString &aiUserId, const QJsonObject &payload);
    /// Fired when an event payload arrives from the server.
    void eventReceived(int seq, const QString &eventType,
                       const QJsonObject &payload);
    /// Fired on auth_ok with current server seq.
    void ready(int serverSeq);

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
    int serverCurrentSeq_ = 0;
    QString wsUrl_;
    QString token_;
    QString deviceId_;
    QString deviceType_ = "qt";
};
