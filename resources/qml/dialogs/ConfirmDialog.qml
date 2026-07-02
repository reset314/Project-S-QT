import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import "../theme/ThemeConfig.qml" as Theme

Dialog {
    id: confirmDialog
    width: 400
    height: 180
    modal: true

    property string message: ""
    property string confirmText: qsTr("Confirm")
    property string cancelText: qsTr("Cancel")
    property bool destructive: false

    signal confirmed()
    signal cancelled()

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

        // Message
        Text {
            Layout.fillWidth: true
            Layout.topMargin: Theme.ThemeConfig.spacingLarge
            text: confirmDialog.message
            color: Theme.ThemeConfig.textPrimary
            font.pixelSize: Theme.ThemeConfig.fontSizeBody
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
        }

        Item { Layout.fillHeight: true }

        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: Theme.ThemeConfig.spacingLarge
            Layout.leftMargin: Theme.ThemeConfig.spacingLarge
            Layout.rightMargin: Theme.ThemeConfig.spacingLarge
            spacing: Theme.ThemeConfig.spacingMedium

            Item { Layout.fillWidth: true }

            // Cancel button
            Rectangle {
                Layout.preferredWidth: 100
                Layout.preferredHeight: 36
                radius: Theme.ThemeConfig.buttonRadius
                color: cancelMouseArea.containsMouse
                       ? Theme.ThemeConfig.sidebarHover : "transparent"
                border.color: Theme.ThemeConfig.dividerColor
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: confirmDialog.cancelText
                    color: Theme.ThemeConfig.textSecondary
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                }

                MouseArea {
                    id: cancelMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        confirmDialog.cancelled()
                        confirmDialog.close()
                    }
                }
            }

            // Confirm button
            Rectangle {
                Layout.preferredWidth: 100
                Layout.preferredHeight: 36
                radius: Theme.ThemeConfig.buttonRadius
                color: confirmDialog.destructive
                       ? (confirmMouseArea.containsMouse ? "#E57373" : Theme.ThemeConfig.errorColor)
                       : (confirmMouseArea.containsMouse
                          ? Theme.ThemeConfig.primaryLight : Theme.ThemeConfig.primaryColor)

                Text {
                    anchors.centerIn: parent
                    text: confirmDialog.confirmText
                    color: "white"
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                MouseArea {
                    id: confirmMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        confirmDialog.confirmed()
                        confirmDialog.close()
                    }
                }
            }
        }
    }

    // Close on Escape
    Keys.onEscapePressed: {
        confirmDialog.cancelled()
        confirmDialog.close()
    }

    // ── Static convenience functions ─────────────────────────────
    function show(msg, onConfirm, onCancel, options) {
        message = msg
        if (options) {
            confirmText = options.confirmText || qsTr("Confirm")
            cancelText = options.cancelText || qsTr("Cancel")
            destructive = options.destructive || false
        } else {
            confirmText = qsTr("Confirm")
            cancelText = qsTr("Cancel")
            destructive = false
        }
        confirmed.disconnectAll()
        cancelled.disconnectAll()
        if (onConfirm) confirmed.connect(onConfirm)
        if (onCancel) cancelled.connect(onCancel)
        confirmDialog.open()
    }

    function confirm(msg, callback) {
        show(msg, callback, null, {})
    }

    function alert(msg, callback) {
        show(msg, callback, null, { confirmText: qsTr("OK"), cancelText: "" })
    }

    // Internal helper for disconnectAll
    function disconnectAll(sig) {
        // QML connections are cleaned up on close
    }
}
