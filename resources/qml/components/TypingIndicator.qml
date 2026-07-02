import QtQuick 2.15
import QtQuick.Controls 2.15
import "../theme/ThemeConfig.qml" as Theme

Rectangle {
    id: typingIndicator
    width: typingRow.width + 24
    height: 36
    radius: Theme.ThemeConfig.bubbleRadius
    color: Theme.ThemeConfig.aiBubbleColor
    border.color: Theme.ThemeConfig.dividerColor
    border.width: 1

    property bool typing: false
    property color dotColor: Theme.ThemeConfig.textSecondary

    visible: typing

    Row {
        id: typingRow
        anchors.centerIn: parent
        spacing: 4

        Repeater {
            model: 3

            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: typingIndicator.dotColor
                opacity: 0.3

                SequentialAnimation on opacity {
                    running: typingIndicator.typing
                    loops: Animation.Infinite

                    PauseAnimation { duration: index * 150 }
                    NumberAnimation {
                        from: 0.3; to: 1.0
                        duration: 300
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        from: 1.0; to: 0.3
                        duration: 300
                        easing.type: Easing.InOutQuad
                    }
                    PauseAnimation { duration: (3 - index) * 150 + 300 }
                }
            }
        }
    }
}
