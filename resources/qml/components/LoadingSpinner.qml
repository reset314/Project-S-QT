import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: spinner
    width: 40
    height: 40

    property bool running: false
    property color spinnerColor: Theme.primaryColor
    property int spinnerSize: 40

    Component.onCompleted: {
        if (spinnerSize !== 40) {
            spinner.width = spinnerSize
            spinner.height = spinnerSize
        }
    }

    // Animated circle
    Canvas {
        id: canvas
        anchors.fill: parent
        visible: spinner.running
        rotation: anim.running ? anim.angle : 0

        property real angle: 0

        onPaint: {
            var ctx = getContext("2d")
            var center = width / 2
            var radius = (Math.min(width, height) / 2) - 2
            var lineWidth = Math.max(2, radius * 0.2)

            ctx.clearRect(0, 0, width, height)
            ctx.lineWidth = lineWidth
            ctx.strokeStyle = Theme.dividerColor
            ctx.beginPath()
            ctx.arc(center, center, radius, 0, Math.PI * 2)
            ctx.stroke()

            var startAngle = (anim.angle - 120) * Math.PI / 180
            var endAngle = anim.angle * Math.PI / 180
            ctx.strokeStyle = spinner.spinnerColor
            ctx.beginPath()
            ctx.arc(center, center, radius, startAngle, endAngle)
            ctx.stroke()
        }

        RotationAnimator {
            id: anim
            target: canvas
            property real angle: 0
            from: 0; to: 360
            duration: 1000
            loops: Animation.Infinite
            running: spinner.running
            onRunningChanged: {
                if (running) canvas.angle = 0
            }
        }

        // Redraw on each frame
        Timer {
            interval: 16
            running: spinner.running
            repeat: true
            onTriggered: canvas.requestPaint()
        }
    }
}
