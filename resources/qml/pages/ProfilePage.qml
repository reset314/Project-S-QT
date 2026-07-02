import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Item {
    id: profilePage
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
                        onClicked: closePage()
                    }
                }

                Text {
                    text: qsTr("User Profiles")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }
            }
        }

        // ── Profile list ────────────────────────────────────────
        ListView {
            id: profileList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: Theme.ThemeConfig.spacingMedium
            topMargin: Theme.ThemeConfig.spacingLarge

            // Placeholder model - profiles loaded via C++ repository
            model: ListModel {
                id: profilesModel
            }

            delegate: Rectangle {
                width: profileList.width - 2 * Theme.ThemeConfig.spacingLarge
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                height: profileCard.implicitHeight + 16
                radius: Theme.ThemeConfig.cardRadius
                color: Theme.ThemeConfig.surfaceColor
                border.color: Theme.ThemeConfig.dividerColor
                border.width: 1

                ColumnLayout {
                    id: profileCard
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        leftMargin: Theme.ThemeConfig.spacingLarge
                        rightMargin: Theme.ThemeConfig.spacingLarge
                        topMargin: Theme.ThemeConfig.spacingSmall
                    }
                    spacing: Theme.ThemeConfig.spacingSmall

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: Theme.ThemeConfig.spacingMedium

                        Text {
                            text: model.key || ""
                            color: Theme.ThemeConfig.textPrimary
                            font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
                            font.weight: Theme.ThemeConfig.fontWeightBold
                            Layout.fillWidth: true
                        }

                        Text {
                            text: model.category || ""
                            color: Theme.ThemeConfig.textHint
                            font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        text: model.value || model.summary || ""
                        color: Theme.ThemeConfig.textSecondary
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                        wrapMode: Text.Wrap
                        maximumLineCount: 3
                    }

                    Text {
                        text: model.layer || ""
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeCaption
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
                    spacing: Theme.ThemeConfig.spacingLarge

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "📋"
                        font.pixelSize: 48
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("No profiles yet")
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Profiles are extracted from conversations automatically")
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeSmall
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
        // C++ integration
        if (typeof profileRepo !== "undefined") {
            profileRepo.getAllProfiles(function(success, data) {
                if (success && data) {
                    profilesModel.clear()
                    try {
                        var profiles = JSON.parse(data)
                        for (var i = 0; i < profiles.length; i++) {
                            profilesModel.append(profiles[i])
                        }
                    } catch (e) {
                        console.error("Failed to parse profiles:", e)
                    }
                }
            })
        }
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
