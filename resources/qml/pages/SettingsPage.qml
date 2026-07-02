import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Item {
    id: settingsPage
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
                        onClicked: closePage()
                    }
                }

                Text {
                    text: qsTr("Settings")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }
            }
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
                    icon: "👤"
                    page: "AccountPage"
                }
                ListElement {
                    section: "Appearance"
                    title: "Appearance"
                    subtitle: "Theme, font size, and display"
                    icon: "🎨"
                    page: "AppearanceSection"
                }
                ListElement {
                    section: "Voice"
                    title: "TTS Voices"
                    subtitle: "Text-to-speech voice selection"
                    icon: "🔊"
                    page: "TTSVoicesPage"
                }
                ListElement {
                    section: "Profiles"
                    title: "User Profiles"
                    subtitle: "Manage extracted user profiles"
                    icon: "📋"
                    page: "ProfilePage"
                }
                ListElement {
                    section: "Network"
                    title: "Server Configuration"
                    subtitle: "Server URL and network settings"
                    icon: "🌐"
                    page: "NetworkSection"
                }
                ListElement {
                    section: "Plugins"
                    title: "Expansions"
                    subtitle: "Manage extension modules"
                    icon: "🧩"
                    page: "ExpansionPage"
                }
                ListElement {
                    section: "About"
                    title: "About"
                    subtitle: "Version and license information"
                    icon: "ℹ️"
                    page: "AboutSection"
                }
            }

            section.property: "section"
            section.delegate: Rectangle {
                width: settingsList.width
                height: 32
                color: Theme.ThemeConfig.backgroundColor

                Text {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.ThemeConfig.spacingLarge
                        verticalCenter: parent.verticalCenter
                    }
                    text: section
                    color: Theme.ThemeConfig.textHint
                    font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                    font.weight: Theme.ThemeConfig.fontWeightBold
                }
            }

            delegate: Rectangle {
                width: settingsList.width
                height: 56
                color: rowMouse.containsMouse
                       ? Theme.ThemeConfig.sidebarHover : Theme.ThemeConfig.surfaceColor

                RowLayout {
                    anchors {
                        fill: parent
                        leftMargin: Theme.ThemeConfig.spacingLarge
                        rightMargin: Theme.ThemeConfig.spacingMedium
                    }
                    spacing: Theme.ThemeConfig.spacingMedium

                    // Icon
                    Rectangle {
                        Layout.preferredWidth: 36; Layout.preferredHeight: 36
                        radius: 18
                        color: Theme.ThemeConfig.sidebarHover

                        Text {
                            anchors.centerIn: parent
                            text: icon
                            font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                        }
                    }

                    // Title + subtitle
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        Text {
                            text: title
                            color: Theme.ThemeConfig.textPrimary
                            font.pixelSize: Theme.ThemeConfig.fontSizeBody
                            font.weight: Theme.ThemeConfig.fontWeightMedium
                        }

                        Text {
                            text: subtitle
                            color: Theme.ThemeConfig.textSecondary
                            font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                        }
                    }

                    // Arrow
                    Text {
                        text: "›"
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeTitle
                    }
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
                    color: Theme.ThemeConfig.dividerColor
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
