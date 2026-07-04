import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: conversationListPage
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ──────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            color: Theme.surfaceColor
            border.color: Theme.dividerColor
            border.width: 1

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.spacingLarge
                    rightMargin: Theme.spacingMedium
                }
                spacing: Theme.spacingMedium

                Text {
                    text: qsTr("Conversations")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeHeading
                    font.weight: Theme.fontWeightBold
                    Layout.fillWidth: true
                }
            }
        }

        // ── Conversation list ───────────────────────────────────
        ListView {
            id: convListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 0

            model: typeof conversationListModel !== "undefined" ? conversationListModel : null

            delegate: C.ConversationItem {
                width: convListView.width
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
                    // Navigate to chat using parent shell
                    navigateToChat(
                        model.aiUserId || "",
                        model.aiUserName || model.title || qsTr("Chat"),
                        model.id || ""
                    )
                }
            }

            // Empty state
            Rectangle {
                visible: convListView.count === 0
                anchors.fill: parent
                color: "transparent"

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: Theme.spacingLarge

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "💬"
                        font.pixelSize: 48
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("No conversations yet")
                        color: Theme.textHint
                        font.pixelSize: Theme.fontSizeBody
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Select a contact and start chatting!")
                        color: Theme.textHint
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }
            }
        }
    }

    function navigateToChat(aiUserId, aiUserName, conversationId) {
        // Look up the parent chain for MainShell or StackView
        var p = parent
        while (p) {
            if (p.hasOwnProperty("chatSelected")) {
                p.chatSelected(aiUserId, aiUserName, conversationId)
                return
            }
            if (p.hasOwnProperty("currentAiUserId")) {
                p.currentAiUserId = aiUserId
                p.currentAiUserName = aiUserName
                p.currentConversationId = conversationId
                return
            }
            p = p.parent
        }
    }
}
