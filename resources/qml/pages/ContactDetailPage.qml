import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme
import "../components" as C

Item {
    id: contactDetailPage
    anchors.fill: parent

    property string aiUserId: ""

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

                // Back button
                Rectangle {
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    radius: 16
                    color: backMouse.containsMouse
                           ? Theme.ThemeConfig.sidebarHover : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "←"
                        font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                        color: Theme.ThemeConfig.textPrimary
                    }

                    MouseArea {
                        id: backMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: closeDetail()
                    }
                }

                Text {
                    text: qsTr("Contact Details")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }
            }
        }

        // ── Detail content ──────────────────────────────────────
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent ? parent.width : 400
                spacing: Theme.ThemeConfig.spacingLarge

                // Avatar + name section
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 140

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: Theme.ThemeConfig.spacingMedium

                        C.UserAvatar {
                            Layout.alignment: Qt.AlignHCenter
                            size: Theme.ThemeConfig.avatarSizeXLarge
                            name: detailName
                        }

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: detailName
                            color: Theme.ThemeConfig.textPrimary
                            font.pixelSize: Theme.ThemeConfig.fontSizeTitle
                            font.weight: Theme.ThemeConfig.fontWeightBold
                        }

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: detailDescription
                            color: Theme.ThemeConfig.textSecondary
                            font.pixelSize: Theme.ThemeConfig.fontSizeBody
                            visible: detailDescription.length > 0
                        }
                    }
                }

                // Info cards
                DetailCard {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.ThemeConfig.spacingLarge
                    Layout.rightMargin: Theme.ThemeConfig.spacingLarge
                    label: qsTr("AI User ID")
                    value: aiUserId
                }

                DetailCard {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.ThemeConfig.spacingLarge
                    Layout.rightMargin: Theme.ThemeConfig.spacingLarge
                    label: qsTr("LLM Provider")
                    value: detailLlmProvider
                }

                DetailCard {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.ThemeConfig.spacingLarge
                    Layout.rightMargin: Theme.ThemeConfig.spacingLarge
                    label: qsTr("LLM Model")
                    value: detailLlmModel
                }

                DetailCard {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.ThemeConfig.spacingLarge
                    Layout.rightMargin: Theme.ThemeConfig.spacingLarge
                    label: qsTr("Created")
                    value: detailCreatedAt
                }

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingMedium }

                // Start chat button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    Layout.leftMargin: Theme.ThemeConfig.spacingLarge
                    Layout.rightMargin: Theme.ThemeConfig.spacingLarge
                    radius: Theme.ThemeConfig.buttonRadius
                    color: chatBtnMouse.containsMouse
                           ? Theme.ThemeConfig.primaryLight : Theme.ThemeConfig.primaryColor

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Start Chat")
                        color: Theme.ThemeConfig.textOnPrimary
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                        font.weight: Theme.ThemeConfig.fontWeightMedium
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
            var id = contactListModel.data(idx, ContactListModel.IdRole)
            if (id === aiUserId) {
                detailName = contactListModel.data(idx, ContactListModel.NameRole) || ""
                detailDescription = contactListModel.data(idx, ContactListModel.DescriptionRole) || ""
                detailLlmProvider = contactListModel.data(idx, ContactListModel.LlmProviderRole) || ""
                detailLlmModel = contactListModel.data(idx, ContactListModel.LlmModelRole) || ""
                detailCreatedAt = contactListModel.data(idx, ContactListModel.CreatedAtRole) || ""
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
        radius: Theme.ThemeConfig.cardRadius
        color: Theme.ThemeConfig.surfaceColor
        border.color: Theme.ThemeConfig.dividerColor
        border.width: 1

        ColumnLayout {
            id: detailRow
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
                leftMargin: Theme.ThemeConfig.spacingLarge
                rightMargin: Theme.ThemeConfig.spacingLarge
            }
            spacing: 2

            Text {
                text: label
                color: Theme.ThemeConfig.textHint
                font.pixelSize: Theme.ThemeConfig.fontSizeCaption
            }

            Text {
                text: value || qsTr("N/A")
                color: Theme.ThemeConfig.textPrimary
                font.pixelSize: Theme.ThemeConfig.fontSizeBody
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }
}
