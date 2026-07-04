import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: conversationItem
    height: 68
    color: mouseArea.containsMouse ? Theme.sidebarHover : "transparent"

    property string aiUserId: ""
    property string aiUserName: ""
    property string aiUserAvatar: ""
    property string lastMessagePreview: ""
    property string timestamp: ""
    property int unreadCount: 0

    signal clicked()

    RowLayout {
        anchors {
            fill: parent
            leftMargin: Theme.spacingLarge
            rightMargin: Theme.spacingMedium
        }
        spacing: Theme.spacingMedium

        // Avatar with unread badge
        Item {
            Layout.preferredWidth: Theme.avatarSizeMedium
            Layout.preferredHeight: Theme.avatarSizeMedium

            UserAvatar {
                id: avatar
                name: conversationItem.aiUserName
                imageSource: conversationItem.aiUserAvatar
                size: Theme.avatarSizeMedium
            }

            // Unread badge on avatar
            Rectangle {
                visible: conversationItem.unreadCount > 0
                anchors {
                    top: parent.top
                    right: parent.right
                    topMargin: -2
                    rightMargin: -4
                }
                width: Math.max(18, unreadBadgeText.implicitWidth + 6)
                height: 18
                radius: 9
                color: Theme.unreadBadgeColor
                z: 2

                Text {
                    id: unreadBadgeText
                    anchors.centerIn: parent
                    text: conversationItem.unreadCount > 99
                          ? "99+" : String(conversationItem.unreadCount)
                    color: Theme.unreadBadgeText
                    font.pixelSize: Theme.fontSizeCaption
                    font.weight: Theme.fontWeightBold
                }
            }
        }

        // Info
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            // Name + timestamp row
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingSmall

                Text {
                    text: conversationItem.aiUserName || qsTr("Chat")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeBody
                    font.weight: conversationItem.unreadCount > 0
                                 ? Theme.fontWeightBold
                                 : Theme.fontWeightNormal
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                Text {
                    text: formatTimestamp(conversationItem.timestamp)
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeCaption
                }
            }

            // Last message preview
            Text {
                text: conversationItem.lastMessagePreview || ""
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSizeSmall
                Layout.fillWidth: true
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: conversationItem.clicked()
    }

    // Bottom divider
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: 72  // Align with text
        }
        height: 1
        color: Theme.dividerColor
    }

    // ── Helpers ──────────────────────────────────────────────────
    function formatTimestamp(isoStr) {
        if (!isoStr || isoStr.length < 16) return ""
        var now = new Date()
        var dt = new Date(isoStr)
        if (isNaN(dt.getTime())) {
            // Try extracting time portion
            var t = isoStr.substring(11, 16)
            return t
        }

        var diffMs = now.getTime() - dt.getTime()
        var diffMins = Math.floor(diffMs / 60000)

        if (diffMins < 1) return qsTr("Just now")
        if (diffMins < 60) return diffMins + "m"

        var diffHours = Math.floor(diffMins / 60)
        if (diffHours < 24) return diffHours + "h"

        var diffDays = Math.floor(diffHours / 24)
        if (diffDays < 7) return diffDays + "d"

        // Show date MM/DD
        return (dt.getMonth() + 1) + "/" + dt.getDate()
    }
}
