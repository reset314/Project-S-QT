import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme
import "../components" as C

Rectangle {
    id: sidebar
    width: Theme.ThemeConfig.sidebarWidth
    color: Theme.ThemeConfig.sidebarBackground

    signal chatSelected(string aiUserId, string aiUserName, string conversationId)
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
            color: Theme.ThemeConfig.primaryColor

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.ThemeConfig.spacingLarge
                    rightMargin: Theme.ThemeConfig.spacingLarge
                }
                spacing: Theme.ThemeConfig.spacingMedium

                C.UserAvatar {
                    Layout.preferredWidth: Theme.ThemeConfig.avatarSizeMedium
                    Layout.preferredHeight: Theme.ThemeConfig.avatarSizeMedium
                    name: "User"
                }

                Text {
                    text: qsTr("Chats")
                    color: Theme.ThemeConfig.textOnPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }

                // Settings gear icon
                Rectangle {
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    radius: 16
                    color: settingsMouse.containsMouse
                           ? Qt.rgba(1, 1, 1, 0.2) : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "⚙"
                        color: Theme.ThemeConfig.textOnPrimary
                        font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    }

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

        // ── Tab Bar ─────────────────────────────────────────────
        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                text: qsTr("Chats")
                width: implicitWidth
            }
            TabButton {
                text: qsTr("Contacts")
                width: implicitWidth
            }
        }

        // ── Content Area (StackLayout of two ListViews) ────────
        StackLayout {
            id: contentStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // Chats tab
            ListView {
                id: conversationListView
                model: typeof conversationListModel !== "undefined" ? conversationListModel : null
                clip: true
                spacing: 1

                delegate: C.ConversationItem {
                    width: conversationListView.width
                    aiUserName: model.aiUserName || model.title || qsTr("Chat")
                    aiUserAvatar: model.aiUserAvatar || ""
                    lastMessagePreview: model.lastMessagePreview || ""
                    timestamp: model.updatedAt || ""
                    unreadCount: {
                        if (typeof unreadTracker !== "undefined" && unreadTracker.unreadMap) {
                            var map = unreadTracker.unreadMap
                            var key = model.aiUserId || ""
                            return map[key] ? map[key] : 0
                        }
                        return 0
                    }

                    onClicked: {
                        sidebar.chatSelected(
                            model.aiUserId || "",
                            model.aiUserName || model.title || qsTr("Chat"),
                            model.id || ""
                        )
                    }
                }

                // Empty state
                Rectangle {
                    visible: conversationListView.count === 0
                    anchors.fill: parent
                    color: "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("No conversations yet")
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    }
                }
            }

            // Contacts tab
            ListView {
                id: contactListView
                model: typeof contactListModel !== "undefined" ? contactListModel : null
                clip: true
                spacing: 1

                delegate: C.ContactCard {
                    width: contactListView.width
                    aiUserId: model.id || ""
                    aiUserName: model.name || ""
                    aiUserDescription: model.description || ""
                    aiUserAvatar: model.avatar || ""
                    llmProvider: model.llmProvider || ""
                    llmModel: model.llmModel || ""

                    onClicked: {
                        sidebar.contactSelected(model.id || "")
                    }

                    onChatClicked: {
                        sidebar.chatSelected(
                            model.id || "",
                            model.name || "",
                            ""
                        )
                    }
                }

                // Empty state
                Rectangle {
                    visible: contactListView.count === 0
                    anchors.fill: parent
                    color: "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("No contacts yet")
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    }
                }
            }
        }

        // ── Bottom Actions ──────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: Theme.ThemeConfig.sidebarBackground
            border.color: Theme.ThemeConfig.dividerColor
            border.width: 1

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.ThemeConfig.spacingLarge
                    rightMargin: Theme.ThemeConfig.spacingLarge
                }

                // New Chat button
                Rectangle {
                    Layout.preferredWidth: Layout.fillWidth ? undefined : implicitWidth
                    implicitWidth: newChatText.implicitWidth + 24
                    Layout.preferredHeight: 32
                    radius: Theme.ThemeConfig.buttonRadius
                    color: newChatMouse.containsMouse
                           ? Theme.ThemeConfig.primaryLight : Theme.ThemeConfig.primaryColor

                    Text {
                        id: newChatText
                        anchors.centerIn: parent
                        text: qsTr("New AI User")
                        color: Theme.ThemeConfig.textOnPrimary
                        font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                        font.weight: Theme.ThemeConfig.fontWeightMedium
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
