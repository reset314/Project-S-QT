import QtQuick 2.15
import QtQuick.Controls 2.15
import "../theme/ThemeConfig.qml" as Theme

Rectangle {
    id: avatar
    width: Theme.ThemeConfig.avatarSizeMedium
    height: Theme.ThemeConfig.avatarSizeMedium
    radius: width / 2
    color: generateColor(name)

    property string name: ""
    property string imageSource: ""
    property int size: Theme.ThemeConfig.avatarSizeMedium

    // Override default width/height based on size property
    Component.onCompleted: {
        if (size !== Theme.ThemeConfig.avatarSizeMedium) {
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
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: avatar.width
                height: avatar.height
                radius: avatar.radius
            }
        }
    }

    Text {
        anchors.centerIn: parent
        text: initials(name)
        color: "white"
        font.pixelSize: Math.max(10, avatar.width * 0.38)
        font.weight: Theme.ThemeConfig.fontWeightBold
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
        color: isOnline ? Theme.ThemeConfig.onlineColor : Theme.ThemeConfig.offlineColor
        border.color: Theme.ThemeConfig.surfaceColor
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
        if (!str || str.length === 0) return Theme.ThemeConfig.primaryColor
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
