import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme
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
            Layout.preferredWidth: Theme.ThemeConfig.sidebarWidth

            onChatSelected: function(aiUserId, aiUserName, conversationId) {
                mainShell.currentAiUserId = aiUserId
                mainShell.currentAiUserName = aiUserName
                mainShell.currentConversationId = conversationId
                stackView.push(chatPageComponent)
                if (typeof unreadTracker !== "undefined") {
                    unreadTracker.clearAndMarkRead(aiUserId)
                }
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
            color: Theme.ThemeConfig.dividerColor
        }

        // ── Center Content (StackView) ─────────────────────────
        StackView {
            id: stackView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            initialItem: conversationListPageComponent

            // Pop transition
            popEnter: Transition {
                XAnimator { from: -stackView.width * 0.3; to: 0; duration: Theme.ThemeConfig.animationNormal }
            }
            popExit: Transition {
                XAnimator { from: 0; to: stackView.width; duration: Theme.ThemeConfig.animationNormal }
            }
            pushEnter: Transition {
                XAnimator { from: stackView.width; to: 0; duration: Theme.ThemeConfig.animationNormal }
            }
            pushExit: Transition {
                XAnimator { from: 0; to: -stackView.width * 0.3; duration: Theme.ThemeConfig.animationNormal }
            }
        }

        // ── Detail Panel (right, collapsible) ───────────────────
        Rectangle {
            id: detailPanel
            visible: mainShell.detailPanelOpen
            Layout.fillHeight: true
            Layout.preferredWidth: visible ? Theme.ThemeConfig.detailPanelWidth : 0
            color: Theme.ThemeConfig.surfaceColor
            border.color: Theme.ThemeConfig.dividerColor
            border.width: 1

            Behavior on Layout.preferredWidth {
                NumberAnimation { duration: Theme.ThemeConfig.animationNormal }
            }

            // Detail content placeholder
            Text {
                anchors.centerIn: parent
                text: qsTr("Details")
                color: Theme.ThemeConfig.textHint
                font.pixelSize: Theme.ThemeConfig.fontSizeBody
            }
        }
    }

    // ── Component cache ────────────────────────────────────────
    Component { id: conversationListPageComponent; Pages.ConversationListPage {} }
    Component { id: chatPageComponent; Pages.ChatPage {
        aiUserId: mainShell.currentAiUserId
        aiUserName: mainShell.currentAiUserName
        conversationId: mainShell.currentConversationId
    }}
    Component { id: contactDetailPageComponent; Pages.ContactDetailPage {
        aiUserId: mainShell.currentAiUserId
    }}
    Component { id: createAIUserPageComponent; Pages.CreateAIUserPage {} }
    Component { id: settingsPageComponent; Pages.SettingsPage {} }
    Component { id: loginPageComponent; Pages.LoginPage {} }

    // ── Confirm dialog ─────────────────────────────────────────
    Dialogs.ConfirmDialog {
        id: confirmDialog
    }
}
