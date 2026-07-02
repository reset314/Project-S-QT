import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Rectangle {
    id: contactCard
    height: 68
    color: mouseArea.containsMouse ? Theme.ThemeConfig.sidebarHover : "transparent"

    property string aiUserId: ""
    property string aiUserName: ""
    property string aiUserDescription: ""
    property string aiUserAvatar: ""
    property string llmProvider: ""
    property string llmModel: ""

    signal clicked()
    signal chatClicked()

    RowLayout {
        anchors {
            fill: parent
            leftMargin: Theme.ThemeConfig.spacingLarge
            rightMargin: Theme.ThemeConfig.spacingMedium
        }
        spacing: Theme.ThemeConfig.spacingMedium

        // Avatar
        UserAvatar {
            Layout.preferredWidth: Theme.ThemeConfig.avatarSizeMedium
            Layout.preferredHeight: Theme.ThemeConfig.avatarSizeMedium
            name: contactCard.aiUserName
            imageSource: contactCard.aiUserAvatar
        }

        // Info
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                text: contactCard.aiUserName || qsTr("Unknown")
                color: Theme.ThemeConfig.textPrimary
                font.pixelSize: Theme.ThemeConfig.fontSizeBody
                font.weight: Theme.ThemeConfig.fontWeightMedium
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Text {
                text: contactCard.aiUserDescription || ""
                color: Theme.ThemeConfig.textSecondary
                font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                Layout.fillWidth: true
                elide: Text.ElideRight
                visible: text.length > 0
            }

            Text {
                text: {
                    var parts = []
                    if (contactCard.llmProvider) parts.push(contactCard.llmProvider)
                    if (contactCard.llmModel) parts.push(contactCard.llmModel)
                    return parts.join(" / ")
                }
                color: Theme.ThemeConfig.textHint
                font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                visible: text.length > 0
            }
        }

        // Chat button
        Rectangle {
            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
            radius: 16
            color: chatMouse.containsMouse
                   ? Theme.ThemeConfig.primaryLight : Theme.ThemeConfig.primaryColor

            Text {
                anchors.centerIn: parent
                text: "💬"
                font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
            }

            MouseArea {
                id: chatMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: contactCard.chatClicked()
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: contactCard.clicked()
    }

    // Bottom divider
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: Theme.ThemeConfig.spacingLarge
        }
        height: 1
        color: Theme.ThemeConfig.dividerColor
    }
}
