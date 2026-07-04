import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../pages" as Pages
import "../dialogs" as Dialogs

Item {
    id: mainShell
    anchors.fill: parent

    property string currentAiUserId: ""
    property string currentAiUserName: ""
    property string currentConversationId: ""
    property bool detailPanelOpen: false

    // ── Layout ─────────────────────────────────────────────────
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Sidebar
        Sidebar {
            id: sidebar
            Layout.fillHeight: true
            Layout.preferredWidth: Theme.sidebarWidth

            onChatSelected: function(aiUserId, aiUserName) {
                mainShell.currentAiUserId = aiUserId
                mainShell.currentAiUserName = aiUserName
                if (typeof unreadTracker !== "undefined") {
                    unreadTracker.clearAndMarkRead(aiUserId)
                }
                // Guard against concurrent StackView transitions
                if (stackView.busy) {
                    // Defer: wait for current transition to finish
                    chatSelectedTimer.aiUserId = aiUserId
                    chatSelectedTimer.aiUserName = aiUserName
                    chatSelectedTimer.restart()
                    return
                }
                navigateToChat()
            }

            onContactSelected: function(aiUserId) {
                mainShell.currentAiUserId = aiUserId
                stackView.push(contactDetailPageComponent)
            }

            onSettingsClicked: {
                stackView.push(settingsPageComponent)
            }

            onCreateAIUserClicked: {
                stackView.push(createAIUserPageComponent)
            }
        }

        // Vertical divider
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 1
            color: Theme.dividerColor
        }

        // ── Center Content (StackView) ─────────────────────────
        StackView {
            id: stackView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            // Empty placeholder until user selects a contact
            initialItem: welcomePlaceholder

            // No transitions — instant page switch
        }

        // ── Detail Panel (right, collapsible) ───────────────────
        Rectangle {
            id: detailPanel
            visible: mainShell.detailPanelOpen
            Layout.fillHeight: true
            Layout.preferredWidth: visible ? Theme.detailPanelWidth : 0
            color: Theme.surfaceColor
            border.color: Theme.dividerColor
            border.width: 1

            Behavior on Layout.preferredWidth {
                NumberAnimation { duration: Theme.animationNormal }
            }

            // Detail content placeholder
            Text {
                anchors.centerIn: parent
                text: qsTr("Details")
                color: Theme.textHint
                font.pixelSize: Theme.fontSizeBody
            }
        }
    }

    // ── Component cache ────────────────────────────────────────
    // ── Welcome / empty state placeholder ───────────────────────
    Component {
        id: welcomePlaceholder
        Rectangle {
            color: Theme.backgroundColor
            implicitWidth: 400
            implicitHeight: 400
            Column {
                anchors.centerIn: parent
                spacing: Theme.spacingMedium
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "💬"
                    font.pixelSize: 64
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Select a contact to start chatting")
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeBody
                }
            }
        }
    }

    Component { id: chatPageComponent; Pages.ChatPage {
        aiUserId: mainShell.currentAiUserId
        aiUserName: mainShell.currentAiUserName
    }}
    Component { id: contactDetailPageComponent; Pages.ContactDetailPage {
        aiUserId: mainShell.currentAiUserId
    }}
    Component { id: createAIUserPageComponent; Pages.CreateAIUserPage {} }
    Component { id: settingsPageComponent; Pages.SettingsPage {} }
    Component { id: loginPageComponent; Pages.LoginPage {} }

    // ── Deferred chat navigation (avoids StackView busy errors) ──
    Timer {
        id: chatSelectedTimer
        interval: 50
        repeat: false
        property string aiUserId: ""
        property string aiUserName: ""
        onTriggered: navigateToChat()
    }

    function navigateToChat() {
        if (stackView.currentItem && stackView.currentItem.objectName === "chatPage") {
            stackView.replace(chatPageComponent)
        } else {
            stackView.push(chatPageComponent)
        }
    }

    // ── Confirm dialog ─────────────────────────────────────────
    Dialogs.ConfirmDialog {
        id: confirmDialog
    }
}
