import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Rectangle {
    id: sidebar
    width: 300
    color: Theme.sidebarBackground
    border.color: Theme.dividerColor; border.width: 1

    property int currentTab: 0 // 0=chats 1=contacts 2=extensions

    signal chatSelected(string aiUserId, string aiUserName)
    signal contactSelected(string aiUserId)
    signal settingsClicked()
    signal createAIUserClicked()

    ColumnLayout {
        anchors.fill: parent; spacing: 0

        // ── Header ───────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 48
            color: "transparent"
            RowLayout {
                anchors { fill: parent; leftMargin: 12; rightMargin: 8 }
                C.UserAvatar { Layout.preferredWidth: 32; Layout.preferredHeight: 32; name: "Me" }
                Item { Layout.fillWidth: true }
                Rectangle { width: 32; height: 32; radius: 16; color: parentMouse.containsMouse ? Theme.sidebarHover : "transparent"
                    C.Icon { anchors.centerIn: parent; name: currentTab == 1 ? "user-plus" : "plus"; size: 18 }
                    MouseArea { id: parentMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (sidebar.currentTab == 1) sidebar.createAIUserClicked()
                        }
                    }
                }
            }
        }

        // ── List area (tab-switched) ─────────────────────────────
        Item {
            Layout.fillWidth: true; Layout.fillHeight: true

            // Chats list
            ListView {
                id: chatList; anchors.fill: parent; visible: sidebar.currentTab === 0; clip: true; spacing: 1
                model: typeof conversationListModel !== "undefined" ? conversationListModel : null
                delegate: C.ChatListDelegate {
                    width: chatList.width
                    aiUserId: model.aiUserId || ""; name: model.aiUserName || model.title || ""
                    avatar: model.aiUserAvatar || ""; lastMessage: model.lastMessagePreview || ""
                    lastTime: formatTime(model.updatedAt || "")
                    unread: { if (typeof unreadTracker !== "undefined" && unreadTracker.unreadMap) return unreadTracker.unreadMap[model.aiUserId] || 0; return 0 }
                    onClicked: sidebar.chatSelected(model.aiUserId, model.aiUserName || model.title)
                }
            }

            // Contacts list
            ListView {
                id: contactList; anchors.fill: parent; visible: sidebar.currentTab === 1; clip: true; spacing: 1
                model: typeof contactListModel !== "undefined" ? contactListModel : null
                delegate: C.ContactCard {
                    width: contactList.width
                    aiUserId: model.id || ""; aiUserName: model.name || ""
                    aiUserDescription: model.description || ""; aiUserAvatar: model.avatar || ""
                    onClicked: sidebar.contactSelected(model.id)
                    onChatClicked: sidebar.chatSelected(model.id, model.name)
                }
            }

            // Extensions list (placeholder)
            ListView {
                id: extList; anchors.fill: parent; visible: sidebar.currentTab === 2; clip: true; spacing: 1
                model: 0
            }
        }
    }

    function formatTime(isoStr) {
        if (!isoStr || isoStr.length < 16) return ""
        return isoStr.substring(11, 16)
    }
}
