import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Reusable header with back button + title.
// Usage: BackHeader { title: "Settings"; onBackClicked: sv.pop() }
Rectangle {
    id: header
    Layout.fillWidth: true
    Layout.preferredHeight: 48
    color: Theme.surfaceColor
    border.color: Theme.dividerColor
    border.width: 1

    property string title: ""
    signal backClicked()

    RowLayout {
        anchors {
            fill: parent
            leftMargin: Theme.spacingMedium
            rightMargin: Theme.spacingMedium
        }
        spacing: Theme.spacingMedium

        // Back button
        Rectangle {
            Layout.preferredWidth: 32; Layout.preferredHeight: 32
            radius: 16
            color: backMouse.containsMouse ? Theme.sidebarHover : "transparent"

            Icon { anchors.centerIn: parent; name: "arrow-left"; size: 20 }

            MouseArea {
                id: backMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: header.backClicked()
            }
        }

        Text {
            Layout.fillWidth: true
            text: header.title
            color: Theme.textPrimary
            font.pixelSize: Theme.fontSizeSubheading
            font.weight: Theme.fontWeightBold
            elide: Text.ElideRight
        }
    }
}
