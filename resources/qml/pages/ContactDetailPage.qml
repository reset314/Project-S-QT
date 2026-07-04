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
                    model: fields
                    delegate: FieldRow { label: model.label; value: model.value; isEdit: editing; fieldKey: model.key
                        onFieldValueChanged: function(k, v) { fieldValues[k] = v }
                    }
                }

                // View Memories button
                Button { Layout.margins: Theme.spacingLarge; text: qsTr("View Memories");
                    onClicked: { /* Memory viewer — Task 12 */ } }
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

    property var fields: [
        {key:"avatar", label:qsTr("Avatar URL"), value:detailAvatar},
        {key:"name", label:qsTr("Name"), value:detailName},
        {key:"description", label:qsTr("Description"), value:detailDescription},
        {key:"llm_provider", label:qsTr("LLM Provider"), value:detailLlmProvider},
        {key:"llm_model", label:qsTr("LLM Model"), value:detailLlmModel},
        {key:"image_provider", label:qsTr("Image Provider"), value:detailImageProvider},
        {key:"image_model", label:qsTr("Image Model"), value:detailImageModel},
        {key:"multi_provider", label:qsTr("Multimodal Provider"), value:detailMultiProvider},
        {key:"multi_model", label:qsTr("Multimodal Model"), value:detailMultiModel},
        {key:"transcribe_provider", label:qsTr("Transcribe Provider"), value:detailTranscribeProvider},
        {key:"transcribe_model", label:qsTr("Transcribe Model"), value:detailTranscribeModel},
        {key:"understand_provider", label:qsTr("Understand Provider"), value:detailUnderstandProvider},
        {key:"understand_model", label:qsTr("Understand Model"), value:detailUnderstandModel},
        {key:"system_prompt", label:qsTr("System Prompt"), value:detailSystemPrompt},
        {key:"tts_id", label:qsTr("TTS Voice"), value:detailTtsId},
        {key:"created", label:qsTr("Created"), value:detailCreated},
        {key:"updated", label:qsTr("Updated"), value:detailUpdated},
    ]
    property var chatConfigFields: [
        {key:"is_display_think", label:qsTr("Display Think"), value:detailDisplayThink},
        {key:"is_display_emotion", label:qsTr("Display Emotion"), value:detailDisplayEmotion}
    ]

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
        var data = fieldValues; data["id"] = aiUserId
        aiUserRepo.updateAIUser(aiUserId, data)
        editing = false
    }

    function lookupContact() {
        if (typeof contactListModel === "undefined" || !aiUserId) return
        for (var i = 0; i < contactListModel.rowCount(); i++) {
            var idx = contactListModel.index(i, 0)
            if (contactListModel.data(idx, 257) === aiUserId) {
                detailName = contactListModel.data(idx, 258) || ""
                detailDescription = contactListModel.data(idx, 259) || ""
                detailAvatar = contactListModel.data(idx, 260) || ""
                detailLlmProvider = contactListModel.data(idx, 262) || ""
                detailLlmModel = contactListModel.data(idx, 263) || ""
                detailCreated = contactListModel.data(idx, 264) || ""
                return
            }
        }
    }
    Component.onCompleted: { lookupContact() }

    function closeDetail() { var sv = findStackView(); if (sv) sv.pop() }
    function findStackView() { var o = page.parent; while (o) { if (o instanceof StackView) return o; o = o.parent } return null }
}
