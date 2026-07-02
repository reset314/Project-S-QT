import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Item {
    id: expansionPage
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
                    text: qsTr("Expansions")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }
            }
        }

        // ── Info Banner ─────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#E3F2FD"

            Text {
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    leftMargin: Theme.ThemeConfig.spacingLarge
                    rightMargin: Theme.ThemeConfig.spacingLarge
                }
                text: qsTr("Extension modules add capabilities to your AI assistants. Toggle modules on or off.")
                color: Theme.ThemeConfig.primaryDark
                font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                wrapMode: Text.Wrap
            }
        }

        // ── Expansion list ──────────────────────────────────────
        ListView {
            id: expansionList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 1

            // Placeholder model — loaded from C++ ExpansionRepository
            model: ListModel {
                id: expansionModel
            }

            section.property: "category"
            section.delegate: Rectangle {
                width: expansionList.width
                height: 32
                color: Theme.ThemeConfig.backgroundColor

                Text {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.ThemeConfig.spacingLarge
                        verticalCenter: parent.verticalCenter
                    }
                    text: section || "Other"
                    color: Theme.ThemeConfig.textHint
                    font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                    font.weight: Theme.ThemeConfig.fontWeightBold
                }
            }

            delegate: Rectangle {
                width: expansionList.width
                height: expansionContent.implicitHeight + 16
                color: Theme.ThemeConfig.surfaceColor

                RowLayout {
                    id: expansionContent
                    anchors {
                        fill: parent
                        leftMargin: Theme.ThemeConfig.spacingLarge
                        rightMargin: Theme.ThemeConfig.spacingMedium
                    }
                    spacing: Theme.ThemeConfig.spacingMedium

                    // Module info
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            text: model.name || model.title || qsTr("Unknown Module")
                            color: Theme.ThemeConfig.textPrimary
                            font.pixelSize: Theme.ThemeConfig.fontSizeBody
                            font.weight: Theme.ThemeConfig.fontWeightMedium
                        }

                        Text {
                            text: model.description || ""
                            color: Theme.ThemeConfig.textSecondary
                            font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                            Layout.fillWidth: true
                            wrapMode: Text.Wrap
                            maximumLineCount: 2
                        }

                        Text {
                            text: "v" + (model.version || "1.0.0") + " · " + (model.author || "")
                            color: Theme.ThemeConfig.textHint
                            font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                        }
                    }

                    // Toggle switch
                    Switch {
                        id: toggleSwitch
                        checked: model.enabled !== undefined ? model.enabled : true

                        onCheckedChanged: {
                            toggleModule(model.moduleId || model.id || "", checked)
                        }
                    }
                }

                // Divider
                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                        leftMargin: Theme.ThemeConfig.spacingLarge
                    }
                    height: 1
                    color: Theme.ThemeConfig.dividerColor
                }
            }

            // Empty state
            Rectangle {
                visible: expansionList.count === 0
                anchors.fill: parent
                color: "transparent"

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: Theme.ThemeConfig.spacingLarge

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "🧩"
                        font.pixelSize: 48
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("No expansions installed")
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Install extension modules to add new capabilities")
                        color: Theme.ThemeConfig.textHint
                        font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                    }
                }
            }
        }
    }

    // ── Actions ──────────────────────────────────────────────────
    function toggleModule(moduleId, enabled) {
        console.log("Toggle module:", moduleId, enabled)
        if (typeof expansionRepo !== "undefined") {
            expansionRepo.setEnabled(moduleId, enabled)
        }
    }

    Component.onCompleted: {
        loadModules()
    }

    function loadModules() {
        if (typeof expansionRepo !== "undefined") {
            expansionRepo.getAllModules(function(success, data) {
                if (success && data) {
                    expansionModel.clear()
                    try {
                        var modules = JSON.parse(data)
                        for (var i = 0; i < modules.length; i++) {
                            expansionModel.append(modules[i])
                        }
                    } catch (e) {
                        console.error("Failed to parse modules:", e)
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
        var obj = expansionPage.parent
        while (obj) {
            if (obj instanceof StackView) return obj
            obj = obj.parent
        }
        return null
    }
}
