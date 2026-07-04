import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Rectangle {
    id: titleBar
    height: Theme.titleBarHeight
    color: Theme.titleBarBackground
    z: 100

    property bool isMaximized: false

    signal minimizeClicked()
    signal maximizeClicked()
    signal closeClicked()

    // Drag to move window — Win32 native API (ReleaseCapture + WM_NCLBUTTONDOWN)
    MouseArea {
        anchors.fill: parent
        onPressed: {
            if (typeof windowHelper !== "undefined") {
                windowHelper.startSystemDrag(titleBar.Window.window)
            }
        }
    }

    Row {
        anchors {
            left: parent.left
            leftMargin: Theme.spacingSmall
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.spacingTiny

        Text {
            text: "Project-S"
            color: Theme.textOnPrimary
            font.pixelSize: Theme.fontSizeSmall
            font.weight: Theme.fontWeightMedium
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        anchors {
            right: parent.right
            rightMargin: 0
            verticalCenter: parent.verticalCenter
        }

        // ── Minimize ────────────────────────────────────────────
        TitleBarButton {
            iconType: "minimize"
            onClicked: titleBar.minimizeClicked()
        }

        // ── Maximize / Restore ──────────────────────────────────
        TitleBarButton {
            iconType: titleBar.isMaximized ? "restore" : "maximize"
            onClicked: titleBar.maximizeClicked()
        }

        // ── Close ───────────────────────────────────────────────
        TitleBarButton {
            iconType: "close"
            isClose: true
            onClicked: titleBar.closeClicked()
        }
    }

    // ── TitleBarButton component with drawn icons ─────────────────
    component TitleBarButton: Rectangle {
        property string iconType: ""     // "minimize" | "maximize" | "restore" | "close"
        property bool isClose: false

        width: 46
        height: Theme.titleBarHeight
        color: mouseArea.containsMouse
               ? (isClose ? Theme.errorColor : Qt.rgba(1, 1, 1, 0.1))
               : "transparent"

        signal clicked()

        // Drawn icon — crisp at any DPI, no font dependency
        Canvas {
            anchors.centerIn: parent
            width: 12; height: 12
            scale: 1.0
            contextType: "2d"
            antialiasing: true

            onPaint: {
                var ctx = getContext("2d")
                ctx.reset()
                ctx.strokeStyle = Theme.textOnPrimary
                ctx.fillStyle = Theme.textOnPrimary
                ctx.lineWidth = 1.2
                ctx.lineCap = "round"
                ctx.lineJoin = "round"

                // Center offset for 12×12 canvas
                var s = 11  // drawable size
                var m = 0.5 // margin
                var d = 2.0 // restore offset

                if (iconType === "minimize") {
                    // Horizontal line at bottom
                    ctx.beginPath()
                    ctx.moveTo(m + 1.5, s - 3)
                    ctx.lineTo(s - 1.5, s - 3)
                    ctx.stroke()
                } else if (iconType === "maximize") {
                    // Square outline
                    ctx.strokeRect(m + 1, m + 1, s - 3, s - 3)
                } else if (iconType === "restore") {
                    // Background clone (lower-right)
                    ctx.fillStyle = Theme.titleBarBackground
                    ctx.fillRect(m + d + 1, m - d + 1, s - 3, s - 3)
                    ctx.strokeStyle = Theme.textOnPrimary
                    ctx.strokeRect(m + d + 1, m - d + 1, s - 3, s - 3)
                    // Foreground clone (upper-left)
                    ctx.fillStyle = Theme.titleBarBackground
                    ctx.fillRect(m - d + 1, m + d + 1, s - 3, s - 3)
                    ctx.strokeRect(m - d + 1, m + d + 1, s - 3, s - 3)
                } else if (iconType === "close") {
                    // X shape — two diagonal lines
                    ctx.beginPath()
                    ctx.moveTo(m + 1.5, m + 1.5)
                    ctx.lineTo(s - 1.5, s - 1.5)
                    ctx.stroke()
                    ctx.beginPath()
                    ctx.moveTo(s - 1.5, m + 1.5)
                    ctx.lineTo(m + 1.5, s - 1.5)
                    ctx.stroke()
                }
            }

            Connections {
                target: parent
                function onIconTypeChanged() { parent.repaint?.() }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: parent.clicked()
        }
    }
}
