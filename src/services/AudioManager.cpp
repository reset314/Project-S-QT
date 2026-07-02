#include "AudioManager.h"

#include <QAudioDevice>
#include <QMediaDevices>
#include <QDebug>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

AudioManager::AudioManager(QObject *parent)
    : QObject(parent)
{
    // Default to the system default audio devices
    micDevice_ = QMediaDevices::defaultAudioInput();
    speakerDevice_ = QMediaDevices::defaultAudioOutput();

    qDebug() << "AudioManager: default mic:" << micDevice_.description()
             << "default speaker:" << speakerDevice_.description();
}

// ---------------------------------------------------------------------------
// Mute
// ---------------------------------------------------------------------------

bool AudioManager::isMuted() const
{
    return muted_;
}

void AudioManager::toggleMute()
{
    setMuted(!muted_);
}

void AudioManager::setMuted(bool muted)
{
    if (muted_ != muted) {
        muted_ = muted;
        qDebug() << "AudioManager: muted:" << muted_;
        emit mutedChanged(muted_);
    }
}

// ---------------------------------------------------------------------------
// Device selection
// ---------------------------------------------------------------------------

QAudioDevice AudioManager::microphoneDevice() const
{
    return micDevice_;
}

QAudioDevice AudioManager::speakerDevice() const
{
    return speakerDevice_;
}

void AudioManager::setMicrophoneDevice(const QAudioDevice &device)
{
    if (micDevice_ != device) {
        micDevice_ = device;
        qDebug() << "AudioManager: microphone set to:" << device.description();
    }
}

void AudioManager::setSpeakerDevice(const QAudioDevice &device)
{
    if (speakerDevice_ != device) {
        speakerDevice_ = device;
        qDebug() << "AudioManager: speaker set to:" << device.description();
    }
}

// ---------------------------------------------------------------------------
// Device enumeration
// ---------------------------------------------------------------------------

QStringList AudioManager::availableMicrophones() const
{
    QStringList list;
    const auto devices = QMediaDevices::audioInputs();
    for (const auto &d : devices) {
        list.append(d.description());
    }
    return list;
}

QStringList AudioManager::availableSpeakers() const
{
    QStringList list;
    const auto devices = QMediaDevices::audioOutputs();
    for (const auto &d : devices) {
        list.append(d.description());
    }
    return list;
}
