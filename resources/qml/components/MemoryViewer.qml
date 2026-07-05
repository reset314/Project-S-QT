import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: viewer; width: 520; height: 600; modal: true; closePolicy: Popup.CloseOnEscape
    x: (parent.width - width) / 2; y: (parent.height - height) / 2; padding: 0
    property string aiUserId: ""

    background: Rectangle { radius: 12; color: Theme.surfaceColor; border.color: Theme.dividerColor; border.width: 1 }

    ColumnLayout {
        anchors.fill: parent; spacing: 0

        // Zone 1: Top bar
        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 44; color: "transparent"
            RowLayout { anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 8
                Text { text: qsTr("Memories"); font.pixelSize: Theme.fontSizeSubheading; font.weight: Font.Bold; color: Theme.textPrimary; Layout.fillWidth: true }
                Rectangle { width: 28; height: 28; radius: 14; color: closeMouse.containsMouse ? Theme.errorColor : "transparent"
                    Icon { anchors.centerIn: parent; name: "x"; size: 16; color: closeMouse.containsMouse ? "white" : Theme.textSecondary }
                    MouseArea { id: closeMouse; anchors.fill: parent; hoverEnabled: true; onClicked: viewer.close() }
                }
            }
        }

        // Zone 2: Filter
        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 40; color: Theme.backgroundColor
            RowLayout { anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12
                Text { text: qsTr("Layer:"); font.pixelSize: Theme.fontSizeSmall; color: Theme.textSecondary }
                CheckBox { id: cbStm; text: "STM"; checked: false; font.pixelSize: Theme.fontSizeSmall }
                CheckBox { id: cbMtm; text: "MTM"; checked: true; font.pixelSize: Theme.fontSizeSmall }
                CheckBox { id: cbLtm; text: "LTM"; checked: true; font.pixelSize: Theme.fontSizeSmall }
                Item { Layout.fillWidth: true }
                Button { text: qsTr("Refresh"); font.pixelSize: Theme.fontSizeSmall; onClicked: loadMemories() }
            }
        }

        // Zone 3: Memory list
        ListView {
            id: memList; Layout.fillWidth: true; Layout.fillHeight: true; clip: true; spacing: 4
            model: ListModel { id: memoryModel }
            delegate: Rectangle {
                width: memList.width; height: memCard.implicitHeight + 12
                color: Theme.surfaceColor; border.color: Theme.dividerColor; border.width: 1; radius: 6
                ColumnLayout {
                    id: memCard; anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; anchors.margins: 8; spacing: 4
                    Text { text: model.summary || model.content.substring(0, 100); font.pixelSize: Theme.fontSizeBody; color: Theme.textPrimary; Layout.fillWidth: true; wrapMode: Text.Wrap }
                    RowLayout { spacing: 8
                        Rectangle { color: typeColor(model.memoryType); radius: 4; width: typeLabel.implicitWidth + 8; height: 18
                            Text { id: typeLabel; anchors.centerIn: parent; text: model.memoryType; font.pixelSize: 10; color: "white" } }
                        Text { text: model.category; font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
                        Item { Layout.fillWidth: true }
                        Text { text: model.createdAt || ""; font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
                    }
                }
            }
        }
    }

    function typeColor(t) { if (t === "STM") return Theme.warningColor; if (t === "MTM") return Theme.primaryColor; return Theme.successColor }
    function loadMemories() {
        var types = []; if (cbStm.checked) types.push("STM"); if (cbMtm.checked) types.push("MTM"); if (cbLtm.checked) types.push("LTM")
        memoryModel.clear()
        if (typeof memoryRepo === "undefined" || !aiUserId) return
        // TODO: call memoryRepo.getMemoriesJson(aiUserId, types) when added
    }
}
