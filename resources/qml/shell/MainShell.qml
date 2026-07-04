import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../pages" as Pages
import "../components" as C
import "../dialogs" as Dialogs

Item {
    id: mainShell
    anchors.fill: parent

    property string currentAiUserId: ""
    property string currentAiUserName: ""
    property string currentConversationId: ""

    // ── Layout: tab-bar | sidebar list | content ────────────────
    RowLayout {
        anchors.fill: parent; spacing: 0

        // ═══ Vertical tab bar (far left) ══════════════════════════
        Rectangle {
            id: tabBar
            Layout.preferredWidth: 56; Layout.fillHeight: true
            color: Theme.sidebarBackground
            border.color: Theme.dividerColor; border.width: 1

            ColumnLayout {
                anchors.fill: parent; spacing: 0

                C.TabButton { iconName: "message-circle"; label: qsTr("Chats")
                    selected: sidebar.currentTab === 0
                    onClicked: sidebar.currentTab = 0 }
                C.TabButton { iconName: "users"; label: qsTr("Contacts")
                    selected: sidebar.currentTab === 1
                    onClicked: sidebar.currentTab = 1 }
                C.TabButton { iconName: "puzzle"; label: qsTr("Ext")
                    selected: sidebar.currentTab === 2
                    onClicked: sidebar.currentTab = 2 }

                Item { Layout.fillHeight: true }

                C.TabButton { iconName: "settings"; label: ""
                    onClicked: sidebar.settingsClicked() }
            }
        }

        // ═══ Sidebar list area ═══════════════════════════════════
        Sidebar {
            id: sidebar
            Layout.preferredWidth: sidebar.width
            Layout.fillHeight: true
            visible: {
                var item = stackView.currentItem
                if (!item) return true
                var n = item.objectName || ""
                // Show list for chat/contact/expansion pages + welcome
                return n === "chatPage" || n === "contactDetail" || n === "" || n === "expansionPage"
            }

            onChatSelected: function(aiUserId, aiUserName) {
                mainShell.currentAiUserId = aiUserId
                mainShell.currentAiUserName = aiUserName
                if (typeof unreadTracker !== "undefined") unreadTracker.clearAndMarkRead(aiUserId)
                navigateToChat()
            }
            onContactSelected: function(aiUserId) {
                mainShell.currentAiUserId = aiUserId
                stackView.push(contactDetailPageComponent)
            }
            onSettingsClicked: { stackView.push(settingsPageComponent) }
            onCreateAIUserClicked: { stackView.push(createAIUserPageComponent) }

            // Tab switch resets content to welcome
            onCurrentTabChanged: { if (stackView.depth > 0) stackView.pop(null) }
        }

        // ═══ Draggable divider ═══════════════════════════════════
        Rectangle {
            Layout.preferredWidth: 4; Layout.fillHeight: true
            color: "transparent"
            MouseArea {
                anchors.fill: parent; cursorShape: Qt.SplitHCursor
                property real startX: 0
                onPressed: function(m) { startX = m.x }
                onPositionChanged: function(m) {
                    var newW = sidebar.width + (m.x - startX)
                    if (newW >= 200 && newW <= 500) sidebar.width = newW
                }
            }
        }

        // ═══ Content StackView ═══════════════════════════════════
        StackView {
            id: stackView
            Layout.fillWidth: true; Layout.fillHeight: true
            clip: true
            initialItem: welcomePlaceholder
        }
    }

    // ── Component cache ──────────────────────────────────────────
    Component {
        id: welcomePlaceholder
        Rectangle {
            color: Theme.backgroundColor
            Column {
                anchors.centerIn: parent; spacing: Theme.spacingMedium
                C.Icon { name: "message-circle"; size: 64; anchors.horizontalCenter: parent.horizontalCenter }
                Text { text: qsTr("Select a contact to start chatting"); color: Theme.textHint; font.pixelSize: Theme.fontSizeBody; anchors.horizontalCenter: parent.horizontalCenter }
            }
        }
    }

    Component { id: chatPageComponent; Pages.ChatPage {
        aiUserId: mainShell.currentAiUserId; aiUserName: mainShell.currentAiUserName
    }}
    Component { id: contactDetailPageComponent; Pages.ContactDetailPage {
        aiUserId: mainShell.currentAiUserId; objectName: "contactDetail"
    }}
    Component { id: createAIUserPageComponent; Pages.CreateAIUserPage {} }
    Component { id: settingsPageComponent; Pages.SettingsPage {} }

    function navigateToChat() {
        if (stackView.currentItem && stackView.currentItem.objectName === "chatPage")
            stackView.replace(chatPageComponent)
        else
            stackView.push(chatPageComponent)
    }

    Dialogs.ConfirmDialog { id: confirmDialog }
}
