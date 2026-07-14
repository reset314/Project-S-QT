import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: profilePage
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ──────────────────────────────────────────────
        C.BackHeader {
            title: qsTr("User Profiles")
            onBackClicked: closePage()
        }

        // ── Profile list ────────────────────────────────────────
        ListView {
            id: profileList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: Theme.spacingMedium
            topMargin: Theme.spacingLarge

            // Placeholder model - profiles loaded via C++ repository
            model: ListModel {
                id: profilesModel
            }

            delegate: Rectangle {
                width: profileList.width - 2 * Theme.spacingLarge
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                height: profileCard.implicitHeight + 16
                radius: Theme.cardRadius
                color: Theme.surfaceColor
                border.color: Theme.dividerColor
                border.width: 1

                ColumnLayout {
                    id: profileCard
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        leftMargin: Theme.spacingLarge
                        rightMargin: Theme.spacingLarge
                        topMargin: Theme.spacingSmall
                    }
                    spacing: Theme.spacingSmall

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.spacingMedium

                        Text {
                            text: model.attribute_key || ""
                            color: Theme.textPrimary
                            font.pixelSize: Theme.fontSizeSubheading
                            font.weight: Theme.fontWeightBold
                            Layout.fillWidth: true
                        }

                        Rectangle {
                            radius: 4; width: confLabel.implicitWidth + 12; height: 20
                            color: (model.confidence >= 0.8) ? Theme.successColor : ((model.confidence >= 0.5) ? Theme.warningColor : Theme.errorColor)
                            Text { id: confLabel; anchors.centerIn: parent; text: Math.round((model.confidence || 0) * 100) + "%"; font.pixelSize: 10; color: "white" }
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        text: model.content || ""
                        color: Theme.textSecondary
                        font.pixelSize: Theme.fontSizeBody
                        wrapMode: Text.Wrap
                        maximumLineCount: 3
                    }

                    RowLayout { spacing: 8
                        Text { text: model.source || ""; font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
                        Item { Layout.fillWidth: true }
                        Text { text: model.created_at || ""; font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
                    }
                }
            }

            // Empty state
            Rectangle {
                visible: profileList.count === 0
                anchors.fill: parent
                color: "transparent"

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: Theme.spacingLarge

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: ""
                        font.pixelSize: 48
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("No profiles yet")
                        color: Theme.textHint
                        font.pixelSize: Theme.fontSizeBody
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Profiles are extracted from conversations automatically")
                        color: Theme.textHint
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }
            }
        }
    }

    // Load profiles on page open
    Component.onCompleted: {
        loadProfiles()
    }

    function loadProfiles() {
        if (typeof profileRepo === "undefined") return
        var json = profileRepo.getProfilesJson()
        if (!json || json.error) return
        profilesModel.clear()
        var profiles = json.profiles || []
        for (var i = 0; i < profiles.length; i++) profilesModel.append(profiles[i])
    }

    function closePage() {
        var sv = findStackView()
        if (sv) sv.pop()
    }

    function findStackView() {
        var obj = profilePage.parent
        while (obj) {
            if (obj instanceof StackView) return obj
            obj = obj.parent
        }
        return null
    }
}
