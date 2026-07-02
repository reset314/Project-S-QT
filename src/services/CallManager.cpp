#include "CallManager.h"

#include "../core/network/CallSignalingClient.h"
#include "../core/auth/TokenManager.h"
#include "../repositories/CallRepository.h"
#include "WebRtcEngine.h"
#include "AudioManager.h"
#include "VideoManager.h"

#include <QDebug>

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

CallManager::CallManager(CallRepository *callRepo,
                         TokenManager *tokenManager,
                         QObject *parent)
    : QObject(parent)
    , callRepo_(callRepo)
    , tokenManager_(tokenManager)
{
    // --- Create owned sub-components ---
    signaling_ = new CallSignalingClient(this);
    webRtc_ = new WebRtcEngine(this);
    audioManager_ = new AudioManager(this);
    videoManager_ = new VideoManager(this);

    // --- Wire signaling signals ---
    QObject::connect(signaling_, &CallSignalingClient::connected,
                     this, &CallManager::onSignalingConnected);
    QObject::connect(signaling_, &CallSignalingClient::offerReceived,
                     this, &CallManager::onSignalingOfferReceived);
    QObject::connect(signaling_, &CallSignalingClient::answerReceived,
                     this, &CallManager::onSignalingAnswerReceived);
    QObject::connect(signaling_, &CallSignalingClient::iceCandidateReceived,
                     this, &CallManager::onSignalingIceCandidate);
    QObject::connect(signaling_, &CallSignalingClient::hangupReceived,
                     this, &CallManager::onSignalingHangup);

    // --- Wire WebRTC signals ---
    QObject::connect(webRtc_, &WebRtcEngine::localSdpGenerated,
                     this, &CallManager::onLocalSdpGenerated);
    QObject::connect(webRtc_, &WebRtcEngine::localIceCandidate,
                     this, &CallManager::onLocalIceCandidate);

    qDebug() << "CallManager: initialized";
}

CallManager::~CallManager()
{
    cleanup();
}

// ---------------------------------------------------------------------------
// QML properties
// ---------------------------------------------------------------------------

QString CallManager::callState() const { return callState_; }
QString CallManager::callType() const { return callType_; }
QString CallManager::remoteName() const { return remoteName_; }

AudioManager* CallManager::audioManager() const { return audioManager_; }
VideoManager* CallManager::videoManager() const { return videoManager_; }

// ---------------------------------------------------------------------------
// startCall  — outgoing call
// ---------------------------------------------------------------------------

void CallManager::startCall(const QString &conversationId, const QString &type)
{
    if (callState_ != "idle") {
        qWarning() << "CallManager: cannot start call, state is" << callState_;
        return;
    }

    setCallType(type);
    currentConversationId_ = conversationId;
    isOfferer_ = true;
    setCallState("connecting");

    // Step 1: POST /call/start to register the call with the server
    auto result = callRepo_->startCall(conversationId, type);
    if (!result) {
        qWarning() << "CallManager: failed to start call:" << result.error().code.c_str();
        emit callEnded(QString::fromStdString(result.error().message));
        setCallState("ended");
        return;
    }

    currentCallId_ = QString::fromStdString(result->conversationId);
    qDebug() << "CallManager: call started, callId:" << currentCallId_;

    // Step 2: Open signaling WebSocket
    signaling_->connectToServer(signalingUrl(currentCallId_),
                                tokenManager_->accessToken());

    // Step 3: Create PeerConnection (offerer side)
    // The actual offer generation happens once the WS is connected
    // and we receive the connected signal — see onSignalingConnected().
    webRtc_->createPeerConnection(true);

    // If this is a video call, start the camera
    if (type == "video") {
        videoManager_->startCamera();
    }
}

// ---------------------------------------------------------------------------
// acceptCall  — incoming call
// ---------------------------------------------------------------------------

void CallManager::acceptCall(const QString &callId)
{
    if (callState_ != "ringing") {
        qWarning() << "CallManager: cannot accept call, state is" << callState_;
        return;
    }

    currentCallId_ = callId;
    isOfferer_ = false;
    setCallState("connecting");

    // Open signaling WebSocket to receive the offer from the caller.
    signaling_->connectToServer(signalingUrl(callId),
                                tokenManager_->accessToken());

    // Create PeerConnection (answerer side).
    // The remote offer will arrive via the signaling channel.
    webRtc_->createPeerConnection(false);
}

// ---------------------------------------------------------------------------
// hangup
// ---------------------------------------------------------------------------

void CallManager::hangup()
{
    if (callState_ == "idle" || callState_ == "ended") return;

    qDebug() << "CallManager: hangup";

    if (signaling_->isConnected()) {
        signaling_->sendHangup();
    }

    cleanup();
    setCallState("ended");
    emit callEnded("local_hangup");
}

// ---------------------------------------------------------------------------
// toggleMute / toggleVideo
// ---------------------------------------------------------------------------

void CallManager::toggleMute()
{
    audioManager_->toggleMute();
}

void CallManager::toggleVideo()
{
    if (callType_ != "video") return;
    videoManager_->toggleCamera();
}

// ---------------------------------------------------------------------------
// Private: state helpers
// ---------------------------------------------------------------------------

void CallManager::setCallState(const QString &state)
{
    if (callState_ != state) {
        callState_ = state;
        qDebug() << "CallManager: state →" << state;
        emit callStateChanged(callState_);
    }
}

void CallManager::setCallType(const QString &type)
{
    if (callType_ != type) {
        callType_ = type;
        emit callTypeChanged(type);
    }
}

QString CallManager::signalingUrl(const QString &id) const
{
    // Build the WS URL from the base server URL stored in TokenManager.
    // TokenManager stores something like "http://localhost:8000"
    // We need "ws://localhost:8000/ws/call/{id}"
    QString base = tokenManager_->serverUrl();
    // Replace http scheme with ws
    if (base.startsWith("https://")) {
        base.replace(0, 8, "wss://");
    } else if (base.startsWith("http://")) {
        base.replace(0, 7, "ws://");
    }
    if (!base.endsWith('/')) {
        base += '/';
    }
    return base + "ws/call/" + id;
}

void CallManager::cleanup()
{
    signalingReady_ = false;

    if (signaling_) {
        signaling_->disconnect();
    }

    if (webRtc_) {
        webRtc_->close();
    }

    if (videoManager_) {
        videoManager_->stopCamera();
    }

    pendingSdp_.clear();
}

// ---------------------------------------------------------------------------
// Signaling callbacks
// ---------------------------------------------------------------------------

void CallManager::onSignalingConnected()
{
    qDebug() << "CallManager: signaling connected";
    signalingReady_ = true;

    // If we are the offerer and still connecting, the WebRTC engine
    // should have already generated an offer via onLocalSdpGenerated.
    // If not yet, it will arrive shortly.

    // For answerer (incoming call), we wait for the remote offer to arrive
    // via onSignalingOfferReceived.
}

void CallManager::onSignalingOfferReceived(const QString &sdp)
{
    qDebug() << "CallManager: offer received";
    if (callState_ == "connecting") {
        // Set remote SDP on the WebRTC engine (this is the offer)
        webRtc_->setRemoteSdp(sdp, "offer");
        // The WebRTC engine will generate an answer via onLocalSdpGenerated
        setCallState("active");
    } else {
        qWarning() << "CallManager: unexpected offer in state" << callState_;
    }
}

void CallManager::onSignalingAnswerReceived(const QString &sdp)
{
    qDebug() << "CallManager: answer received";
    if (callState_ == "connecting") {
        // Set remote SDP on the WebRTC engine (this is the answer)
        webRtc_->setRemoteSdp(sdp, "answer");
        setCallState("active");
    } else {
        qWarning() << "CallManager: unexpected answer in state" << callState_;
    }
}

void CallManager::onSignalingIceCandidate(const QString &candidate, const QString &mid)
{
    qDebug() << "CallManager: remote ICE candidate, mid:" << mid;
    webRtc_->addRemoteCandidate(candidate, mid);
}

void CallManager::onSignalingHangup()
{
    qDebug() << "CallManager: remote hangup received";
    cleanup();
    setCallState("ended");
    emit callEnded("remote_hangup");
}

// ---------------------------------------------------------------------------
// WebRTC callbacks
// ---------------------------------------------------------------------------

void CallManager::onLocalSdpGenerated(const QString &sdp, const QString &type)
{
    qDebug() << "CallManager: local SDP generated, type:" << type;

    // If signaling is not yet ready, stash the SDP and send once connected
    if (!signalingReady_) {
        pendingSdp_ = sdp;
        return;
    }

    if (type == "offer") {
        signaling_->sendOffer(sdp);
    } else if (type == "answer") {
        signaling_->sendAnswer(sdp);
    }
}

void CallManager::onLocalIceCandidate(const QString &candidate, const QString &mid)
{
    if (signalingReady_) {
        signaling_->sendIceCandidate(candidate, mid);
    }
}
