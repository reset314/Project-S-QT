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
                        Rectangle {
                            color: categoryColor(model.category)
                            radius: 4; width: catText.implicitWidth + 12; height: 20
                            Text { id: catText; anchors.centerIn: parent; text: model.category || "ai"; font.pixelSize: 10; color: "white" }
                        }
                    }
                    Text { text: model.description || ""; font.pixelSize: Theme.fontSizeSmall; color: Theme.textSecondary; Layout.fillWidth: true; wrapMode: Text.Wrap; maximumLineCount: 2; elide: Text.ElideRight }
                    RowLayout { spacing: 8
                        Text { text: qsTr("Functions:") + " " + (Array.isArray(model.functions) ? model.functions.length : 0); font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
                        Item { Layout.fillWidth: true }
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

    function categoryColor(cat) {
        if (cat === "client") return Theme.primaryColor
        if (cat === "data") return Theme.successColor
        return Theme.warningColor  // ai
    }
    function loadModules() {
        if (typeof expansionRepo === "undefined") return
        var json = expansionRepo.listModulesJson()
        moduleModel.clear()
        if (!json || json.error) return
        var modules = json.modules || []
        for (var i = 0; i < modules.length; i++) moduleModel.append(modules[i])
    }

    Component.onCompleted: loadModules()

    function closePage() { var sv = findStackView(); if (sv) sv.pop() }
    function findStackView() { var o = page.parent; while (o) { if (o instanceof StackView) return o; o = o.parent } return null }
}
