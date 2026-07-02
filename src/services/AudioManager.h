#pragma once
#include <QObject>
#include <QAudioDevice>

/// Manages audio device selection and mute state for voice/video calls.
/// Uses Qt Multimedia's QMediaCaptureSession + QAudioInput for capture
/// and QAudioSink for playback (when not using WebRTC's built-in audio).
class AudioManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool muted READ isMuted NOTIFY mutedChanged)
public:
    explicit AudioManager(QObject *parent = nullptr);

    bool isMuted() const;

    Q_INVOKABLE void toggleMute();
    Q_INVOKABLE void setMuted(bool muted);

    /// Currently selected microphone device.
    QAudioDevice microphoneDevice() const;

    /// Currently selected speaker device.
    QAudioDevice speakerDevice() const;

    Q_INVOKABLE void setMicrophoneDevice(const QAudioDevice &device);
    Q_INVOKABLE void setSpeakerDevice(const QAudioDevice &device);

    /// List available audio input devices.
    Q_INVOKABLE QStringList availableMicrophones() const;

    /// List available audio output devices.
    Q_INVOKABLE QStringList availableSpeakers() const;

signals:
    void mutedChanged(bool muted);

private:
    bool muted_ = false;
    QAudioDevice micDevice_;
    QAudioDevice speakerDevice_;
};
