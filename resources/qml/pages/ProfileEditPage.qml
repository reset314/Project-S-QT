import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Item {
    id: profileEditPage
    anchors.fill: parent

    property string profileKey: ""
    property string profileCategory: ""
    property string profileLayer: ""
    property string profileValue: ""

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
                    text: profileKey ? qsTr("Edit Profile") : qsTr("New Profile")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }
            }
        }

        // ── Form ────────────────────────────────────────────────
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: Math.min(600, parent ? parent.width - 48 : 552)
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                spacing: Theme.ThemeConfig.spacingLarge

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingLarge }

                // Key field
                Text {
                    text: qsTr("Key")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                TextField {
                    id: keyField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    text: profileKey
                    placeholderText: qsTr("e.g. user_name, user_age")
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    readOnly: profileKey.length > 0

                    background: Rectangle {
                        radius: Theme.ThemeConfig.buttonRadius
                        color: keyField.readOnly
                               ? Theme.ThemeConfig.dividerColor
                               : Theme.ThemeConfig.backgroundColor
                        border.color: keyField.activeFocus
                                      ? Theme.ThemeConfig.primaryColor
                                      : Theme.ThemeConfig.dividerColor
                    }
                }

                // Category field
                Text {
                    text: qsTr("Category")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                TextField {
                    id: categoryField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    text: profileCategory
                    placeholderText: qsTr("e.g. personal, professional")
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody

                    background: Rectangle {
                        radius: Theme.ThemeConfig.buttonRadius
                        color: Theme.ThemeConfig.backgroundColor
                        border.color: categoryField.activeFocus
                                      ? Theme.ThemeConfig.primaryColor
                                      : Theme.ThemeConfig.dividerColor
                    }
                }

                // Layer field
                Text {
                    text: qsTr("Layer")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                ComboBox {
                    id: layerCombo
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    model: ["core", "contextual", "temporary"]
                    currentIndex: {
                        if (profileLayer === "contextual") return 1
                        if (profileLayer === "temporary") return 2
                        return 0
                    }

                    background: Rectangle {
                        radius: Theme.ThemeConfig.buttonRadius
                        color: Theme.ThemeConfig.backgroundColor
                        border.color: Theme.ThemeConfig.dividerColor
                    }
                }

                // Value/Summary field
                Text {
                    text: qsTr("Value / Summary")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                    font.weight: Theme.ThemeConfig.fontWeightMedium
                }

                TextArea {
                    id: valueField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    text: profileValue
                    placeholderText: qsTr("Profile content or value")
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    wrapMode: TextArea.Wrap
                    selectByMouse: true

                    background: Rectangle {
                        radius: Theme.ThemeConfig.buttonRadius
                        color: Theme.ThemeConfig.backgroundColor
                        border.color: valueField.activeFocus
                                      ? Theme.ThemeConfig.primaryColor
                                      : Theme.ThemeConfig.dividerColor
                    }
                }

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingMedium }

                // Save / Cancel
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.ThemeConfig.spacingMedium

                    Item { Layout.fillWidth: true }

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 40
                        radius: Theme.ThemeConfig.buttonRadius
                        color: cancelBtnMouse.containsMouse
                               ? Theme.ThemeConfig.sidebarHover : "transparent"
                        border.color: Theme.ThemeConfig.dividerColor
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Cancel")
                            color: Theme.ThemeConfig.textSecondary
                            font.pixelSize: Theme.ThemeConfig.fontSizeBody
                        }

                        MouseArea {
                            id: cancelBtnMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: closePage()
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 40
                        radius: Theme.ThemeConfig.buttonRadius
                        color: saveBtnMouse.containsMouse
                               ? Theme.ThemeConfig.primaryLight
                               : Theme.ThemeConfig.primaryColor

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Save")
                            color: Theme.ThemeConfig.textOnPrimary
                            font.pixelSize: Theme.ThemeConfig.fontSizeBody
                            font.weight: Theme.ThemeConfig.fontWeightMedium
                        }

                        MouseArea {
                            id: saveBtnMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: saveProfile()
                        }
                    }
                }

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingXLarge }
            }
        }
    }

    function saveProfile() {
        var payload = {
            key: keyField.text.trim(),
            category: categoryField.text.trim(),
            layer: layerCombo.currentText,
            value: valueField.text.trim()
        }

        if (typeof profileRepo !== "undefined") {
            if (profileKey) {
                profileRepo.updateProfile(profileKey, JSON.stringify(payload), function(ok) {
                    if (ok) closePage()
                })
            } else {
                profileRepo.createProfile(JSON.stringify(payload), function(ok) {
                    if (ok) closePage()
                })
            }
        } else {
            closePage()
        }
    }

    function closePage() {
        var sv = findStackView()
        if (sv) sv.pop()
    }

    function findStackView() {
        var obj = profileEditPage.parent
        while (obj) {
            if (obj instanceof StackView) return obj
            obj = obj.parent
        }
        return null
    }
}
