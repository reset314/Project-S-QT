import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import "../components" as C

Item {
    id: themePage
    ColumnLayout {
        anchors.fill: parent; spacing: 0
        C.BackHeader { title: qsTr("Theme"); onBackClicked: closeWindow() }

        ColumnLayout {
            Layout.fillWidth: true; Layout.margins: Theme.spacingLarge; spacing: Theme.spacingMedium

            Text { text: qsTr("Theme Mode"); color: Theme.textPrimary; font.pixelSize: Theme.fontSizeBody; font.weight: Font.Bold }

            ComboBox {
                id: themeCombo
                Layout.fillWidth: true
                model: [qsTr("Light"), qsTr("Dark"), qsTr("Glass"), qsTr("Custom")]
                currentIndex: {
                    var path = typeof themeManager !== "undefined" ? themeManager.themeUrl : ""
                    if (path.includes("dark")) return 1
                    if (path.includes("glass")) return 2
                    if (path.includes("light")) return 0
                    return 3
                }
                onActivated: function(selIndex) {
                    var urls = ["qrc:/themes/light.css", "qrc:/themes/dark.css", "qrc:/themes/glass.css"]
                    if (selIndex < 3 && typeof themeManager !== "undefined") {
                        themeManager.setTheme(urls[selIndex])
                    }
                }
            }

            // ── Confirm & Apply ────────────────────────────────────
            Button {
                text: qsTr("Apply & Close")
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                onClicked: {
                    if (typeof themeManager !== "undefined") {
                        themeManager.reload()
                    }
                    closeWindow()
                }
            }

            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Theme.dividerColor }

            Text { text: qsTr("Import Custom Theme"); color: Theme.textPrimary; font.pixelSize: Theme.fontSizeBody; font.weight: Font.Bold }

            Button {
                text: qsTr("Select CSS File")
                onClicked: fileDialog.open()
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Import CSS Theme")
        nameFilters: ["CSS Files (*.css)"]
        onAccepted: {
            var src = fileUrl.toString()
            if (typeof themeManager !== "undefined") {
                themeManager.setTheme(src)
            }
        }
    }

    function closeWindow() {
        if (themePage.Window && themePage.Window.window) {
            themePage.Window.window.close()
        }
    }
}
