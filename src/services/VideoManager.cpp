#include "VideoManager.h"

#include <QCameraDevice>
#include <QMediaDevices>
#include <QDebug>

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

VideoManager::VideoManager(QObject *parent)
    : QObject(parent)
    , localVideoSink_(new QVideoSink(this))
    , remoteVideoSink_(new QVideoSink(this))
{
    qDebug() << "VideoManager: initialized";
}

VideoManager::~VideoManager()
{
    stopCamera();
}

// ---------------------------------------------------------------------------
// Camera control
// ---------------------------------------------------------------------------

bool VideoManager::isCameraOn() const
{
    return cameraOn_;
}

void VideoManager::toggleCamera()
{
    setCameraOn(!cameraOn_);
}

void VideoManager::setCameraOn(bool on)
{
    if (cameraOn_ == on) return;

    if (on) {
        startCamera();
    } else {
        stopCamera();
    }
}

void VideoManager::startCamera()
{
    if (cameraOn_) return;

    // Find the default front-facing or first available camera
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        qWarning() << "VideoManager: no camera available";
        return;
    }

    // Prefer front-facing camera if available
    QCameraDevice selectedCamera = cameras.first();
    for (const auto &cam : cameras) {
        if (cam.position() == QCameraDevice::FrontFace) {
            selectedCamera = cam;
            break;
        }
    }

    camera_ = new QCamera(selectedCamera, this);
    captureSession_ = new QMediaCaptureSession(this);
    captureSession_->setCamera(camera_);
    captureSession_->setVideoSink(localVideoSink_);

    camera_->start();
    cameraOn_ = true;

    qDebug() << "VideoManager: camera started:" << selectedCamera.description();
    emit cameraOnChanged(true);
}

void VideoManager::stopCamera()
{
    if (!cameraOn_) return;

    if (camera_) {
        camera_->stop();
    }
    delete camera_;
    camera_ = nullptr;
    delete captureSession_;
    captureSession_ = nullptr;

    cameraOn_ = false;

    qDebug() << "VideoManager: camera stopped";
    emit cameraOnChanged(false);
}

// ---------------------------------------------------------------------------
// Video sinks
// ---------------------------------------------------------------------------

QVideoSink* VideoManager::localVideoSink() const
{
    return localVideoSink_;
}

QVideoSink* VideoManager::remoteVideoSink() const
{
    return remoteVideoSink_;
}

void VideoManager::setRemoteVideoSink(QVideoSink *sink)
{
    // Replace the remote sink. The old sink's connections are
    // automatically cleaned up when it is destroyed (parented to this).
    if (remoteVideoSink_ != sink) {
        // Keep ownership of the old sink
        remoteVideoSink_->deleteLater();
        remoteVideoSink_ = sink;
        if (remoteVideoSink_) {
            remoteVideoSink_->setParent(this);
        }
        emit remoteVideoSinkChanged();
        qDebug() << "VideoManager: remote video sink updated";
    }
}
