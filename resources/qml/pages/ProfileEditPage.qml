import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

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
        C.BackHeader {
            title: profileKey ? qsTr("Edit Profile") : qsTr("New Profile")
            onBackClicked: closePage()
        }

        // ── Form ────────────────────────────────────────────────
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: Math.min(600, parent ? parent.width - 48 : 552)
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                spacing: Theme.spacingLarge

                Item { Layout.preferredHeight: Theme.spacingLarge }

                // Key field
                Text {
                    text: qsTr("Key")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeSmall
                    font.weight: Theme.fontWeightMedium
                }

                TextField {
                    id: keyField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    text: profileKey
                    placeholderText: activeFocus ? "" : qsTr("e.g. user_name, user_age")
                    font.pixelSize: Theme.fontSizeBody
                    readOnly: profileKey.length > 0

                    background: Rectangle {
                        radius: Theme.buttonRadius
                        color: keyField.readOnly
                               ? Theme.dividerColor
                               : Theme.backgroundColor
                        border.color: keyField.activeFocus
                                      ? Theme.primaryColor
                                      : Theme.dividerColor
                    }
                }

                // Category field
                Text {
                    text: qsTr("Category")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeSmall
                    font.weight: Theme.fontWeightMedium
                }

                TextField {
                    id: categoryField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    text: profileCategory
                    placeholderText: activeFocus ? "" : qsTr("e.g. personal, professional")
                    font.pixelSize: Theme.fontSizeBody

                    background: Rectangle {
                        radius: Theme.buttonRadius
                        color: Theme.backgroundColor
                        border.color: categoryField.activeFocus
                                      ? Theme.primaryColor
                                      : Theme.dividerColor
                    }
                }

                // Layer field
                Text {
                    text: qsTr("Layer")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeSmall
                    font.weight: Theme.fontWeightMedium
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
                        radius: Theme.buttonRadius
                        color: Theme.backgroundColor
                        border.color: Theme.dividerColor
                    }
                }

                // Value/Summary field
                Text {
                    text: qsTr("Value / Summary")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeSmall
                    font.weight: Theme.fontWeightMedium
                }

                TextArea {
                    id: valueField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    text: profileValue
                    placeholderText: activeFocus ? "" : qsTr("Profile content or value")
                    font.pixelSize: Theme.fontSizeBody
                    wrapMode: TextArea.Wrap
                    selectByMouse: true

                    background: Rectangle {
                        radius: Theme.buttonRadius
                        color: Theme.backgroundColor
                        border.color: valueField.activeFocus
                                      ? Theme.primaryColor
                                      : Theme.dividerColor
                    }
                }

                Item { Layout.preferredHeight: Theme.spacingMedium }

                // Save / Cancel
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingMedium

                    Item { Layout.fillWidth: true }

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 40
                        radius: Theme.buttonRadius
                        color: cancelBtnMouse.containsMouse
                               ? Theme.sidebarHover : "transparent"
                        border.color: Theme.dividerColor
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Cancel")
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSizeBody
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
                        radius: Theme.buttonRadius
                        color: saveBtnMouse.containsMouse
                               ? Theme.primaryLight
                               : Theme.primaryColor

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Save")
                            color: Theme.textOnPrimary
                            font.pixelSize: Theme.fontSizeBody
                            font.weight: Theme.fontWeightMedium
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

                Item { Layout.preferredHeight: Theme.spacingXLarge }
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
