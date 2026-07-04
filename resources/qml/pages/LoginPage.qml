import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: loginPage
    width: 420
    height: 480

    signal loginSuccess()

    Rectangle {
        anchors.fill: parent
        radius: Theme.cardRadius
        color: Theme.surfaceColor
        border.color: Theme.dividerColor
        border.width: 1

        layer.enabled: true

        ColumnLayout {
            anchors {
                fill: parent
                topMargin: Theme.spacingXLarge
                bottomMargin: Theme.spacingXLarge
                leftMargin: 40
                rightMargin: 40
            }
            spacing: Theme.spacingLarge

            // Logo / Title
            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: Theme.spacingSmall

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Project-S"
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeDisplay
                    font.weight: Theme.fontWeightBold
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Sign in to your account")
                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSizeBody
                }
            }

            Item { Layout.preferredHeight: Theme.spacingSmall }

            // Username field
            ColumnLayout {
                spacing: Theme.spacingTiny

                Text {
                    text: qsTr("Username")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeSmall
                    font.weight: Theme.fontWeightMedium
                }

                TextField {
                    id: usernameField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    placeholderText: activeFocus ? "" : qsTr("Enter your username")
                    font.pixelSize: Theme.fontSizeBody
                    color: Theme.textPrimary

                    background: Rectangle {
                        radius: Theme.buttonRadius
                        color: Theme.backgroundColor
                        border.color: usernameField.activeFocus
                                      ? Theme.primaryColor
                                      : Theme.dividerColor
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
                spacing: Theme.spacingTiny

                Text {
                    text: qsTr("Password")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeSmall
                    font.weight: Theme.fontWeightMedium
                }

                TextField {
                    id: passwordField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    placeholderText: activeFocus ? "" : qsTr("Enter your password")
                    echoMode: TextInput.Password
                    font.pixelSize: Theme.fontSizeBody

                    background: Rectangle {
                        radius: Theme.buttonRadius
                        color: Theme.backgroundColor
                        border.color: passwordField.activeFocus
                                      ? Theme.primaryColor
                                      : Theme.dividerColor
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
                        leftMargin: Theme.spacingMedium
                        rightMargin: Theme.spacingMedium
                    }
                    text: typeof loginViewModel !== "undefined" ? loginViewModel.errorMessage : ""
                    color: Theme.errorColor
                    font.pixelSize: Theme.fontSizeSmall
                    wrapMode: Text.Wrap
                }
            }

            Item { Layout.preferredHeight: Theme.spacingSmall }

            // Login button
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 44
                radius: Theme.buttonRadius
                color: loginBtnMouse.containsMouse
                       ? Theme.primaryLight
                       : Theme.primaryColor
                opacity: loginEnabled ? 1.0 : 0.6

                property bool loginEnabled: usernameField.text.trim().length > 0
                                          && passwordField.text.length > 0

                Text {
                    anchors.centerIn: parent
                    text: qsTr("Sign In")
                    color: Theme.textOnPrimary
                    font.pixelSize: Theme.fontSizeBody
                    font.weight: Theme.fontWeightMedium
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
