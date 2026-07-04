import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: contactCard
    height: 68
    color: mouseArea.containsMouse ? Theme.sidebarHover : "transparent"

    property string aiUserId: ""
    property string aiUserName: ""
    property string aiUserDescription: ""
    property string aiUserAvatar: ""
    property string llmProvider: ""
    property string llmModel: ""

    signal clicked()
    signal chatClicked()

    // Full-card click → chat
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: contactCard.chatClicked()
    }

    RowLayout {
        anchors {
            fill: parent
            leftMargin: Theme.spacingLarge
            rightMargin: Theme.spacingMedium
        }
        spacing: Theme.spacingMedium

        // Avatar
        UserAvatar {
            Layout.preferredWidth: Theme.avatarSizeMedium
            Layout.preferredHeight: Theme.avatarSizeMedium
            name: contactCard.aiUserName
            imageSource: contactCard.aiUserAvatar
        }

        // Info
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                text: contactCard.aiUserName || qsTr("Unknown")
                color: Theme.textPrimary
                font.pixelSize: Theme.fontSizeBody
                font.weight: Theme.fontWeightMedium
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Text {
                text: contactCard.aiUserDescription || ""
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSizeSmall
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
                color: Theme.textHint
                font.pixelSize: Theme.fontSizeCaption
                visible: text.length > 0
            }
        }

    }

    // Bottom divider
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: Theme.spacingLarge
        }
        height: 1
        color: Theme.dividerColor
    }
}
