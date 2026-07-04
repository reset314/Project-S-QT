import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: messageInput
    height: Math.min(Theme.chatInputMaxHeight, Math.max(Theme.chatInputMinHeight, inputColumn.implicitHeight + 12))
    color: Theme.surfaceColor
    border.color: Theme.dividerColor; border.width: 1

    signal sendMessage(string text)
    signal attachFile()

    RowLayout {
        id: inputColumn
        anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 4; anchors.topMargin: 6; anchors.bottomMargin: 6
        spacing: 4

        // [📎] Upload
        Rectangle { width: 36; height: 36; radius: 18
            color: attachMouse.containsMouse ? Theme.sidebarHover : "transparent"
            Icon { anchors.centerIn: parent; name: "paperclip"; size: 20 }
            MouseArea { id: attachMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: messageInput.attachFile() }
        }

        // [🎤] Voice toggle
        Rectangle { width: 36; height: 36; radius: 18
            color: voiceMouse.containsMouse ? Theme.sidebarHover : "transparent"
            Icon { anchors.centerIn: parent; name: "mic"; size: 20 }
            MouseArea { id: voiceMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor }
        }

        // Input field
        ScrollView {
            Layout.fillWidth: true; Layout.fillHeight: true; clip: true
            TextArea {
                id: textArea
                placeholderText: activeFocus ? "" : qsTr("Type a message...")
                placeholderTextColor: Theme.textHint
                font.pixelSize: Theme.fontSizeBody; color: Theme.textPrimary
                wrapMode: TextArea.Wrap; selectByMouse: true;
                background: Rectangle { color: "transparent" }
                Keys.onPressed: function(event) {
                    if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter) && !(event.modifiers & Qt.ShiftModifier)) {
                        event.accepted = true; sendCurrentText()
                    }
                }
            }
        }

        // [↑] Send
        Rectangle { width: 36; height: 36; radius: 18
            color: textArea.text.trim().length > 0 ? Theme.primaryColor : Theme.dividerColor
            Icon { anchors.centerIn: parent; name: "arrow-up"; size: 20
                color: textArea.text.trim().length > 0 ? Theme.textOnPrimary : Theme.textSecondary }
            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                enabled: textArea.text.trim().length > 0; onClicked: sendCurrentText() }
        }

        // [···] More menu
        Rectangle { width: 36; height: 36; radius: 18
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
            // Extension-registered input_bar_menu items go here
        }
    }

    component MenuButton: Rectangle {
        property string iconName: ""; property string text: ""
        signal clicked()
        width: 140; height: 36; radius: 4
        color: mbMouse.containsMouse ? Theme.sidebarHover : "transparent"
        RowLayout { anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 8
            Icon { name: iconName; size: 18 }
            Text { text: parent.text; font.pixelSize: Theme.fontSizeSmall; color: Theme.textPrimary }
        }
        MouseArea { id: mbMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: parent.clicked() }
    }

    function sendCurrentText() {
        var txt = textArea.text.trim()
        if (txt.length > 0) { messageInput.sendMessage(txt); textArea.clear() }
    }
    function startCall() { if (typeof callManager !== "undefined") callManager.startCall("") }
}
