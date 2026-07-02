import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Rectangle {
    id: messageInput
    height: Math.min(
        Theme.ThemeConfig.chatInputMaxHeight,
        Math.max(Theme.ThemeConfig.chatInputMinHeight, inputColumn.implicitHeight + 16)
    )
    color: Theme.ThemeConfig.surfaceColor
    border.color: Theme.ThemeConfig.dividerColor
    border.width: 1

    signal sendMessage(string text)
    signal attachFile()

    RowLayout {
        anchors {
            fill: parent
            leftMargin: Theme.ThemeConfig.spacingMedium
            rightMargin: Theme.ThemeConfig.spacingSmall
            topMargin: Theme.ThemeConfig.spacingSmall
            bottomMargin: Theme.ThemeConfig.spacingSmall
        }
        spacing: Theme.ThemeConfig.spacingSmall

        // Attach button
        Rectangle {
            Layout.preferredWidth: 36
            Layout.preferredHeight: 36
            radius: 18
            color: attachMouse.containsMouse
                   ? Theme.ThemeConfig.sidebarHover : "transparent"

            Text {
                anchors.centerIn: parent
                text: "📎"
                font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
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
                placeholderText: qsTr("Type a message...")
                placeholderTextColor: Theme.ThemeConfig.textHint
                font.pixelSize: Theme.ThemeConfig.fontSizeBody
                color: Theme.ThemeConfig.textPrimary
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
                   ? Theme.ThemeConfig.primaryColor
                   : Theme.ThemeConfig.dividerColor

            Text {
                anchors.centerIn: parent
                text: "↑"
                color: textArea.text.trim().length > 0
                       ? Theme.ThemeConfig.textOnPrimary
                       : Theme.ThemeConfig.textSecondary
                font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
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
