import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: avatar
    width: Theme.avatarSizeMedium
    height: Theme.avatarSizeMedium
    radius: width / 2
    color: generateColor(name)
    clip: true

    property string name: ""
    property string imageSource: ""
    property int size: Theme.avatarSizeMedium

    // Override default width/height based on size property
    Component.onCompleted: {
        if (size !== Theme.avatarSizeMedium) {
            avatar.width = size
            avatar.height = size
        }
    }

    // Show image if provided, otherwise show initials
    Image {
        anchors.fill: parent
        source: imageSource
        visible: imageSource !== ""
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        layer.enabled: true
    }

    Text {
        anchors.centerIn: parent
        text: initials(name)
        color: "white"
        font.pixelSize: Math.max(10, avatar.width * 0.38)
        font.weight: Theme.fontWeightBold
        visible: imageSource === ""
    }

    // ── Online indicator dot ─────────────────────────────────────
    property bool showOnline: false
    property bool isOnline: false

    Rectangle {
        visible: showOnline
        width: Math.max(8, avatar.width * 0.28)
        height: Math.max(8, avatar.width * 0.28)
        radius: width / 2
        color: isOnline ? Theme.onlineColor : Theme.offlineColor
        border.color: Theme.surfaceColor
        border.width: 2
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: -1
            bottomMargin: -1
        }
    }

    // ── Helper functions ─────────────────────────────────────────
    function initials(str) {
        if (!str || str.length === 0) return "?"
        var parts = str.trim().split(/\s+/)
        if (parts.length >= 2) {
            return (parts[0][0] + parts[parts.length - 1][0]).toUpperCase()
        }
        return str.substring(0, 2).toUpperCase()
    }

    function generateColor(str) {
        if (!str || str.length === 0) return Theme.primaryColor
        var hash = 0
        for (var i = 0; i < str.length; i++) {
            hash = str.charCodeAt(i) + ((hash << 5) - hash)
        }
        var colors = [
            "#1976D2", "#388E3C", "#D32F2F", "#F57C00",
            "#7B1FA2", "#0097A7", "#5D4037", "#303F9F"
        ]
        return colors[Math.abs(hash) % colors.length]
    }
}
