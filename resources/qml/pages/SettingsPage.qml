import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: settingsPage
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ──────────────────────────────────────────────
        C.BackHeader {
            title: qsTr("Settings")
            onBackClicked: closePage()
        }

        // ── Settings List ───────────────────────────────────────
        ListView {
            id: settingsList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 1

            model: ListModel {
                ListElement {
                    section: "Account"
                    title: "Account"
                    subtitle: "Manage your account settings"
                    icon: "user-plus"
                    page: "AccountPage"
                }
                ListElement {
                    section: "Appearance"
                    title: "Appearance"
                    subtitle: "Theme, font size, and display"
                    icon: "palette"
                    page: "AppearanceSection"
                }
                ListElement {
                    section: "Voice"
                    title: "TTS Voices"
                    subtitle: "Text-to-speech voice selection"
                    icon: "play"
                    page: "TTSVoicesPage"
                }
                ListElement {
                    section: "Profiles"
                    title: "User Profiles"
                    subtitle: "Manage extracted user profiles"
                    icon: "brain"
                    page: "ProfilePage"
                }
                ListElement {
                    section: "Network"
                    title: "Server Configuration"
                    subtitle: "Server URL and network settings"
                    icon: "settings"
                    page: "NetworkSection"
                }
                ListElement {
                    section: "Plugins"
                    title: "Expansions"
                    subtitle: "Manage extension modules"
                    icon: "puzzle"
                    page: "ExpansionPage"
                }
                ListElement {
                    section: "About"
                    title: "About"
                    subtitle: "Version and license information"
                    icon: "info"
                    page: "AboutSection"
                }
            }

            section.property: "section"
            section.delegate: Rectangle {
                width: settingsList.width
                height: 32
                color: Theme.backgroundColor

                Text {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.spacingLarge
                        verticalCenter: parent.verticalCenter
                    }
                    text: section
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeCaption
                    font.weight: Theme.fontWeightBold
                }
            }

            delegate: Rectangle {
                width: settingsList.width
                height: 56
                color: rowMouse.containsMouse
                       ? Theme.sidebarHover : Theme.surfaceColor

                RowLayout {
                    anchors {
                        fill: parent
                        leftMargin: Theme.spacingLarge
                        rightMargin: Theme.spacingMedium
                    }
                    spacing: Theme.spacingMedium

                    // Icon
                    Rectangle {
                        Layout.preferredWidth: 36; Layout.preferredHeight: 36
                        radius: 18
                        color: Theme.sidebarHover
                        C.Icon { anchors.centerIn: parent; name: icon; size: 18 }
                    }

                    // Title + subtitle
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        Text {
                            text: title
                            color: Theme.textPrimary
                            font.pixelSize: Theme.fontSizeBody
                            font.weight: Theme.fontWeightMedium
                        }

                        Text {
                            text: subtitle
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSizeSmall
                        }
                    }

                    // Arrow
                    C.Icon { name: "chevron-right"; size: 20; color: Theme.textHint }
                }

                MouseArea {
                    id: rowMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: navigateToPage(page)
                }

                // Divider
                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                        leftMargin: 68
                    }
                    height: 1
                    color: Theme.dividerColor
                }
            }
        }
    }

    // ── Navigation ───────────────────────────────────────────────
    function navigateToPage(page) {
        switch (page) {
            case "AccountPage":
                navigateToComponent("AccountPage")
                break
            case "TTSVoicesPage":
                navigateToComponent("TTSVoicesPage")
                break
            case "ProfilePage":
                navigateToComponent("ProfilePage")
                break
            case "ExpansionPage":
                navigateToComponent("ExpansionPage")
                break
            default:
                console.log("Page not yet implemented:", page)
                break
        }
    }

    function navigateToComponent(componentName) {
        var sv = findStackView()
        if (!sv) return

        switch (componentName) {
            case "AccountPage":
                sv.push(accountPageComponent)
                break
            case "TTSVoicesPage":
                sv.push(ttsVoicesPageComponent)
                break
            case "ProfilePage":
                sv.push(profilePageComponent)
                break
            case "ExpansionPage":
                sv.push(expansionPageComponent)
                break
        }
    }

    function closePage() {
        var sv = findStackView()
        if (sv) sv.pop()
    }

    function findStackView() {
        var obj = settingsPage.parent
        while (obj) {
            if (obj instanceof StackView) return obj
            obj = obj.parent
        }
        return null
    }

    // Component cache
    Component { id: accountPageComponent; AccountPage {} }
    Component { id: ttsVoicesPageComponent; TTSVoicesPage {} }
    Component { id: profilePageComponent; ProfilePage {} }
    Component { id: expansionPageComponent; ExpansionPage {} }
}
