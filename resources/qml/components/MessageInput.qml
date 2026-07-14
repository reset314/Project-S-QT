import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: messageInput
    color: Theme.surfaceColor
    border.color: Theme.dividerColor; border.width: 1

    signal sendMessage(string text)
    signal attachFile()

    // implicitHeight drives ColumnLayout in ChatPage — grows upward (pushes ListView)
    implicitHeight: Math.min(Theme.chatInputMaxHeight,
                             Math.max(Theme.chatInputMinHeight,
                                      textArea.implicitHeight + 24))

    RowLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 8
        anchors.rightMargin: 4
        spacing: 4

        // [📎] Upload
        Rectangle {
            Layout.preferredWidth: 36; Layout.preferredHeight: 36; radius: 18
            color: attachMouse.containsMouse ? Theme.sidebarHover : "transparent"
            Icon { anchors.centerIn: parent; name: "paperclip"; size: 20 }
            MouseArea { id: attachMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: messageInput.attachFile() }
        }

        // [🎤] Voice toggle
        Rectangle {
            Layout.preferredWidth: 36; Layout.preferredHeight: 36; radius: 18
            color: voiceMouse.containsMouse ? Theme.sidebarHover : "transparent"
            Icon { anchors.centerIn: parent; name: "mic"; size: 20 }
            MouseArea { id: voiceMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor }
        }

        // Input field — height driven by content, vertical-align center for single line
        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.preferredHeight: textArea.implicitHeight
            Layout.maximumHeight: Theme.chatInputMaxHeight - 24
            clip: true
            padding: 0
            background: Rectangle { color: "transparent" }
            contentWidth: availableWidth

            TextArea {
                id: textArea
                width: scrollView.availableWidth
                height: implicitHeight
                font.pixelSize: Theme.fontSizeBody
                color: Theme.textPrimary
                wrapMode: TextArea.Wrap
                selectByMouse: true
                verticalAlignment: TextEdit.AlignVCenter
                topPadding: 6
                bottomPadding: 6
                leftPadding: 6
                rightPadding: 6
                background: Rectangle { color: "transparent" }

                Text {
                    id: placeholder
                    anchors.fill: parent
                    anchors.margins: textArea.topPadding
                    text: qsTr("Type a message...")
                    color: Theme.textHint
                    font: textArea.font
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    visible: !textArea.text && !textArea.activeFocus
                    enabled: false
                }

                Keys.onPressed: function(event) {
                    if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter) && !(event.modifiers & Qt.ShiftModifier)) {
                        event.accepted = true
                        sendCurrentText()
                    }
                }
            }
        }

        // [↑] Send
        Rectangle {
            Layout.preferredWidth: 36; Layout.preferredHeight: 36; radius: 18
            color: textArea.text.trim().length > 0 ? Theme.primaryColor : Theme.dividerColor
            Icon { anchors.centerIn: parent; name: "arrow-up"; size: 20
                color: textArea.text.trim().length > 0 ? Theme.textOnPrimary : Theme.textSecondary }
            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                enabled: textArea.text.trim().length > 0; onClicked: sendCurrentText() }
        }

        // [···] More menu
        Rectangle {
            Layout.preferredWidth: 36; Layout.preferredHeight: 36; radius: 18
            color: moreMouse.containsMouse ? Theme.sidebarHover : "transparent"
            Icon { anchors.centerIn: parent; name: "more-horizontal"; size: 20 }
            MouseArea { id: moreMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: moreMenu.open() }
        }
    }

    // ── More menu popup ──────────────────────────────────────────
    Popup {
        id: moreMenu
        y: -height; x: parent.width - width
        padding: 4
        background: Rectangle { radius: 8; color: Theme.surfaceColor; border.color: Theme.dividerColor; border.width: 1 }

        ColumnLayout { spacing: 0
            MenuButton { iconName: "phone"; text: qsTr("Voice Call"); onClicked: { moreMenu.close(); startCall() } }
        }
    }

    component MenuButton: Rectangle {
        id: menuBtn
        property string iconName: ""; property string text: ""
        signal clicked()
        width: 140; height: 36; radius: 4
        color: mbMouse.containsMouse ? Theme.sidebarHover : "transparent"
        RowLayout { anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 8
            Icon { name: menuBtn.iconName; size: 18 }
            Text { text: menuBtn.text; font.pixelSize: Theme.fontSizeSmall; color: Theme.textPrimary }
        }
        MouseArea { id: mbMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: menuBtn.clicked() }
    }

    function sendCurrentText() {
        var txt = textArea.text.trim()
        if (txt.length > 0) { messageInput.sendMessage(txt); textArea.clear() }
    }
    function startCall() { if (typeof callManager !== "undefined") callManager.startCall("") }
}
