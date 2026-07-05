import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: statusBar
    height: Theme.statusBarHeight
    color: Theme.primaryDark

    property bool connected: false
    property int totalUnread: 0

    Row {
        anchors {
            left: parent.left
            leftMargin: Theme.spacingSmall
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.spacingMedium

        // Connection indicator
        Row {
            spacing: Theme.spacingTiny
            anchors.verticalCenter: parent.verticalCenter

            Rectangle {
                width: 8
                height: 8
                radius: 4
                anchors.verticalCenter: parent.verticalCenter
                color: statusBar.connected
                       ? Theme.onlineColor
                       : Theme.offlineColor

                SequentialAnimation on opacity {
                    running: !statusBar.connected
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.3; duration: 800 }
                    NumberAnimation { from: 0.3; to: 1.0; duration: 800 }
                }
            }

            Text {
                text: statusBar.connected ? qsTr("Connected") : qsTr("Disconnected")
                color: Theme.textOnPrimary
                font.pixelSize: Theme.fontSizeCaption
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Row {
        anchors {
            right: parent.right
            rightMargin: Theme.spacingSmall
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.spacingMedium

        // Unread badge
        Rectangle {
            visible: totalUnread > 0
            anchors.verticalCenter: parent.verticalCenter
            width: unreadText.implicitWidth + 8
            height: 16
            radius: 8
            color: Theme.unreadBadgeColor

            Text {
                id: unreadText
                anchors.centerIn: parent
                text: totalUnread > 99 ? "99+" : String(totalUnread)
                color: Theme.unreadBadgeText
                font.pixelSize: Theme.fontSizeCaption
                font.weight: Theme.fontWeightBold
            }
        }
    }

    Component.onCompleted: {
        if (typeof appState !== "undefined")
            statusBar.connected = appState.connectionStatus === "connected"
    }

    Connections {
        target: typeof unreadTracker !== "undefined" ? unreadTracker : null
        enabled: typeof unreadTracker !== "undefined"
        function onTotalUnreadChanged(total) { statusBar.totalUnread = total }
    }
    Connections {
        target: typeof appState !== "undefined" ? appState : null
        enabled: typeof appState !== "undefined"
        function onConnectionStatusChanged() {
            statusBar.connected = appState.connectionStatus === "connected"
        }
    }
}
