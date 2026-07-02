import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme
import "../components" as C

Item {
    id: contactListPage
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ──────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            color: Theme.ThemeConfig.surfaceColor
            border.color: Theme.ThemeConfig.dividerColor
            border.width: 1

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.ThemeConfig.spacingLarge
                    rightMargin: Theme.ThemeConfig.spacingMedium
                }
                spacing: Theme.ThemeConfig.spacingMedium

                Text {
                    text: qsTr("Contacts")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }
            }
        }

        // ── Search bar ──────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: Theme.ThemeConfig.surfaceColor
            border.color: Theme.ThemeConfig.dividerColor
            border.width: 1

            TextField {
                id: searchField
                anchors {
                    fill: parent
                    leftMargin: Theme.ThemeConfig.spacingLarge
                    rightMargin: Theme.ThemeConfig.spacingLarge
                    topMargin: Theme.ThemeConfig.spacingSmall
                    bottomMargin: Theme.ThemeConfig.spacingSmall
                }
                placeholderText: qsTr("Search contacts...")
                font.pixelSize: Theme.ThemeConfig.fontSizeBody

                background: Rectangle {
                    radius: Theme.ThemeConfig.buttonRadius
                    color: Theme.ThemeConfig.backgroundColor
                }

                leftPadding: Theme.ThemeConfig.spacingMedium
            }
        }

        // ── Contact list ────────────────────────────────────────
        ListView {
            id: contactListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 0

            model: typeof contactListModel !== "undefined" ? contactListModel : null

            delegate: C.ContactCard {
                width: contactListView.width
                aiUserId: model.id || ""
                aiUserName: model.name || ""
                aiUserDescription: model.description || ""
                aiUserAvatar: model.avatar || ""
                llmProvider: model.llmProvider || ""
                llmModel: model.llmModel || ""

                visible: matchesSearch(aiUserName, aiUserDescription)

                onClicked: {
                    navigateToDetail(model.id || "")
                }

                onChatClicked: {
                    navigateToChat(
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

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: Theme.ThemeConfig.spacingLarge

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "👥"
                        font.pixelSize: 48
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("No contacts yet")
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    }
                }
            }
        }

        // ── FAB: Create AI User ─────────────────────────────────
        Rectangle {
            anchors {
                bottom: parent.bottom
                right: parent.right
                bottomMargin: Theme.ThemeConfig.spacingLarge
                rightMargin: Theme.ThemeConfig.spacingLarge
            }
            width: 56; height: 56; radius: 28
            color: Theme.ThemeConfig.accentColor
            z: 2

            layer.enabled: true
            layer.effect: DropShadow {
                horizontalOffset: 0; verticalOffset: 4
                radius: 8; samples: 17
                color: "#40000000"
            }

            Text {
                anchors.centerIn: parent
                text: "+"
                color: "white"
                font.pixelSize: Theme.ThemeConfig.fontSizeDisplay
                font.weight: Theme.ThemeConfig.fontWeightBold
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: navigateToCreateAIUser()
            }
        }
    }

    // ── Filtering ────────────────────────────────────────────────
    function matchesSearch(name, description) {
        var q = searchField.text.toLowerCase().trim()
        if (q.length === 0) return true
        var n = (name || "").toLowerCase()
        var d = (description || "").toLowerCase()
        return n.indexOf(q) >= 0 || d.indexOf(q) >= 0
    }

    // ── Navigation ───────────────────────────────────────────────
    function navigateToDetail(aiUserId) {
        var p = parent
        while (p) {
            if (p.hasOwnProperty("contactSelected")) {
                p.contactSelected(aiUserId)
                return
            }
            p = p.parent
        }
    }

    function navigateToChat(aiUserId, aiUserName, conversationId) {
        var p = parent
        while (p) {
            if (p.hasOwnProperty("chatSelected")) {
                p.chatSelected(aiUserId, aiUserName, conversationId)
                return
            }
            p = p.parent
        }
    }

    function navigateToCreateAIUser() {
        var p = parent
        while (p) {
            if (p.hasOwnProperty("createAIUserClicked")) {
                p.createAIUserClicked()
                return
            }
            p = p.parent
        }
    }
}
