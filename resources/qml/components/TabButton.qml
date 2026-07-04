import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: tab
    property string iconName: ""
    property string label: ""
    property bool selected: false
    signal clicked()

    width: 56; height: 52
    color: selected ? Theme.sidebarSelected : "transparent"

    Column {
        anchors.centerIn: parent; spacing: 2
        Icon { name: tab.iconName; size: 22; anchors.horizontalCenter: parent.horizontalCenter
            color: tab.selected ? Theme.primaryColor : Theme.textSecondary }
        Text { visible: tab.label !== ""; text: tab.label; font.pixelSize: 10;
            color: tab.selected ? Theme.primaryColor : Theme.textSecondary;
            anchors.horizontalCenter: parent.horizontalCenter }
    }

    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: tab.clicked() }
}
