#pragma once
#include <QObject>
#include <QCamera>
#include <QVideoSink>
#include <QMediaCaptureSession>
#include <memory>

/// Manages local camera capture and provides video sinks for QML VideoOutput.
///
/// Two video sinks are exposed:
/// - localVideoSink:  receives frames from the local camera (self-view).
/// - remoteVideoSink: receives frames from the remote WebRTC track.
///
/// The remote video sink is populated by CallManager when the WebRTC engine
/// receives a remote track.
class VideoManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QObject* localVideoSink READ localVideoSinkObj NOTIFY localVideoSinkChanged)
    Q_PROPERTY(QObject* remoteVideoSink READ remoteVideoSinkObj NOTIFY remoteVideoSinkChanged)
    Q_PROPERTY(bool cameraOn READ isCameraOn NOTIFY cameraOnChanged)
public:
    explicit VideoManager(QObject *parent = nullptr);
    ~VideoManager() override;

    bool isCameraOn() const;

    Q_INVOKABLE void toggleCamera();
    Q_INVOKABLE void setCameraOn(bool on);

    QVideoSink* localVideoSink() const;
    QVideoSink* remoteVideoSink() const;
    QObject* localVideoSinkObj() const { return localVideoSink_; }
    QObject* remoteVideoSinkObj() const { return remoteVideoSink_; }

    /// Called by CallManager when a remote video track is ready.
    /// The provided sink will receive frames from the remote peer.
    void setRemoteVideoSink(QVideoSink *sink);

    /// Start the local camera and connect it to the local video sink.
    void startCamera();

    /// Stop the local camera.
    void stopCamera();

signals:
    void cameraOnChanged(bool on);
    void localVideoSinkChanged();
    void remoteVideoSinkChanged();

private:
    QCamera *camera_ = nullptr;
    QMediaCaptureSession *captureSession_ = nullptr;
    QVideoSink *localVideoSink_ = nullptr;
    QVideoSink *remoteVideoSink_ = nullptr;
    bool cameraOn_ = false;
};

Q_DECLARE_OPAQUE_POINTER(QVideoSink*)
