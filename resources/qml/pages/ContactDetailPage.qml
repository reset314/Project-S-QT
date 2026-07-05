import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: page; anchors.fill: parent; property string aiUserId: ""
    property bool editing: false

    ColumnLayout {
        anchors.fill: parent; spacing: 0
        C.BackHeader { title: qsTr("Contact Details"); onBackClicked: closeDetail() }

        ScrollView {
            id: scroll; Layout.fillWidth: true; Layout.fillHeight: true; clip: true
            ColumnLayout {
                width: scroll.availableWidth; spacing: Theme.spacingMedium

                // Avatar + name
                RowLayout { Layout.fillWidth: true; Layout.margins: Theme.spacingLarge; spacing: Theme.spacingMedium
                    C.UserAvatar { Layout.preferredWidth: 64; Layout.preferredHeight: 64; name: detailName; imageSource: detailAvatar }
                    ColumnLayout { spacing: 2
                        Text { text: detailName || qsTr("Unknown"); font.pixelSize: Theme.fontSizeTitle; font.weight: Font.Bold; color: Theme.textPrimary }
                        Text { text: detailDescription || ""; font.pixelSize: Theme.fontSizeBody; color: Theme.textSecondary; visible: text !== "" }
                    }
                }

                // Edit / Save toggle
                RowLayout { Layout.fillWidth: true; Layout.margins: Theme.spacingLarge
                    Item { Layout.fillWidth: true }
                    Button { text: editing ? qsTr("Save") : qsTr("Edit")
                        onClicked: { if (editing) saveChanges(); editing = !editing } }
                    Button { visible: editing; text: qsTr("Cancel"); onClicked: { editing = false; reloadData() } }
                }

                // ── Fields ────────────────────────────────────────────
                Repeater {
                    model: fieldModel
                    delegate: FieldRow { label: model.label; value: model.val; isEdit: editing; fieldKey: model.key
                        onFieldValueChanged: function(k, v) { fieldValues[k] = v }
                    }
                }

                // View Memories button
                Button { Layout.margins: Theme.spacingLarge; text: qsTr("View Memories");
                    onClicked: { memoryViewer.aiUserId = aiUserId; memoryViewer.loadMemories(); memoryViewer.open() } }
            }
        }
    }

    // ── Field data ────────────────────────────────────────────────
    property var fieldValues: ({})
    property string detailName: ""; property string detailDescription: ""; property string detailAvatar: ""
    property string detailLlmProvider: ""; property string detailLlmModel: ""
    property string detailImageProvider: ""; property string detailImageModel: ""
    property string detailMultiProvider: ""; property string detailMultiModel: ""
    property string detailTranscribeProvider: ""; property string detailTranscribeModel: ""
    property string detailUnderstandProvider: ""; property string detailUnderstandModel: ""
    property string detailSystemPrompt: ""; property string detailTtsId: ""
    property bool detailDisplayThink: true; property bool detailDisplayEmotion: true
    property string detailCreated: ""; property string detailUpdated: ""

    ListModel { id: fieldModel }

    function buildFieldModel() {
        fieldModel.clear()
        var items = [
            {key:"avatar", label:qsTr("Avatar URL"), val:detailAvatar},
            {key:"name", label:qsTr("Name"), val:detailName},
            {key:"description", label:qsTr("Description"), val:detailDescription},
            {key:"llm_provider", label:qsTr("LLM Provider"), val:detailLlmProvider},
            {key:"llm_model", label:qsTr("LLM Model"), val:detailLlmModel},
            {key:"image_provider", label:qsTr("Image Provider"), val:detailImageProvider},
            {key:"image_model", label:qsTr("Image Model"), val:detailImageModel},
            {key:"multi_provider", label:qsTr("Multimodal Provider"), val:detailMultiProvider},
            {key:"multi_model", label:qsTr("Multimodal Model"), val:detailMultiModel},
            {key:"transcribe_provider", label:qsTr("Transcribe Provider"), val:detailTranscribeProvider},
            {key:"transcribe_model", label:qsTr("Transcribe Model"), val:detailTranscribeModel},
            {key:"understand_provider", label:qsTr("Understand Provider"), val:detailUnderstandProvider},
            {key:"understand_model", label:qsTr("Understand Model"), val:detailUnderstandModel},
            {key:"system_prompt", label:qsTr("System Prompt"), val:detailSystemPrompt},
            {key:"tts_id", label:qsTr("TTS Voice"), val:detailTtsId},
            {key:"is_display_think", label:qsTr("Display Think"), val:detailDisplayThink ? "true" : "false"},
            {key:"is_display_emotion", label:qsTr("Display Emotion"), val:detailDisplayEmotion ? "true" : "false"},
            {key:"created", label:qsTr("Created"), val:detailCreated},
            {key:"updated", label:qsTr("Updated"), val:detailUpdated},
        ]
        for (var i = 0; i < items.length; i++)
            fieldModel.append(items[i])
    }

    component FieldRow: Rectangle {
        property string label: ""; property string value: ""; property bool isEdit: false; property string fieldKey: ""
        signal fieldValueChanged(string key, string val)
        Layout.fillWidth: true; Layout.preferredHeight: rowCol.implicitHeight + 16; Layout.margins: Theme.spacingLarge
        radius: Theme.cardRadius; color: Theme.surfaceColor; border.color: Theme.dividerColor; border.width: 1
        ColumnLayout {
            id: rowCol; anchors.left: parent.left; anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; anchors.margins: 12; spacing: 4
            Text { text: label; font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
            Text { visible: !isEdit; text: value || qsTr("N/A"); font.pixelSize: Theme.fontSizeBody; color: Theme.textPrimary; Layout.fillWidth: true; wrapMode: Text.Wrap }
            TextField { visible: isEdit; text: value; font.pixelSize: Theme.fontSizeBody; Layout.fillWidth: true
                onTextChanged: fieldValueChanged(fieldKey, text) }
        }
    }

    function reloadData() { lookupContact() }
    function saveChanges() {
        if (typeof aiUserRepo === "undefined") return
        var result = aiUserRepo.updateAIUserJson(aiUserId, fieldValues)
        if (result && !result.error) {
            editing = false; lookupContact()
        }
    }

    function lookupContact() {
        console.warn("ContactDetail: aiUserRepo=" + (typeof aiUserRepo) + " aiUserId=" + aiUserId)
        if (typeof aiUserRepo === "undefined" || !aiUserId) return
        var json = aiUserRepo.getAIUserJson(aiUserId)
        console.warn("ContactDetail: getAIUserJson returned " + JSON.stringify(json))
        if (!json || json.error) return
        detailName = json.name || ""; detailDescription = json.description || ""; detailAvatar = json.avatar || ""
        detailLlmProvider = json.llm_provider || ""; detailLlmModel = json.llm_model || ""
        detailImageProvider = json.llm_image_provider || ""; detailImageModel = json.llm_image_model || ""
        detailMultiProvider = json.llm_multimodal_provider || ""; detailMultiModel = json.llm_multimodal_model || ""
        detailTranscribeProvider = json.llm_audio_transcribe_provider || ""; detailTranscribeModel = json.llm_audio_transcribe_model || ""
        detailUnderstandProvider = json.llm_audio_understanding_provider || ""; detailUnderstandModel = json.llm_audio_understanding_model || ""
        detailSystemPrompt = json.system_prompt || ""; detailTtsId = json.tts_id || ""
        detailCreated = json.created_at || ""; detailUpdated = json.updated_at || ""
        var cc = json.chat_config || {}
        detailDisplayThink = cc.is_display_think !== undefined ? cc.is_display_think : true
        detailDisplayEmotion = cc.is_display_emotion !== undefined ? cc.is_display_emotion : true
        buildFieldModel()
    }
    Component.onCompleted: { lookupContact() }

    function closeDetail() { var sv = findStackView(); if (sv) sv.pop() }
    function findStackView() { var o = page.parent; while (o) { if (o instanceof StackView) return o; o = o.parent } return null }

    C.MemoryViewer { id: memoryViewer }
}
