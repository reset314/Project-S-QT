import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import "../theme/ThemeConfig.qml" as Theme

Dialog {
    id: callInviteDialog
    width: 400
    height: 240
    modal: true

    property string callerName: ""
    property string callType: "voice"   // "voice" | "video"
    property string callId: ""

    signal accepted(string callId)
    signal declined(string callId)

    // Center in parent window
    x: (parent ? parent.width : 800) / 2 - width / 2
    y: (parent ? parent.height : 600) / 2 - height / 2

    background: Rectangle {
        radius: Theme.ThemeConfig.cardRadius
        color: Theme.ThemeConfig.surfaceColor

        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0; verticalOffset: 8
            radius: 24; samples: 33
            color: "#30000000"
        }
    }

    contentItem: ColumnLayout {
        spacing: Theme.ThemeConfig.spacingLarge

        // ── Call icon ─────────────────────────────────────────────
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: Theme.ThemeConfig.spacingLarge
            width: 56; height: 56
            radius: 28
            color: callType === "video" ? Theme.ThemeConfig.primaryColor
                                        : Theme.ThemeConfig.successColor

            Text {
                anchors.centerIn: parent
                text: callType === "video" ? "📹" : "📞"
                font.pixelSize: 28
            }
        }

        // ── Message ───────────────────────────────────────────────
        Text {
            Layout.fillWidth: true
            Layout.leftMargin: Theme.ThemeConfig.spacingLarge
            Layout.rightMargin: Theme.ThemeConfig.spacingLarge
            text: qsTr("Incoming %1 call from %2")
                  .arg(callType === "video" ? qsTr("video") : qsTr("voice"))
                  .arg(callerName || qsTr("Unknown"))
            color: Theme.ThemeConfig.textPrimary
            font.pixelSize: Theme.ThemeConfig.fontSizeBody
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
        }

        Item { Layout.fillHeight: true }

        // ── Buttons ───────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: Theme.ThemeConfig.spacingLarge
            Layout.leftMargin: Theme.ThemeConfig.spacingLarge
            Layout.rightMargin: Theme.ThemeConfig.spacingLarge
            spacing: Theme.ThemeConfig.spacingMedium

            Item { Layout.fillWidth: true }

            // Decline button
            Rectangle {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                radius: Theme.ThemeConfig.buttonRadius
                color: declineMouse.containsMouse ? "#E57373" : Theme.ThemeConfig.errorColor

                Text {
                    anchors.centerIn: parent
                    text: qsTr("Decline")
                    color: "white"
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                MouseArea {
                    id: declineMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        callInviteDialog.declined(callId)
                        callInviteDialog.close()
                    }
                }
            }

            // Accept button
            Rectangle {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                radius: Theme.ThemeConfig.buttonRadius
                color: acceptMouse.containsMouse ? "#66BB6A" : Theme.ThemeConfig.successColor

                Text {
                    anchors.centerIn: parent
                    text: qsTr("Accept")
                    color: "white"
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                MouseArea {
                    id: acceptMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        callInviteDialog.accepted(callId)
                        callInviteDialog.close()
                    }
                }
            }
        }
    }

    // ── Close on Escape ──────────────────────────────────────────
    Keys.onEscapePressed: {
        callInviteDialog.declined(callId)
        callInviteDialog.close()
    }

    // ── Convenience function ─────────────────────────────────────
    function show(caller, type, id) {
        callerName = caller
        callType = type
        callId = id
        callInviteDialog.open()
    }
}
