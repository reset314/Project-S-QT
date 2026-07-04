import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

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
        radius: Theme.cardRadius
        color: Theme.surfaceColor

        layer.enabled: true
    }

    contentItem: ColumnLayout {
        spacing: Theme.spacingLarge

        // Message
        Text {
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacingLarge
            text: confirmDialog.message
            color: Theme.textPrimary
            font.pixelSize: Theme.fontSizeBody
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
        }

        Item { Layout.fillHeight: true }

        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: Theme.spacingLarge
            Layout.leftMargin: Theme.spacingLarge
            Layout.rightMargin: Theme.spacingLarge
            spacing: Theme.spacingMedium

            Item { Layout.fillWidth: true }

            // Cancel button
            Rectangle {
                Layout.preferredWidth: 100
                Layout.preferredHeight: 36
                radius: Theme.buttonRadius
                color: cancelMouseArea.containsMouse
                       ? Theme.sidebarHover : "transparent"
                border.color: Theme.dividerColor
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: confirmDialog.cancelText
                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSizeBody
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
                radius: Theme.buttonRadius
                color: confirmDialog.destructive
                       ? (confirmMouseArea.containsMouse ? "#E57373" : Theme.errorColor)
                       : (confirmMouseArea.containsMouse
                          ? Theme.primaryLight : Theme.primaryColor)

                Text {
                    anchors.centerIn: parent
                    text: confirmDialog.confirmText
                    color: "white"
                    font.pixelSize: Theme.fontSizeBody
                    font.weight: Theme.fontWeightMedium
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

    // Close on Escape (Dialog handles Esc natively via standardButtons or rejected)
    onRejected: {
        confirmDialog.cancelled()
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
