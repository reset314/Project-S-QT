import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Rectangle {
    id: sidebar
    width: Theme.sidebarWidth
    color: Theme.sidebarBackground

    /// Emitted when the user selects an AI user to chat with.
    signal chatSelected(string aiUserId, string aiUserName)

    signal contactSelected(string aiUserId)
    signal settingsClicked()
    signal createAIUserClicked()

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── User Info Header ────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            color: Theme.primaryColor
            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.spacingLarge
                    rightMargin: Theme.spacingLarge
                }
                spacing: Theme.spacingMedium
                C.UserAvatar {
                    Layout.preferredWidth: Theme.avatarSizeMedium
                    Layout.preferredHeight: Theme.avatarSizeMedium
                    name: "User"
                }
                Text {
                    text: qsTr("Chats")
                    color: Theme.textOnPrimary
                    font.pixelSize: Theme.fontSizeHeading
                    font.weight: Theme.fontWeightBold
                    Layout.fillWidth: true
                }
                Rectangle {
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    radius: 16
                    color: settingsMouse.containsMouse ? Qt.rgba(1,1,1,0.2) : "transparent"
                    Text { anchors.centerIn: parent; text: "⚙"; color: Theme.textOnPrimary; font.pixelSize: Theme.fontSizeHeading }
                    MouseArea {
                        id: settingsMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: sidebar.settingsClicked()
                    }
                }
            }
        }

        // ── AI User list (all chats are per AI user) ────────────
        ListView {
            id: userListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 1
            // Show all contacts (AI users); could be filtered to only
            // those with recent messages in a "Chats" tab later.
            model: typeof contactListModel !== "undefined" ? contactListModel : null

            delegate: C.ContactCard {
                width: userListView.width
                aiUserId: model.id || ""
                aiUserName: model.name || ""
                aiUserDescription: model.description || ""
                aiUserAvatar: model.avatar || ""
                // llmProvider: model.llmProvider || ""
                // llmModel: model.llmModel || ""

                // Unread badge
                property int unreadCount: {
                    if (typeof unreadTracker !== "undefined" && unreadTracker.unreadMap) {
                        var map = unreadTracker.unreadMap
                        var key = model.id || ""
                        return map[key] ? map[key] : 0
                    }
                    return 0
                }

                onChatClicked: {
                    sidebar.chatSelected(model.id || "", model.name || "")
                }

                onClicked: {
                    sidebar.contactSelected(model.id || "")
                }
            }

            Rectangle {
                visible: userListView.count === 0
                anchors.fill: parent
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text: qsTr("No contacts yet")
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeBody
                }
            }
        }

        // ── Bottom Actions ──────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: Theme.sidebarBackground
            border.color: Theme.dividerColor
            border.width: 1
            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.spacingLarge
                    rightMargin: Theme.spacingLarge
                }
                Rectangle {
                    Layout.preferredHeight: 32
                    implicitWidth: newChatText.implicitWidth + 24
                    radius: Theme.buttonRadius
                    color: newChatMouse.containsMouse ? Theme.primaryLight : Theme.primaryColor
                    Text {
                        id: newChatText
                        anchors.centerIn: parent
                        text: qsTr("New AI User")
                        color: Theme.textOnPrimary
                        font.pixelSize: Theme.fontSizeSmall
                        font.weight: Theme.fontWeightMedium
                    }
                    MouseArea {
                        id: newChatMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: sidebar.createAIUserClicked()
                    }
                }
                Item { Layout.fillWidth: true }
            }
        }
    }
}
