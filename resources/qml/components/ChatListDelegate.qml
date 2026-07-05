import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: item
    property string aiUserId: ""; property string name: ""; property string avatar: ""
    property string lastMessage: ""; property string lastTime: ""; property int unread: 0
    signal clicked()

    width: parent ? parent.width : 300; height: 64
    color: itemMouse.containsMouse ? Theme.sidebarHover : "transparent"

    RowLayout {
        anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 8; anchors.topMargin: 8; anchors.bottomMargin: 8
        spacing: 10

        UserAvatar { Layout.preferredWidth: 44; Layout.preferredHeight: 44; name: item.name; imageSource: item.avatar }

        ColumnLayout {
            Layout.fillWidth: true; spacing: 2
            RowLayout {
                Text { text: item.name || qsTr("Unknown"); font.pixelSize: Theme.fontSizeBody; font.weight: Font.DemiBold
                    color: Theme.textPrimary; Layout.fillWidth: true; elide: Text.ElideRight }
                Text { visible: item.lastTime !== ""; text: item.lastTime; font.pixelSize: Theme.fontSizeCaption
                    color: Theme.textHint }
            }
            RowLayout {
                Text { text: item.lastMessage || ""; font.pixelSize: Theme.fontSizeSmall; color: Theme.textSecondary
                    Layout.fillWidth: true; elide: Text.ElideRight; maximumLineCount: 1 }
                Rectangle {
                    visible: item.unread > 0; width: unreadText.implicitWidth + 8; height: 18; radius: 9
                    color: Theme.unreadBadgeColor
                    Text { id: unreadText; anchors.centerIn: parent; text: item.unread > 99 ? "99+" : item.unread
                        font.pixelSize: 10; color: "white" }
                }
            }
        }
    }

    MouseArea { id: itemMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
        onClicked: item.clicked() }
}
