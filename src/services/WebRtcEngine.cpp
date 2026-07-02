#include "WebRtcEngine.h"
#include <QDebug>
#include <QVideoSink>

#ifdef HAS_LIBDATACHANNEL
#include <QVideoFrame>
#include <QImage>
#endif

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

WebRtcEngine::WebRtcEngine(QObject *parent)
    : QObject(parent)
{
#ifdef HAS_LIBDATACHANNEL
    available_ = true;
    qDebug() << "WebRtcEngine: libdatachannel available";
#else
    available_ = false;
    qDebug() << "WebRtcEngine: libdatachannel NOT available";
    // Delay the signal slightly so QML has time to connect to it.
    QMetaObject::invokeMethod(this, &WebRtcEngine::notAvailable, Qt::QueuedConnection);
#endif
}

WebRtcEngine::~WebRtcEngine()
{
    close();
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool WebRtcEngine::isAvailable() const
{
    return available_;
}

void WebRtcEngine::createPeerConnection(bool isOfferer)
{
#ifdef HAS_LIBDATACHANNEL
    if (!available_) return;

    // Close any existing connection first
    close();

    isOfferer_ = isOfferer;

    rtc::Configuration config;
    // STUN servers for NAT traversal
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");
    // Additional STUN/TURN can be configured here

    pc_ = std::make_unique<rtc::PeerConnection>(config);

    // --- Local SDP generated callback ---
    pc_->onLocalDescription([this](rtc::Description desc) {
        QString sdp = QString::fromStdString(desc);
        QString type = QString::fromStdString(desc.typeString());
        qDebug() << "WebRtcEngine: local description generated, type:" << type;
        emit localSdpGenerated(sdp, type);
    });

    // --- Local ICE candidate callback ---
    pc_->onLocalCandidate([this](rtc::Candidate cand) {
        QString candidate = QString::fromStdString(cand.candidate());
        QString mid = QString::fromStdString(cand.mid());
        qDebug() << "WebRtcEngine: local ICE candidate, mid:" << mid;
        emit localIceCandidate(candidate, mid);
    });

    // --- State change callback ---
    pc_->onStateChange([this](rtc::PeerConnection::State state) {
        qDebug() << "WebRtcEngine: peer connection state:" << static_cast<int>(state);
    });

    // --- Gathering state callback ---
    pc_->onGatheringStateChange([this](rtc::PeerConnection::GatheringState state) {
        qDebug() << "WebRtcEngine: gathering state:" << static_cast<int>(state);
    });

    // --- Remote track callback (video) ---
    pc_->onTrack([this](std::shared_ptr<rtc::Track> track) {
        QString mid = QString::fromStdString(track->mid());
        QString desc = QString::fromStdString(track->description());
        qDebug() << "WebRtcEngine: remote track received, mid:" << mid << "desc:" << desc;

        // libdatachannel tracks emit samples via a callback.
        // We expose a QVideoSink that QML can render.
        // Actual frame delivery requires platform-specific plumbing;
        // for now we signal that a remote video track is ready.
        // The VideoManager will handle frame conversion from the track's
        // onMessage / onFrame callback.
        Q_UNUSED(track)
        // emit remoteVideoTrackReady(videoSink);
    });

    // --- Data channel (unused for basic call, but available) ---
    pc_->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
        Q_UNUSED(dc)
        qDebug() << "WebRtcEngine: data channel opened";
    });

    qDebug() << "WebRtcEngine: PeerConnection created, isOfferer:" << isOfferer;
#else
    Q_UNUSED(isOfferer)
    qWarning() << "WebRtcEngine: createPeerConnection called but libdatachannel not available";
#endif
}

void WebRtcEngine::setRemoteSdp(const QString &sdp, const QString &type)
{
#ifdef HAS_LIBDATACHANNEL
    if (!pc_) {
        qWarning() << "WebRtcEngine: setRemoteSdp called with no PeerConnection";
        return;
    }
    qDebug() << "WebRtcEngine: setting remote description, type:" << type;
    pc_->setRemoteDescription(sdp.toStdString(), type.toStdString());
#else
    Q_UNUSED(sdp)
    Q_UNUSED(type)
#endif
}

void WebRtcEngine::addRemoteCandidate(const QString &candidate, const QString &mid)
{
#ifdef HAS_LIBDATACHANNEL
    if (!pc_) {
        qWarning() << "WebRtcEngine: addRemoteCandidate called with no PeerConnection";
        return;
    }
    qDebug() << "WebRtcEngine: adding remote ICE candidate, mid:" << mid;
    pc_->addRemoteCandidate(candidate.toStdString(), mid.toStdString());
#else
    Q_UNUSED(candidate)
    Q_UNUSED(mid)
#endif
}

void WebRtcEngine::close()
{
#ifdef HAS_LIBDATACHANNEL
    if (pc_) {
        qDebug() << "WebRtcEngine: closing PeerConnection";
        pc_->close();
        pc_.reset();
        audioTrack_.reset();
        videoTrack_.reset();
    }
#endif
}
