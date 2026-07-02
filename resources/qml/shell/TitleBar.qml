import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "../theme/ThemeConfig.qml" as Theme

Rectangle {
    id: titleBar
    height: Theme.ThemeConfig.titleBarHeight
    color: Theme.ThemeConfig.titleBarBackground
    z: 100

    signal minimizeClicked()
    signal maximizeClicked()
    signal closeClicked()

    // Drag to move window
    MouseArea {
        anchors.fill: parent
        property point lastMousePos: Qt.point(0, 0)
        onPressed: {
            lastMousePos = Qt.point(mouseX, mouseY)
        }
        onPositionChanged: {
            if (pressed) {
                var dx = mouseX - lastMousePos.x
                var dy = mouseY - lastMousePos.y
                var win = rootWindow()
                if (win) {
                    win.x += dx
                    win.y += dy
                }
            }
        }
    }

    Row {
        anchors {
            left: parent.left
            leftMargin: Theme.ThemeConfig.spacingSmall
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.ThemeConfig.spacingTiny

        Text {
            text: "AI Chat"
            color: Theme.ThemeConfig.textOnPrimary
            font.pixelSize: Theme.ThemeConfig.fontSizeSmall
            font.weight: Theme.ThemeConfig.fontWeightMedium
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        anchors {
            right: parent.right
            rightMargin: 0
            verticalCenter: parent.verticalCenter
        }

        TitleBarButton {
            text: "─"
            onClicked: titleBar.minimizeClicked()
        }

        TitleBarButton {
            text: "□"
            onClicked: titleBar.maximizeClicked()
        }

        TitleBarButton {
            text: "✕"
            isClose: true
            onClicked: titleBar.closeClicked()
        }
    }

    function rootWindow() {
        var obj = titleBar
        while (obj && !(obj instanceof Window)) {
            obj = obj.parent
        }
        return obj
    }

    component TitleBarButton: Rectangle {
        property string text: ""
        property bool isClose: false

        width: 46
        height: Theme.ThemeConfig.titleBarHeight
        color: mouseArea.containsMouse
               ? (isClose ? Theme.ThemeConfig.errorColor : Qt.rgba(1, 1, 1, 0.1))
               : "transparent"

        signal clicked()

        Text {
            anchors.centerIn: parent
            text: parent.text
            color: Theme.ThemeConfig.textOnPrimary
            font.pixelSize: Theme.ThemeConfig.fontSizeSmall
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: parent.clicked()
        }
    }
}
