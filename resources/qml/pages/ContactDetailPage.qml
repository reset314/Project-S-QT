import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: contactDetailPage
    anchors.fill: parent

    property string aiUserId: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ──────────────────────────────────────────────
        C.BackHeader {
            title: qsTr("Contact Details")
            onBackClicked: closeDetail()
        }

        // ── Detail content ──────────────────────────────────────
        ScrollView {
            id: contactScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: contactScroll.availableWidth
                spacing: Theme.spacingLarge

                // Avatar + name section
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 140

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: Theme.spacingMedium

                        C.UserAvatar {
                            Layout.alignment: Qt.AlignHCenter
                            size: Theme.avatarSizeXLarge
                            name: detailName
                        }

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: detailName
                            color: Theme.textPrimary
                            font.pixelSize: Theme.fontSizeTitle
                            font.weight: Theme.fontWeightBold
                        }

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: detailDescription
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSizeBody
                            visible: detailDescription.length > 0
                        }
                    }
                }

                // Info cards
                DetailCard {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.spacingLarge
                    Layout.rightMargin: Theme.spacingLarge
                    label: qsTr("AI User ID")
                    value: aiUserId
                }

                DetailCard {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.spacingLarge
                    Layout.rightMargin: Theme.spacingLarge
                    label: qsTr("LLM Provider")
                    value: detailLlmProvider
                }

                DetailCard {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.spacingLarge
                    Layout.rightMargin: Theme.spacingLarge
                    label: qsTr("LLM Model")
                    value: detailLlmModel
                }

                DetailCard {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.spacingLarge
                    Layout.rightMargin: Theme.spacingLarge
                    label: qsTr("Created")
                    value: detailCreatedAt
                }

                Item { Layout.preferredHeight: Theme.spacingMedium }

                // Start chat button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    Layout.leftMargin: Theme.spacingLarge
                    Layout.rightMargin: Theme.spacingLarge
                    radius: Theme.buttonRadius
                    color: chatBtnMouse.containsMouse
                           ? Theme.primaryLight : Theme.primaryColor

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Start Chat")
                        color: Theme.textOnPrimary
                        font.pixelSize: Theme.fontSizeBody
                        font.weight: Theme.fontWeightMedium
                    }

                    MouseArea {
                        id: chatBtnMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: startChat()
                    }
                }
            }
        }
    }

    // ── Contact data (looked up from model) ─────────────────────
    property string detailName: ""
    property string detailDescription: ""
    property string detailLlmProvider: ""
    property string detailLlmModel: ""
    property string detailCreatedAt: ""

    // Look up contact info from model
    Connections {
        target: typeof contactListModel !== "undefined" ? contactListModel : null
        enabled: typeof contactListModel !== "undefined"
        function onModelReset() {
            lookupContact()
        }
        function onRowsInserted() {
            lookupContact()
        }
    }

    Component.onCompleted: {
        lookupContact()
    }

    function lookupContact() {
        if (typeof contactListModel === "undefined" || !aiUserId) return
        for (var i = 0; i < contactListModel.rowCount(); i++) {
            var idx = contactListModel.index(i, 0)
            // Roles: Id=257, Name=258, Description=259, Avatar=260,
            // CreatedBy=261, LlmProvider=262, LlmModel=263, CreatedAt=264
            var id = contactListModel.data(idx, 257)  // IdRole
            if (id === aiUserId) {
                detailName = contactListModel.data(idx, 258) || ""
                detailDescription = contactListModel.data(idx, 259) || ""
                detailLlmProvider = contactListModel.data(idx, 262) || ""
                detailLlmModel = contactListModel.data(idx, 263) || ""
                detailCreatedAt = contactListModel.data(idx, 264) || ""
                return
            }
        }
    }

    // ── Navigation ───────────────────────────────────────────────
    function closeDetail() {
        var sv = findStackView()
        if (sv) sv.pop()
    }

    function startChat() {
        var p = parent
        while (p) {
            if (p.hasOwnProperty("chatSelected")) {
                p.chatSelected(aiUserId, detailName, "")
                return
            }
            p = p.parent
        }
    }

    function findStackView() {
        var obj = contactDetailPage.parent
        while (obj) {
            if (obj instanceof StackView) return obj
            obj = obj.parent
        }
        return null
    }

    // ── Detail card component ──────────────────────────────────
    component DetailCard: Rectangle {
        property string label: ""
        property string value: ""

        height: detailRow.implicitHeight + 16
        radius: Theme.cardRadius
        color: Theme.surfaceColor
        border.color: Theme.dividerColor
        border.width: 1

        ColumnLayout {
            id: detailRow
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
                leftMargin: Theme.spacingLarge
                rightMargin: Theme.spacingLarge
            }
            spacing: 2

            Text {
                text: label
                color: Theme.textHint
                font.pixelSize: Theme.fontSizeCaption
            }

            Text {
                text: value || qsTr("N/A")
                color: Theme.textPrimary
                font.pixelSize: Theme.fontSizeBody
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }
}
