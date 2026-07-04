import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: messageInput
    height: Math.min(
        Theme.chatInputMaxHeight,
        Math.max(Theme.chatInputMinHeight, inputColumn.implicitHeight + 16)
    )
    color: Theme.surfaceColor
    border.color: Theme.dividerColor
    border.width: 1

    signal sendMessage(string text)
    signal attachFile()

    RowLayout {
        id: inputColumn
        anchors {
            fill: parent
            leftMargin: Theme.spacingMedium
            rightMargin: Theme.spacingSmall
            topMargin: Theme.spacingSmall
            bottomMargin: Theme.spacingSmall
        }
        spacing: Theme.spacingSmall

        // Attach button
        Rectangle {
            Layout.preferredWidth: 36
            Layout.preferredHeight: 36
            radius: 18
            color: attachMouse.containsMouse
                   ? Theme.sidebarHover : "transparent"

            Text {
                anchors.centerIn: parent
                text: "📎"
                font.pixelSize: Theme.fontSizeSubheading
            }

            MouseArea {
                id: attachMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: messageInput.attachFile()
            }
        }

        // Text input area
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            TextArea {
                id: textArea
                placeholderText: activeFocus ? "" : qsTr("Type a message...")
                placeholderTextColor: Theme.textHint
                font.pixelSize: Theme.fontSizeBody
                color: Theme.textPrimary
                wrapMode: TextArea.Wrap
                selectByMouse: true
                persistentSelection: true

                background: Rectangle {
                    color: "transparent"
                }

                // Enter sends, Shift+Enter newline
                Keys.onPressed: function(event) {
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        if (!(event.modifiers & Qt.ShiftModifier)) {
                            event.accepted = true
                            sendCurrentText()
                        }
                    }
                }
            }
        }

        // Send button
        Rectangle {
            Layout.preferredWidth: 36
            Layout.preferredHeight: 36
            radius: 18
            color: textArea.text.trim().length > 0
                   ? Theme.primaryColor
                   : Theme.dividerColor

            Text {
                anchors.centerIn: parent
                text: "↑"
                color: textArea.text.trim().length > 0
                       ? Theme.textOnPrimary
                       : Theme.textSecondary
                font.pixelSize: Theme.fontSizeSubheading
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                enabled: textArea.text.trim().length > 0
                onClicked: sendCurrentText()
            }
        }
    }

    function sendCurrentText() {
        var txt = textArea.text.trim()
        if (txt.length > 0) {
            messageInput.sendMessage(txt)
            textArea.clear()
        }
    }

    function focusInput() {
        textArea.forceActiveFocus()
    }
}
