import QtQuick 2.15
import QtQuick.Controls 2.15
import "../theme/ThemeConfig.qml" as Theme

Rectangle {
    id: statusBar
    height: Theme.ThemeConfig.statusBarHeight
    color: Theme.ThemeConfig.primaryDark

    property bool connected: false
    property int totalUnread: 0

    Row {
        anchors {
            left: parent.left
            leftMargin: Theme.ThemeConfig.spacingSmall
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.ThemeConfig.spacingMedium

        // Connection indicator
        Row {
            spacing: Theme.ThemeConfig.spacingTiny
            anchors.verticalCenter: parent.verticalCenter

            Rectangle {
                width: 8
                height: 8
                radius: 4
                anchors.verticalCenter: parent.verticalCenter
                color: statusBar.connected
                       ? Theme.ThemeConfig.onlineColor
                       : Theme.ThemeConfig.offlineColor

                SequentialAnimation on opacity {
                    running: !statusBar.connected
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.3; duration: 800 }
                    NumberAnimation { from: 0.3; to: 1.0; duration: 800 }
                }
            }

            Text {
                text: statusBar.connected ? qsTr("Connected") : qsTr("Disconnected")
                color: Theme.ThemeConfig.textOnPrimary
                font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Row {
        anchors {
            right: parent.right
            rightMargin: Theme.ThemeConfig.spacingSmall
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.ThemeConfig.spacingMedium

        // Unread badge
        Rectangle {
            visible: totalUnread > 0
            anchors.verticalCenter: parent.verticalCenter
            width: unreadText.implicitWidth + 8
            height: 16
            radius: 8
            color: Theme.ThemeConfig.unreadBadgeColor

            Text {
                id: unreadText
                anchors.centerIn: parent
                text: totalUnread > 99 ? "99+" : String(totalUnread)
                color: Theme.ThemeConfig.unreadBadgeText
                font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                font.weight: Theme.ThemeConfig.fontWeightBold
            }
        }
    }

    Connections {
        target: typeof unreadTracker !== "undefined" ? unreadTracker : null
        enabled: typeof unreadTracker !== "undefined"
        function onTotalUnreadChanged(total) {
            statusBar.totalUnread = total
        }
    }
}
