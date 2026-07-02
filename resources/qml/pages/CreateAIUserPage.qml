import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme
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
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            color: Theme.ThemeConfig.surfaceColor
            border.color: Theme.ThemeConfig.dividerColor
            border.width: 1

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.ThemeConfig.spacingLarge
                    rightMargin: Theme.ThemeConfig.spacingMedium
                }
                spacing: Theme.ThemeConfig.spacingMedium

                // Back button
                Rectangle {
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    radius: 16
                    color: backMouse.containsMouse
                           ? Theme.ThemeConfig.sidebarHover : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "←"
                        font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                        color: Theme.ThemeConfig.textPrimary
                    }

                    MouseArea {
                        id: backMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: closePage()
                    }
                }

                Text {
                    text: qsTr("Create AI User")
                    color: Theme.ThemeConfig.textPrimary
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                    font.weight: Theme.ThemeConfig.fontWeightBold
                    Layout.fillWidth: true
                }
            }
        }

        // ── Form content ────────────────────────────────────────
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: Math.min(600, parent ? parent.width - 48 : 552)
                anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                spacing: Theme.ThemeConfig.spacingLarge

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingLarge }

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

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingMedium }

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

                FormField {
                    id: systemPromptField
                    label: qsTr("System Prompt")
                    placeholder: qsTr("Custom system prompt for this AI")
                    multiline: true
                }

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
                            leftMargin: Theme.ThemeConfig.spacingMedium
                            rightMargin: Theme.ThemeConfig.spacingMedium
                        }
                        text: errorMessage
                        color: Theme.ThemeConfig.errorColor
                        font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                        wrapMode: Text.Wrap
                    }
                }

                // Buttons
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.ThemeConfig.spacingMedium

                    Item { Layout.fillWidth: true }

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 40
                        radius: Theme.ThemeConfig.buttonRadius
                        color: cancelMouse.containsMouse
                               ? Theme.ThemeConfig.sidebarHover : "transparent"
                        border.color: Theme.ThemeConfig.dividerColor
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Cancel")
                            color: Theme.ThemeConfig.textSecondary
                            font.pixelSize: Theme.ThemeConfig.fontSizeBody
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
                        radius: Theme.ThemeConfig.buttonRadius
                        color: canSave
                               ? (saveMouse.containsMouse
                                  ? Theme.ThemeConfig.primaryLight
                                  : Theme.ThemeConfig.primaryColor)
                               : Theme.ThemeConfig.dividerColor

                        property bool canSave: nameField.text.trim().length > 0 && !saving

                        Text {
                            anchors.centerIn: parent
                            text: saving ? qsTr("Creating...") : qsTr("Create")
                            color: canSave ? Theme.ThemeConfig.textOnPrimary : Theme.ThemeConfig.textSecondary
                            font.pixelSize: Theme.ThemeConfig.fontSizeBody
                            font.weight: Theme.ThemeConfig.fontWeightMedium
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

                Item { Layout.preferredHeight: Theme.ThemeConfig.spacingXLarge }
            }
        }
    }

    // ── Internal components ──────────────────────────────────────
    component SectionHeader: Text {
        Layout.fillWidth: true
        text: parent ? parent.text : ""
        color: Theme.ThemeConfig.primaryColor
        font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
        font.weight: Theme.ThemeConfig.fontWeightBold
    }

    component FormField: ColumnLayout {
        property string label: ""
        property string placeholder: ""
        property bool multiline: false
        property bool required: false
        property alias text: textField.text

        Layout.fillWidth: true
        spacing: Theme.ThemeConfig.spacingTiny

        Text {
            text: formFieldLabel
            color: Theme.ThemeConfig.textPrimary
            font.pixelSize: Theme.ThemeConfig.fontSizeSmall
            font.weight: Theme.ThemeConfig.fontWeightMedium
        }

        property string formFieldLabel: label + (required ? " *" : "")

        TextField {
            id: textField
            Layout.fillWidth: true
            Layout.preferredHeight: multiline ? 80 : 40
            placeholderText: placeholder
            font.pixelSize: Theme.ThemeConfig.fontSizeBody

            background: Rectangle {
                radius: Theme.ThemeConfig.buttonRadius
                color: Theme.ThemeConfig.backgroundColor
                border.color: textField.activeFocus
                              ? Theme.ThemeConfig.primaryColor
                              : Theme.ThemeConfig.dividerColor
                border.width: textField.activeFocus ? 2 : 1
            }

            // Use TextArea-like behavior for multiline
            wrapMode: multiline ? Text.Wrap : Text.NoWrap
            verticalAlignment: multiline ? Text.AlignTop : Text.AlignVCenter
            inputMethodHints: multiline ? Qt.ImhMultiLine : Qt.ImhNone
        }
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
        if (systemPromptField.text.trim()) payload.system_prompt = systemPromptField.text.trim()

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
