import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: page; anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent; spacing: 0
        C.BackHeader { title: qsTr("Extensions"); onBackClicked: closePage() }

        ListView {
            id: expList; Layout.fillWidth: true; Layout.fillHeight: true; clip: true; spacing: 1
            model: ListModel { id: moduleModel }

            delegate: Rectangle {
                width: expList.width; height: cardCol.implicitHeight + 16
                color: Theme.surfaceColor
                ColumnLayout {
                    id: cardCol; anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; anchors.margins: 12; spacing: 4
                    RowLayout {
                        Text { text: model.name || qsTr("Unknown"); font.pixelSize: Theme.fontSizeBody; font.weight: Font.Bold; color: Theme.textPrimary; Layout.fillWidth: true }
                        Rectangle { color: model.review_status === "approved" ? Theme.successColor : (model.review_status === "rejected" ? Theme.errorColor : Theme.warningColor)
                            radius: 4; width: statusText.implicitWidth + 12; height: 20
                            Text { id: statusText; anchors.centerIn: parent; text: model.review_status || "pending"; font.pixelSize: 10; color: "white" }
                        }
                    }
                    Text { text: model.description || ""; font.pixelSize: Theme.fontSizeSmall; color: Theme.textSecondary; Layout.fillWidth: true; wrapMode: Text.Wrap; maximumLineCount: 2 }
                    RowLayout { spacing: 8
                        Text { text: "v" + (model.version || "1.0.0"); font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
                        Text { text: "· "+ (model.author || ""); font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
                        Item { Layout.fillWidth: true }
                        Switch { checked: model.enabled !== false; onClicked: toggleModule(model.name || "", checked) }
                    }
                }
            }

            Rectangle { visible: expList.count === 0; anchors.fill: parent; color: "transparent"
                ColumnLayout { anchors.centerIn: parent; spacing: Theme.spacingMedium
                    C.Icon { name: "puzzle"; size: 48; anchors.horizontalCenter: parent.horizontalCenter; color: Theme.textHint }
                    Text { text: qsTr("No extensions installed"); color: Theme.textHint; font.pixelSize: Theme.fontSizeBody; anchors.horizontalCenter: parent.horizontalCenter }
                }
            }
        }
    }

    function loadModules() {
        if (typeof expansionRepo === "undefined") return
        var json = expansionRepo.listModulesJson()
        moduleModel.clear()
        if (!json || json.error) return
        var modules = json.modules || []
        for (var i = 0; i < modules.length; i++) moduleModel.append(modules[i])
    }

    function toggleModule(name, enabled) {
        if (typeof expansionRepo === "undefined") return
        expansionRepo.toggleModuleJson(name, enabled)
    }

    Component.onCompleted: loadModules()

    function closePage() { var sv = findStackView(); if (sv) sv.pop() }
    function findStackView() { var o = page.parent; while (o) { if (o instanceof StackView) return o; o = o.parent } return null }
}
