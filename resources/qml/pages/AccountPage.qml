import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: accountPage
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ──────────────────────────────────────────────
        C.BackHeader {
            title: qsTr("Account")
            onBackClicked: closePage()
        }

        // ── Content ─────────────────────────────────────────────
        ScrollView {
            id: accountScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: accountScroll.availableWidth
                spacing: Theme.spacingLarge

                Item { Layout.preferredHeight: Theme.spacingLarge }

                // User info card
                Rectangle {
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.spacingXLarge
                    Layout.rightMargin: Theme.spacingXLarge
                    height: 80
                    radius: Theme.cardRadius
                    color: Theme.surfaceColor
                    border.color: Theme.dividerColor
                    border.width: 1

                    RowLayout {
                        anchors {
                            fill: parent
                            leftMargin: Theme.spacingLarge
                            rightMargin: Theme.spacingLarge
                        }
                        spacing: Theme.spacingMedium

                        Text {
                            text: "👤"
                            font.pixelSize: 32
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Text {
                                text: qsTr("Current User")
                                color: Theme.textPrimary
                                font.pixelSize: Theme.fontSizeSubheading
                                font.weight: Theme.fontWeightMedium
                            }

                            Text {
                                text: qsTr("Signed in")
                                color: Theme.onlineColor
                                font.pixelSize: Theme.fontSizeSmall
                            }
                        }
                    }
                }

                Item { Layout.preferredHeight: Theme.spacingMedium }

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
                    Layout.leftMargin: Theme.spacingXLarge
                    Layout.rightMargin: Theme.spacingXLarge
                    radius: Theme.buttonRadius
                    color: logoutMouse.containsMouse
                           ? "#E57373" : Theme.errorColor

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Sign Out")
                        color: "white"
                        font.pixelSize: Theme.fontSizeBody
                        font.weight: Theme.fontWeightMedium
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
        Layout.leftMargin: Theme.spacingXLarge
        Layout.rightMargin: Theme.spacingXLarge
        height: 48
        radius: Theme.buttonRadius
        color: Theme.surfaceColor
        border.color: Theme.dividerColor
        border.width: 1

        RowLayout {
            anchors {
                fill: parent
                leftMargin: Theme.spacingLarge
                rightMargin: Theme.spacingLarge
            }
            spacing: Theme.spacingMedium

            Text {
                text: label
                color: Theme.textPrimary
                font.pixelSize: Theme.fontSizeBody
                Layout.fillWidth: true
            }

            Text {
                text: value
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSizeBody
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
