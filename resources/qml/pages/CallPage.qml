import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 6.5

Window {
    id: callWindow
    width: 800
    height: 600
    title: callManager ? callManager.remoteName || qsTr("Call") : qsTr("Call")
    color: "#1a1a2e"
    flags: Qt.Window | Qt.WindowStaysOnTopHint
    visible: false

    property bool isVoiceOnly: callManager ? callManager.callType === "voice" : true
    property string currentState: callManager ? callManager.callState : "idle"

    // ── State bindings ────────────────────────────────────────────
    onCurrentStateChanged: {
        if (currentState === "active" || currentState === "connecting") {
            callWindow.show()
            callWindow.raise()
        } else if (currentState === "ended" || currentState === "idle") {
            callWindow.hide()
        }
    }

    // ── Remote video (fullscreen background) ──────────────────────
    Rectangle {
        id: remoteVideoArea
        anchors.fill: parent
        color: "#0d0d1a"

        // Video output for remote peer
        VideoOutput {
            anchors.fill: parent
            visible: !isVoiceOnly
            fillMode: VideoOutput.PreserveAspectFit
            // Connect to remote video sink from VideoManager
            videoSink: (callManager && callManager.videoManager)
                       ? callManager.videoManager.remoteVideoSink : null
        }

        // Voice-only: avatar placeholder + animated waveform
        Item {
            anchors.centerIn: parent
            visible: isVoiceOnly

            // Avatar circle
            Rectangle {
                id: avatarCircle
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -40
                width: 120; height: 120
                radius: 60
                color: Theme.primaryColor
                border.width: 3
                border.color: Theme.primaryLight

                Text {
                    anchors.centerIn: parent
                    text: callManager ? (callManager.remoteName || "?") : "?"
                    color: "white"
                    font.pixelSize: 36
                    font.weight: Font.Bold
                }
            }

            Text {
                anchors.top: avatarCircle.bottom
                anchors.topMargin: Theme.spacingLarge
                anchors.horizontalCenter: parent.horizontalCenter
                text: callManager ? (callManager.remoteName || qsTr("Connecting...")) : ""
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSizeBody
            }

            // Animated waveform bars (simple pulse animation)
            Row {
                anchors.top: avatarCircle.bottom
                anchors.topMargin: Theme.spacingXLarge + 20
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 6

                Repeater {
                    model: 5
                    Rectangle {
                        width: 4
                        height: 20 + Math.random() * 30
                        radius: 2
                        color: Theme.primaryLight
                        anchors.verticalCenter: parent.verticalCenter

                        SequentialAnimation on height {
                            loops: Animation.Infinite
                            running: isVoiceOnly && currentState === "active"
                            NumberAnimation {
                                from: 10; to: 40
                                duration: 400 + (index * 80)
                                easing.type: Easing.InOutQuad
                            }
                            NumberAnimation {
                                from: 40; to: 10
                                duration: 400 + (index * 80)
                                easing.type: Easing.InOutQuad
                            }
                        }
                    }
                }
            }
        }
    }

    // ── Self-view PIP (bottom-right) ──────────────────────────────
    Rectangle {
        id: selfView
        width: 160; height: 120
        anchors.right: parent.right
        anchors.bottom: controlBar.top
        anchors.margins: Theme.spacingMedium
        radius: Theme.cardRadius
        color: "#2a2a3e"
        border.width: 2
        border.color: Theme.primaryColor
        visible: !isVoiceOnly
        z: 10

        VideoOutput {
            anchors.fill: parent
            fillMode: VideoOutput.PreserveAspectCrop
            videoSink: (callManager && callManager.videoManager)
                       ? callManager.videoManager.localVideoSink : null
        }

        // Muted overlay on self-view
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "#cc000000"
            visible: callManager && callManager.audioManager
                     ? callManager.audioManager.muted : false

            Text {
                anchors.centerIn: parent
                text: "🔇"
                font.pixelSize: 24
            }
        }
    }

    // ── Call state overlay (connecting / ended) ──────────────────
    Rectangle {
        anchors.centerIn: parent
        width: stateLabel.implicitWidth + 40
        height: stateLabel.implicitHeight + 20
        radius: Theme.cardRadius
        color: "#cc1a1a2e"
        visible: currentState === "connecting" || currentState === "ringing"

        Text {
            id: stateLabel
            anchors.centerIn: parent
            text: currentState === "connecting" ? qsTr("Connecting...")
                  : currentState === "ringing" ? qsTr("Ringing...")
                  : ""
            color: "white"
            font.pixelSize: Theme.fontSizeHeading
        }
    }

    // ── Control bar (bottom) ─────────────────────────────────────
    Rectangle {
        id: controlBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 80
        color: "#cc1a1a2e"
        z: 20

        RowLayout {
            anchors.centerIn: parent
            spacing: Theme.spacingXLarge

            // ── Mute button ───────────────────────────────────
            Rectangle {
                Layout.preferredWidth: 48; Layout.preferredHeight: 48
                radius: 24
                color: micBtnMouse.containsMouse
                       ? Theme.sidebarHover
                       : "#33ffffff"

                property bool isMuted: callManager && callManager.audioManager
                                       ? callManager.audioManager.muted : false

                Text {
                    anchors.centerIn: parent
                    text: parent.isMuted ? "🔇" : "🎤"
                    font.pixelSize: 20
                }

                MouseArea {
                    id: micBtnMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (callManager) callManager.toggleMute()
                    }
                }
            }

            // ── Video toggle button ───────────────────────────
            Rectangle {
                Layout.preferredWidth: 48; Layout.preferredHeight: 48
                radius: 24
                color: videoBtnMouse.containsMouse
                       ? Theme.sidebarHover
                       : "#33ffffff"
                visible: !isVoiceOnly

                property bool isCameraOn: callManager && callManager.videoManager
                                          ? callManager.videoManager.cameraOn : false

                Text {
                    anchors.centerIn: parent
                    text: parent.isCameraOn ? "📹" : "📷"
                    font.pixelSize: 20
                }

                MouseArea {
                    id: videoBtnMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (callManager) callManager.toggleVideo()
                    }
                }
            }

            // ── Hangup button (always visible when in call) ──
            Rectangle {
                Layout.preferredWidth: 56; Layout.preferredHeight: 56
                radius: 28
                color: hangupMouse.containsMouse ? "#E57373" : Theme.errorColor

                Text {
                    anchors.centerIn: parent
                    text: "📞"
                    font.pixelSize: 24
                }

                MouseArea {
                    id: hangupMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (callManager) callManager.hangup()
                    }
                }
            }
        }
    }

    // ── Accept button (only visible for incoming calls) ──────────
    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: controlBar.top
        anchors.bottomMargin: Theme.spacingMedium
        width: acceptLabel.implicitWidth + 40
        height: 44
        radius: 22
        color: acceptMouse.containsMouse ? "#66BB6A" : Theme.successColor
        visible: currentState === "ringing"
        z: 15

        Text {
            id: acceptLabel
            anchors.centerIn: parent
            text: qsTr("Accept Call")
            color: "white"
            font.pixelSize: Theme.fontSizeSubheading
            font.weight: Font.Medium
        }

        MouseArea {
            id: acceptMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (callManager) callManager.acceptCall("")
            }
        }
    }

    // ── Close on Escape ──────────────────────────────────────────
    Keys.onEscapePressed: {
        if (callManager) callManager.hangup()
    }
}
