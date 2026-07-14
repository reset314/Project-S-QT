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
                anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 8
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

            // Chats list — shows AI users (same as contacts, click → chat)
            ListView {
                id: chatList; anchors.fill: parent; visible: sidebar.currentTab === 0; clip: true; spacing: 1
                model: typeof contactListModel !== "undefined" ? contactListModel : null
                delegate: C.ChatListDelegate {
                    width: chatList.width
                    aiUserId: model.id || ""; name: model.name || ""
                    avatar: model.avatar || ""; lastMessage: model.description || ""
                    lastTime: ""
                    unread: { if (typeof unreadTracker !== "undefined" && unreadTracker.unreadMap) return unreadTracker.unreadMap[model.id] || 0; return 0 }
                    onClicked: sidebar.chatSelected(model.id, model.name)
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

            // Extensions list
            ListView {
                id: extList; anchors.fill: parent; visible: sidebar.currentTab === 2; clip: true; spacing: 1
                model: ListModel { id: extModel }
                delegate: Rectangle {
                    width: extList.width; height: extCard.implicitHeight + 16
                    color: Theme.surfaceColor
                    ColumnLayout {
                        id: extCard; anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; anchors.margins: 12; spacing: 4
                        RowLayout {
                            Text { text: model.name || qsTr("Unknown"); font.pixelSize: Theme.fontSizeBody; font.weight: Font.Bold; color: Theme.textPrimary; Layout.fillWidth: true }
                            Rectangle {
                                color: extCategoryColor(model.category)
                                radius: 4; width: catLabel.implicitWidth + 12; height: 20
                                Text { id: catLabel; anchors.centerIn: parent; text: model.category || "ai"; font.pixelSize: 10; color: "white" }
                            }
                        }
                        Text { text: model.description || ""; font.pixelSize: Theme.fontSizeSmall; color: Theme.textSecondary; Layout.fillWidth: true; wrapMode: Text.Wrap; maximumLineCount: 2; elide: Text.ElideRight }
                        Text { text: qsTr("Functions:") + " " + (Array.isArray(model.functions) ? model.functions.length : 0); font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
                    }
                }
                Component.onCompleted: loadExtModules()
            }
        }
    }

    function extCategoryColor(cat) {
        if (cat === "client") return Theme.primaryColor
        if (cat === "data") return Theme.successColor
        return Theme.warningColor
    }
    function loadExtModules() {
        if (typeof expansionRepo === "undefined") return
        var json = expansionRepo.listModulesJson()
        extModel.clear()
        if (!json || json.error) return
        var modules = json.modules || []
        for (var i = 0; i < modules.length; i++) extModel.append(modules[i])
    }

    function formatTime(isoStr) {
        if (!isoStr || isoStr.length < 16) return ""
        return isoStr.substring(11, 16)
    }
}
