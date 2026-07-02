import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme
import "../components" as C

Item {
    id: loginPage
    width: 420
    height: 480

    signal loginSuccess()

    Rectangle {
        anchors.fill: parent
        radius: Theme.ThemeConfig.cardRadius
        color: Theme.ThemeConfig.surfaceColor
        border.color: Theme.ThemeConfig.dividerColor
        border.width: 1

        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0; verticalOffset: 4
            radius: 16; samples: 25
            color: "#18000000"
        }

        ColumnLayout {
            anchors {
                fill: parent
                topMargin: Theme.ThemeConfig.spacingXLarge
                bottomMargin: Theme.ThemeConfig.spacingXLarge
                leftMargin: 40
                rightMargin: 40
            }
            spacing: Theme.ThemeConfig.spacingLarge

            // Logo / Title
            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: Theme.ThemeConfig.spacingSmall

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "AI Chat"
                    color: Theme.ThemeConfig.primaryColor
                    font.pixelSize: Theme.ThemeConfig.fontSizeDisplay
                    font.weight: Theme.ThemeConfig.fontWeightBold
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Sign in to your account")
                    color: Theme.ThemeConfig.textSecondary
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                }
            }

            Item { Layout.preferredHeight: Theme.ThemeConfig.spacingSmall }

            // Username field
            ColumnLayout {
                spacing: Theme.ThemeConfig.spacingTiny

                Text {
                    text: qsTr("Username")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                TextField {
                    id: usernameField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    placeholderText: qsTr("Enter your username")
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    color: Theme.ThemeConfig.textPrimary

                    background: Rectangle {
                        radius: Theme.ThemeConfig.buttonRadius
                        color: Theme.ThemeConfig.backgroundColor
                        border.color: usernameField.activeFocus
                                      ? Theme.ThemeConfig.primaryColor
                                      : Theme.ThemeConfig.dividerColor
                        border.width: usernameField.activeFocus ? 2 : 1
                    }

                    // Bind to LoginViewModel if available
                    Binding {
                        target: typeof loginViewModel !== "undefined" ? loginViewModel : null
                        property: "username"
                        value: usernameField.text
                        when: typeof loginViewModel !== "undefined"
                    }
                }
            }

            // Password field
            ColumnLayout {
                spacing: Theme.ThemeConfig.spacingTiny

                Text {
                    text: qsTr("Password")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                TextField {
                    id: passwordField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    placeholderText: qsTr("Enter your password")
                    echoMode: TextInput.Password
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody

                    background: Rectangle {
                        radius: Theme.ThemeConfig.buttonRadius
                        color: Theme.ThemeConfig.backgroundColor
                        border.color: passwordField.activeFocus
                                      ? Theme.ThemeConfig.primaryColor
                                      : Theme.ThemeConfig.dividerColor
                        border.width: passwordField.activeFocus ? 2 : 1
                    }

                    Binding {
                        target: typeof loginViewModel !== "undefined" ? loginViewModel : null
                        property: "password"
                        value: passwordField.text
                        when: typeof loginViewModel !== "undefined"
                    }

                    // Enter to login
                    Keys.onReturnPressed: performLogin()
                    Keys.onEnterPressed: performLogin()
                }
            }

            // Error message
            Rectangle {
                Layout.fillWidth: true
                visible: errorText.text.length > 0
                height: errorText.implicitHeight + 12
                radius: 4
                color: "#FFEBEE"

                Text {
                    id: errorText
                    anchors {
                        left: parent.left
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: Theme.ThemeConfig.spacingMedium
                        rightMargin: Theme.ThemeConfig.spacingMedium
                    }
                    text: typeof loginViewModel !== "undefined" ? loginViewModel.errorMessage : ""
                    color: Theme.ThemeConfig.errorColor
                    font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                    wrapMode: Text.Wrap
                }
            }

            Item { Layout.preferredHeight: Theme.ThemeConfig.spacingSmall }

            // Login button
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 44
                radius: Theme.ThemeConfig.buttonRadius
                color: loginBtnMouse.containsMouse
                       ? Theme.ThemeConfig.primaryLight
                       : Theme.ThemeConfig.primaryColor
                opacity: loginEnabled ? 1.0 : 0.6

                property bool loginEnabled: usernameField.text.trim().length > 0
                                          && passwordField.text.length > 0

                Text {
                    anchors.centerIn: parent
                    text: qsTr("Sign In")
                    color: Theme.ThemeConfig.textOnPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                MouseArea {
                    id: loginBtnMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    enabled: parent.loginEnabled
                    onClicked: performLogin()
                }
            }

            // Loading spinner
            C.LoadingSpinner {
                Layout.alignment: Qt.AlignHCenter
                running: typeof loginViewModel !== "undefined" && loginViewModel.loading
                visible: typeof loginViewModel !== "undefined" && loginViewModel.loading
            }

            Item { Layout.fillHeight: true }
        }
    }

    function performLogin() {
        if (typeof loginViewModel !== "undefined") {
            loginViewModel.login()
        } else {
            // Standalone test mode
            loginPage.loginSuccess()
        }
    }

    // ── C++ viewmodel connections ──────────────────────────────
    Connections {
        target: typeof loginViewModel !== "undefined" ? loginViewModel : null
        enabled: typeof loginViewModel !== "undefined"
        function onLoginSuccess() {
            loginPage.loginSuccess()
        }
    }
}
