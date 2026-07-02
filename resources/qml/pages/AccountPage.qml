import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Item {
    id: accountPage
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
                    text: qsTr("Account")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }
            }
        }

        // ── Content ─────────────────────────────────────────────
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent ? parent.width : 400
                spacing: Theme.ThemeConfig.spacingLarge

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingLarge }

                // User info card
                Rectangle {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.ThemeConfig.spacingXLarge
                    Layout.rightMargin: Theme.ThemeConfig.spacingXLarge
                    height: 80
                    radius: Theme.ThemeConfig.cardRadius
                    color: Theme.ThemeConfig.surfaceColor
                    border.color: Theme.ThemeConfig.dividerColor
                    border.width: 1

                    RowLayout {
                        anchors {
                            fill: parent
                            leftMargin: Theme.ThemeConfig.spacingLarge
                            rightMargin: Theme.ThemeConfig.spacingLarge
                        }
                        spacing: Theme.ThemeConfig.spacingMedium

                        Text {
                            text: "👤"
                            font.pixelSize: 32
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Text {
                                text: qsTr("Current User")
                                color: Theme.ThemeConfig.textPrimary
                                font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
                                font.weight: Theme.ThemeConfig.fontWeightMedium
                            }

                            Text {
                                text: qsTr("Signed in")
                                color: Theme.ThemeConfig.onlineColor
                                font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                            }
                        }
                    }
                }

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingMedium }

                // Server URL
                SettingsRow {
                    label: qsTr("Server URL")
                    value: typeof settingsViewModel !== "undefined"
                           ? settingsViewModel.serverUrl : ""
                }

                SettingsRow {
                    label: qsTr("Theme")
                    value: typeof settingsViewModel !== "undefined"
                           ? settingsViewModel.theme : qsTr("Light")
                }

                // Logout button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    Layout.leftMargin: Theme.ThemeConfig.spacingXLarge
                    Layout.rightMargin: Theme.ThemeConfig.spacingXLarge
                    radius: Theme.ThemeConfig.buttonRadius
                    color: logoutMouse.containsMouse
                           ? "#E57373" : Theme.ThemeConfig.errorColor

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Sign Out")
                        color: "white"
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                        font.weight: Theme.ThemeConfig.fontWeightMedium
                    }

                    MouseArea {
                        id: logoutMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: performLogout()
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }
    }

    // ── Component ────────────────────────────────────────────────
    component SettingsRow: Rectangle {
        property string label: ""
        property string value: ""

        Layout.fillWidth: true
        Layout.leftMargin: Theme.ThemeConfig.spacingXLarge
        Layout.rightMargin: Theme.ThemeConfig.spacingXLarge
        height: 48
        radius: Theme.ThemeConfig.buttonRadius
        color: Theme.ThemeConfig.surfaceColor
        border.color: Theme.ThemeConfig.dividerColor
        border.width: 1

        RowLayout {
            anchors {
                fill: parent
                leftMargin: Theme.ThemeConfig.spacingLarge
                rightMargin: Theme.ThemeConfig.spacingLarge
            }
            spacing: Theme.ThemeConfig.spacingMedium

            Text {
                text: label
                color: Theme.ThemeConfig.textPrimary
                font.pixelSize: Theme.ThemeConfig.fontSizeBody
                Layout.fillWidth: true
            }

            Text {
                text: value
                color: Theme.ThemeConfig.textSecondary
                font.pixelSize: Theme.ThemeConfig.fontSizeBody
            }
        }
    }

    function performLogout() {
        // Signal parent shell to show login
        if (typeof tokenManager !== "undefined") {
            tokenManager.clearTokens()
        }
        // Navigate to root and show login
        var obj = accountPage
        while (obj && obj.parent) {
            obj = obj.parent
            if (obj.hasOwnProperty("contentLoader")) {
                obj.contentLoader.showLogin()
                return
            }
        }
    }

    function closePage() {
        var sv = findStackView()
        if (sv) sv.pop()
    }

    function findStackView() {
        var obj = accountPage.parent
        while (obj) {
            if (obj instanceof StackView) return obj
            obj = obj.parent
        }
        return null
    }
}
