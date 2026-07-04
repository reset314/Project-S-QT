import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: ttsVoicesPage
    anchors.fill: parent

    property string selectedVoice: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ──────────────────────────────────────────────
        C.BackHeader {
            title: qsTr("TTS Voices")
            onBackClicked: closePage()
        }

        // ── Voice list ──────────────────────────────────────────
        ListView {
            id: voiceList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 1

            // Placeholder model — loaded from C++ TTSRepository
            model: ListModel {
                id: voicesModel
                ListElement { voiceId: "default"; name: "Default Voice"; provider: "System"; language: "Auto" }
            }

            section.property: "provider"
            section.delegate: Rectangle {
                width: voiceList.width
                height: 32
                color: Theme.backgroundColor

                Text {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.spacingLarge
                        verticalCenter: parent.verticalCenter
                    }
                    text: section
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeCaption
                    font.weight: Theme.fontWeightBold
                }
            }

            delegate: Rectangle {
                width: voiceList.width
                height: 56
                color: delegateMouse.containsMouse
                       ? Theme.sidebarHover : Theme.surfaceColor

                RowLayout {
                    anchors {
                        fill: parent
                        leftMargin: Theme.spacingLarge
                        rightMargin: Theme.spacingMedium
                    }
                    spacing: Theme.spacingMedium

                    // Radio indicator
                    Rectangle {
                        Layout.preferredWidth: 20; Layout.preferredHeight: 20
                        radius: 10
                        border.color: selectedVoice === model.voiceId
                                      ? Theme.primaryColor
                                      : Theme.dividerColor
                        border.width: 2
                        color: selectedVoice === model.voiceId
                               ? Theme.primaryColor : "transparent"

                        Rectangle {
                            anchors.centerIn: parent
                            width: 8; height: 8; radius: 4
                            color: "white"
                            visible: selectedVoice === model.voiceId
                        }
                    }

                    // Voice info
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        Text {
                            text: model.name || ""
                            color: Theme.textPrimary
                            font.pixelSize: Theme.fontSizeBody
                            font.weight: selectedVoice === model.voiceId
                                         ? Theme.fontWeightMedium
                                         : Theme.fontWeightNormal
                        }

                        Text {
                            text: {
                                var parts = []
                                if (model.language) parts.push(model.language)
                                if (model.gender) parts.push(model.gender)
                                return parts.join(" · ")
                            }
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSizeSmall
                        }
                    }

                    // Preview button
                    Rectangle {
                        Layout.preferredWidth: 32; Layout.preferredHeight: 32
                        radius: 16
                        color: previewMouse.containsMouse
                               ? Theme.primaryLight : Theme.primaryColor

                        Text {
                            anchors.centerIn: parent
                            text: "▶"
                            color: "white"
                            font.pixelSize: Theme.fontSizeSmall
                        }

                        MouseArea {
                            id: previewMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: previewVoice(model.voiceId)
                        }
                    }
                }

                MouseArea {
                    id: delegateMouse
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: selectVoice(model.voiceId)
                }

                // Divider
                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                        leftMargin: 68
                    }
                    height: 1
                    color: Theme.dividerColor
                }
            }

            // Empty state
            Rectangle {
                visible: voiceList.count === 0
                anchors.fill: parent
                color: "transparent"

                Text {
                    anchors.centerIn: parent
                    text: qsTr("No voices available")
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeBody
                }
            }
        }

        // ── TTS Speed Slider ─────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            color: Theme.surfaceColor
            border.color: Theme.dividerColor
            border.width: 1

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.spacingLarge
                    rightMargin: Theme.spacingLarge
                }
                spacing: Theme.spacingMedium

                Text {
                    text: qsTr("Speed")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeBody
                }

                Slider {
                    id: speedSlider
                    Layout.fillWidth: true
                    from: 0.5
                    to: 2.0
                    value: typeof settingsViewModel !== "undefined"
                           ? settingsViewModel.ttsSpeed : 1.0
                    stepSize: 0.1

                    onValueChanged: {
                        if (typeof settingsViewModel !== "undefined") {
                            settingsViewModel.ttsSpeed = value
                        }
                    }
                }

                Text {
                    text: speedSlider.value.toFixed(1) + "x"
                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSizeSmall
                    Layout.preferredWidth: 36
                }
            }
        }
    }

    // ── Actions ──────────────────────────────────────────────────
    function selectVoice(voiceId) {
        selectedVoice = voiceId
        if (typeof settingsViewModel !== "undefined") {
            settingsViewModel.ttsVoice = voiceId
        }
    }

    function previewVoice(voiceId) {
        console.log("Preview voice:", voiceId)
        // C++ integration: ttsService.previewVoice(voiceId)
    }

    Component.onCompleted: {
        loadVoices()
        if (typeof settingsViewModel !== "undefined") {
            selectedVoice = settingsViewModel.ttsVoice
        }
    }

    function loadVoices() {
        if (typeof ttsRepo !== "undefined") {
            ttsRepo.getVoices(function(success, data) {
                if (success && data) {
                    voicesModel.clear()
                    try {
                        var voices = JSON.parse(data)
                        for (var i = 0; i < voices.length; i++) {
                            voicesModel.append(voices[i])
                        }
                    } catch (e) {
                        console.error("Failed to parse voices:", e)
                    }
                }
            })
        }
    }

    function closePage() {
        var sv = findStackView()
        if (sv) sv.pop()
    }

    function findStackView() {
        var obj = ttsVoicesPage.parent
        while (obj) {
            if (obj instanceof StackView) return obj
            obj = obj.parent
        }
        return null
    }
}
