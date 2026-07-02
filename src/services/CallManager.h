#pragma once
#include <QObject>
#include <QString>

class CallSignalingClient;
class WebRtcEngine;
class AudioManager;
class VideoManager;
class CallRepository;
class TokenManager;

/// Orchestrates the full call lifecycle: signaling, WebRTC, audio, and video.
///
/// State machine: idle → ringing → connecting → active → ended → idle
///
/// Exposed to QML as a context property so pages can bind to callState
/// and invoke startCall / acceptCall / hangup.
class CallManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString callState READ callState NOTIFY callStateChanged)
    Q_PROPERTY(QString callType READ callType NOTIFY callTypeChanged)
    Q_PROPERTY(QString remoteName READ remoteName NOTIFY remoteNameChanged)
    Q_PROPERTY(AudioManager* audioManager READ audioManager CONSTANT)
    Q_PROPERTY(VideoManager* videoManager READ videoManager CONSTANT)
public:
    explicit CallManager(CallRepository *callRepo,
                         TokenManager *tokenManager,
                         QObject *parent = nullptr);
    ~CallManager() override;

    QString callState() const;
    QString callType() const;
    QString remoteName() const;

    AudioManager* audioManager() const;
    VideoManager* videoManager() const;

    // --- QML-invokable actions ---

    /// Start an outgoing call. POSTs /call/start, then opens signaling WS.
    Q_INVOKABLE void startCall(const QString &conversationId,
                               const QString &type = "voice");

    /// Accept an incoming call. Opens signaling WS and creates answer.
    Q_INVOKABLE void acceptCall(const QString &callId);

    /// End the current call (local or remote hangup).
    Q_INVOKABLE void hangup();

    Q_INVOKABLE void toggleMute();
    Q_INVOKABLE void toggleVideo();

signals:
    void callStateChanged(const QString &state);
    void callTypeChanged(const QString &type);
    void remoteNameChanged(const QString &name);

    /// Emitted when an incoming call invitation is received.
    void incomingCall(const QString &callId, const QString &callerName,
                      const QString &callType);

    /// Emitted when the call ends (local or remote).
    void callEnded(const QString &reason);

private slots:
    // --- Signaling callbacks ---
    void onSignalingConnected();
    void onSignalingOfferReceived(const QString &sdp);
    void onSignalingAnswerReceived(const QString &sdp);
    void onSignalingIceCandidate(const QString &candidate, const QString &mid);
    void onSignalingHangup();

    // --- WebRTC callbacks ---
    void onLocalSdpGenerated(const QString &sdp, const QString &type);
    void onLocalIceCandidate(const QString &candidate, const QString &mid);

private:
    void setCallState(const QString &state);
    void setCallType(const QString &type);

    /// Build the WS URL for the signaling endpoint.
    QString signalingUrl(const QString &id) const;

    /// Tear down signaling, WebRTC, and reset state.
    void cleanup();

    // --- Owned sub-components ---
    CallSignalingClient *signaling_ = nullptr;
    WebRtcEngine *webRtc_ = nullptr;
    AudioManager *audioManager_ = nullptr;
    VideoManager *videoManager_ = nullptr;

    // --- External dependencies (not owned) ---
    CallRepository *callRepo_ = nullptr;
    TokenManager *tokenManager_ = nullptr;

    // --- State ---
    QString callState_ = "idle";
    QString callType_ = "voice";
    QString remoteName_;
    QString currentCallId_;
    QString currentConversationId_;
    QString pendingSdp_;   // for outgoing: the generated offer to send after WS connects
    bool isOfferer_ = false;
    bool signalingReady_ = false;
};
