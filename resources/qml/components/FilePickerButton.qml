import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: filePicker
    width: 40
    height: 40

    property string filter: "All files (*.*)"
    property bool pickMultiple: false
    property string selectedFilePath: ""

    signal fileSelected(string filePath)
    signal filesSelected(var filePaths)

    Rectangle {
        anchors.fill: parent
        radius: Theme.buttonRadius
        color: mouseArea.containsMouse
               ? Theme.sidebarHover : "transparent"

        Text {
            anchors.centerIn: parent
            text: "📎"
            font.pixelSize: Theme.fontSizeTitle
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: openFileDialog()
        }
    }

    function openFileDialog() {
        // Use C++ helper through context if available
        var paths = []
        if (typeof fileDialogHelper !== "undefined") {
            paths = fileDialogHelper.openFiles(filter, pickMultiple)
        } else {
            // Fallback: C++ provides QFileDialog integration
            console.warn("FilePickerButton: fileDialogHelper not available, use C++ bridge")
        }

        if (paths.length > 0) {
            selectedFilePath = paths[0]
            fileSelected(paths[0])
            if (pickMultiple) filesSelected(paths)
        }
    }
}
