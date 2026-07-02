#pragma once
#include <QObject>
#include <QWebSocket>
#include <QJsonObject>

/// WebSocket client for WebRTC call signaling.
/// Connects to /ws/call/{conversationId}, sends auth token, then relays
/// SDP offers/answers and ICE candidates between the local WebRTC engine
/// and the remote peer via the signaling server.
class CallSignalingClient : public QObject {
    Q_OBJECT
public:
    explicit CallSignalingClient(QObject *parent = nullptr);
    ~CallSignalingClient() override;

    /// Open WebSocket to the signaling endpoint and authenticate.
    void connectToServer(const QString &wsUrl, const QString &token);

    /// Gracefully close the signaling connection.
    void disconnect();

    bool isConnected() const;

    // --- Outgoing signaling ---

    void sendOffer(const QString &sdp);
    void sendAnswer(const QString &sdp);
    void sendIceCandidate(const QString &candidate, const QString &mid);
    void sendHangup();

signals:
    void connected();
    void disconnected();

    // Incoming signaling events
    void offerReceived(const QString &sdp);
    void answerReceived(const QString &sdp);
    void iceCandidateReceived(const QString &candidate, const QString &mid);
    void hangupReceived();
    void errorReceived(const QString &code, const QString &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &text);

private:
    void handleFrame(const QJsonObject &frame);

    QWebSocket ws_;
    bool authenticated_ = false;
    bool intentionalDisconnect_ = false;
    QString token_;
};
