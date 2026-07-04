import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: createAIUserPage
    anchors.fill: parent

    property bool saving: false
    property string errorMessage: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ──────────────────────────────────────────────
        C.BackHeader {
            title: qsTr("Create AI User")
            onBackClicked: closePage()
        }

        // ── Form content ────────────────────────────────────────
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: Math.min(600, parent ? parent.width - 48 : 552)
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                spacing: Theme.spacingLarge

                Item { Layout.preferredHeight: Theme.spacingLarge }

                // Basic Info Section
                SectionHeader { text: qsTr("Basic Information") }

                FormField {
                    id: nameField
                    label: qsTr("Name")
                    placeholder: qsTr("Enter AI user name")
                    required: true
                }

                FormField {
                    id: descField
                    label: qsTr("Description")
                    placeholder: qsTr("Brief description of this AI character")
                    multiline: true
                }

                FormField {
                    id: avatarField
                    label: qsTr("Avatar URL")
                    placeholder: qsTr("https://example.com/avatar.png")
                }

                Item { Layout.preferredHeight: Theme.spacingMedium }

                // LLM Configuration
                SectionHeader { text: qsTr("LLM Configuration") }

                FormField {
                    id: llmProviderField
                    label: qsTr("LLM Provider")
                    placeholder: qsTr("e.g. deepseek, siliconflow")
                }

                FormField {
                    id: llmModelField
                    label: qsTr("LLM Model")
                    placeholder: qsTr("e.g. deepseek-chat, Qwen/Qwen2.5-7B-Instruct")
                }

                FormField {
                    id: imageProviderField
                    label: qsTr("Image Generation Provider")
                    placeholder: qsTr("Optional")
                }

                FormField {
                    id: imageModelField
                    label: qsTr("Image Generation Model")
                    placeholder: qsTr("Optional")
                }

                FormField { id: multiProvField; label: qsTr("Multimodal Provider"); placeholder: qsTr("Optional") }
                FormField { id: multiModelField; label: qsTr("Multimodal Model"); placeholder: qsTr("Optional") }
                FormField { id: transProvField; label: qsTr("Transcribe Provider"); placeholder: qsTr("Optional") }
                FormField { id: transModelField; label: qsTr("Transcribe Model"); placeholder: qsTr("Optional") }
                FormField { id: underProvField; label: qsTr("Understand Provider"); placeholder: qsTr("Optional") }
                FormField { id: underModelField; label: qsTr("Understand Model"); placeholder: qsTr("Optional") }
                FormField { id: systemPromptField; label: qsTr("System Prompt"); placeholder: qsTr("Custom system prompt for this AI"); multiline: true }
                FormField { id: ttsField; label: qsTr("TTS Voice ID"); placeholder: qsTr("Optional") }

                SectionHeader { text: qsTr("Chat Config") }
                SwitchRow { id: thinkSwitch; label: qsTr("Display Think") }
                SwitchRow { id: emotionSwitch; label: qsTr("Display Emotion") }

                // Error message
                Rectangle {
                    Layout.fillWidth: true
                    visible: errorMessage.length > 0
                    height: errorLabel.implicitHeight + 12
                    radius: 4
                    color: "#FFEBEE"

                    Text {
                        id: errorLabel
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            leftMargin: Theme.spacingMedium
                            rightMargin: Theme.spacingMedium
                        }
                        text: errorMessage
                        color: Theme.errorColor
                        font.pixelSize: Theme.fontSizeSmall
                        wrapMode: Text.Wrap
                    }
                }

                // Buttons
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingMedium

                    Item { Layout.fillWidth: true }

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 40
                        radius: Theme.buttonRadius
                        color: cancelMouse.containsMouse
                               ? Theme.sidebarHover : "transparent"
                        border.color: Theme.dividerColor
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Cancel")
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSizeBody
                        }

                        MouseArea {
                            id: cancelMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: closePage()
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 40
                        radius: Theme.buttonRadius
                        color: canSave
                               ? (saveMouse.containsMouse
                                  ? Theme.primaryLight
                                  : Theme.primaryColor)
                               : Theme.dividerColor

                        property bool canSave: nameField.text.trim().length > 0 && !saving

                        Text {
                            anchors.centerIn: parent
                            text: saving ? qsTr("Creating...") : qsTr("Create")
                            color: canSave ? Theme.textOnPrimary : Theme.textSecondary
                            font.pixelSize: Theme.fontSizeBody
                            font.weight: Theme.fontWeightMedium
                        }

                        MouseArea {
                            id: saveMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            enabled: parent.canSave
                            onClicked: createAIUser()
                        }
                    }
                }

                Item { Layout.preferredHeight: Theme.spacingXLarge }
            }
        }
    }

    // ── Internal components ──────────────────────────────────────
    component SectionHeader: Text {
        Layout.fillWidth: true
        text: parent ? parent.text : ""
        color: Theme.primaryColor
        font.pixelSize: Theme.fontSizeSubheading
        font.weight: Theme.fontWeightBold
    }

    component FormField: ColumnLayout {
        property string label: ""
        property string placeholder: ""
        property bool multiline: false
        property bool required: false
        property alias text: textField.text

        Layout.fillWidth: true
        spacing: Theme.spacingTiny

        Text {
            text: formFieldLabel
            color: Theme.textPrimary
            font.pixelSize: Theme.fontSizeSmall
            font.weight: Theme.fontWeightMedium
        }

        property string formFieldLabel: label + (required ? " *" : "")

        TextField {
            id: textField
            Layout.fillWidth: true
            Layout.preferredHeight: multiline ? 80 : 40
            placeholderText: activeFocus ? "" : placeholder
            font.pixelSize: Theme.fontSizeBody

            background: Rectangle {
                radius: Theme.buttonRadius
                color: Theme.backgroundColor
                border.color: textField.activeFocus
                              ? Theme.primaryColor
                              : Theme.dividerColor
                border.width: textField.activeFocus ? 2 : 1
            }

            // Use TextArea-like behavior for multiline
            wrapMode: multiline ? Text.Wrap : Text.NoWrap
            verticalAlignment: multiline ? Text.AlignTop : Text.AlignVCenter
            inputMethodHints: multiline ? Qt.ImhMultiLine : Qt.ImhNone
        }
    }

    component SwitchRow: RowLayout {
        property string label: ""; property alias checked: sw.checked
        Layout.fillWidth: true; Layout.margins: Theme.spacingSmall
        Text { text: label; font.pixelSize: Theme.fontSizeBody; color: Theme.textPrimary; Layout.fillWidth: true }
        Switch { id: sw; checked: true }
    }

    // ── Actions ──────────────────────────────────────────────────
    function createAIUser() {
        if (!nameField.text.trim()) return

        saving = true
        errorMessage = ""

        // Build JSON payload
        var payload = {
            name: nameField.text.trim()
        }
        if (descField.text.trim()) payload.description = descField.text.trim()
        if (avatarField.text.trim()) payload.avatar = avatarField.text.trim()
        if (llmProviderField.text.trim()) payload.llm_provider = llmProviderField.text.trim()
        if (llmModelField.text.trim()) payload.llm_model = llmModelField.text.trim()
        if (imageProviderField.text.trim()) payload.llm_image_provider = imageProviderField.text.trim()
        if (imageModelField.text.trim()) payload.llm_image_model = imageModelField.text.trim()
        if (multiProvField.text.trim()) payload.llm_multimodal_provider = multiProvField.text.trim()
        if (multiModelField.text.trim()) payload.llm_multimodal_model = multiModelField.text.trim()
        if (transProvField.text.trim()) payload.llm_audio_transcribe_provider = transProvField.text.trim()
        if (transModelField.text.trim()) payload.llm_audio_transcribe_model = transModelField.text.trim()
        if (underProvField.text.trim()) payload.llm_audio_understanding_provider = underProvField.text.trim()
        if (underModelField.text.trim()) payload.llm_audio_understanding_model = underModelField.text.trim()
        if (systemPromptField.text.trim()) payload.system_prompt = systemPromptField.text.trim()
        if (ttsField.text.trim()) payload.tts_id = ttsField.text.trim()
        payload.chat_config = { is_display_think: thinkSwitch.checked, is_display_emotion: emotionSwitch.checked }

        console.log("Creating AI user:", JSON.stringify(payload))

        // C++ integration: call AIUserRepository via context property
        if (typeof aiUserRepo !== "undefined") {
            aiUserRepo.createAIUser(JSON.stringify(payload), function(success, result) {
                saving = false
                if (success) {
                    closePage()
                } else {
                    errorMessage = result || qsTr("Failed to create AI user")
                }
            })
        } else {
            // Standalone mode
            saving = false
            closePage()
        }
    }

    function closePage() {
        var sv = findStackView()
        if (sv) sv.pop()
    }

    function findStackView() {
        var obj = createAIUserPage.parent
        while (obj) {
            if (obj instanceof StackView) return obj
            obj = obj.parent
        }
        return null
    }
}
