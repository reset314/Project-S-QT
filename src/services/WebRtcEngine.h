#pragma once
#include <QObject>
#include <QString>

#ifdef HAS_LIBDATACHANNEL
#include <memory>
#include <rtc/rtc.hpp>
#endif

class QVideoSink;

/// Wraps libdatachannel PeerConnection for WebRTC.
/// When HAS_LIBDATACHANNEL is not defined, all methods are no-ops and
/// the notAvailable() signal is emitted on construction.
class WebRtcEngine : public QObject {
    Q_OBJECT
public:
    explicit WebRtcEngine(QObject *parent = nullptr);
    ~WebRtcEngine() override;

    /// Whether libdatachannel is available at runtime.
    bool isAvailable() const;

    /// Create a PeerConnection with STUN/TURN configuration.
    /// @param isOfferer  true if this peer generates the offer, false for answer.
    void createPeerConnection(bool isOfferer);

    /// Set the remote SDP description.
    void setRemoteSdp(const QString &sdp, const QString &type);

    /// Add a remote ICE candidate.
    void addRemoteCandidate(const QString &candidate, const QString &mid);

    /// Close and tear down the PeerConnection.
    void close();

signals:
    /// Emitted once during construction when libdatachannel is not available.
    void notAvailable();

    /// Local SDP generated (offer or answer ready to send via signaling).
    void localSdpGenerated(const QString &sdp, const QString &type);

    /// Local ICE candidate ready to send.
    void localIceCandidate(const QString &candidate, const QString &mid);

    /// Remote video track is ready; the caller can use the video sink for rendering.
    void remoteVideoTrackReady(QVideoSink *videoSink);

private:
#ifdef HAS_LIBDATACHANNEL
    std::unique_ptr<rtc::PeerConnection> pc_;

    // Hold a reference to the audio/video tracks so they are not destroyed.
    std::shared_ptr<rtc::Track> audioTrack_;
    std::shared_ptr<rtc::Track> videoTrack_;

    bool isOfferer_ = false;
#endif

    bool available_ = false;
};
