import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme
import "../components" as C

Item {
    id: chatPage
    anchors.fill: parent

    property string aiUserId: ""
    property string aiUserName: ""
    property string conversationId: ""
    property bool isAiTyping: false

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Chat Header ────────────────────────────────────────
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
                        onClicked: closeChat()
                    }
                }

                C.UserAvatar {
                    Layout.preferredWidth: Theme.ThemeConfig.avatarSizeSmall
                    Layout.preferredHeight: Theme.ThemeConfig.avatarSizeSmall
                    name: chatPage.aiUserName
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0

                    Text {
                        text: chatPage.aiUserName || qsTr("Chat")
                        color: Theme.ThemeConfig.textPrimary
                        font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
                        font.weight: Theme.ThemeConfig.fontWeightMedium
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    Text {
                        text: chatPage.isAiTyping ? qsTr("typing...") : qsTr("Online")
                        color: chatPage.isAiTyping
                               ? Theme.ThemeConfig.textSecondary
                               : Theme.ThemeConfig.onlineColor
                        font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                    }
                }
            }
        }

        // ── Messages List ──────────────────────────────────────
        ListView {
            id: messageList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 4
            verticalLayoutDirection: ListView.BottomToTop

            model: typeof chatMessagesModel !== "undefined" ? chatMessagesModel : null

            delegate: Loader {
                width: messageList.width
                sourceComponent: {
                    var msgType = model.msgType || "text"
                    switch (msgType) {
                        case "image":
                        case "video":
                        case "audio":
                        case "file":
                            return mediaMessageDelegate
                        default:
                            return chatBubbleDelegate
                    }
                }
            }

            // Auto-scroll to bottom on new messages
            property bool autoScroll: true

            onCountChanged: {
                if (autoScroll) {
                    positionViewAtBeginning()
                }
            }

            // Detect manual scroll to disable auto-scroll
            onContentYChanged: {
                if (contentY < originY - 60) {
                    autoScroll = false
                } else if (contentY >= originY - 10) {
                    autoScroll = true
                }
            }

            // Scroll-to-bottom FAB
            Rectangle {
                visible: !messageList.autoScroll && messageList.count > 0
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    bottomMargin: Theme.ThemeConfig.spacingLarge
                    rightMargin: Theme.ThemeConfig.spacingLarge
                }
                width: 40; height: 40; radius: 20
                color: Theme.ThemeConfig.primaryColor

                Text {
                    anchors.centerIn: parent
                    text: "↓"
                    color: "white"
                    font.pixelSize: Theme.ThemeConfig.fontSizeHeading
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        messageList.autoScroll = true
                        messageList.positionViewAtBeginning()
                    }
                }
            }

            // Empty state
            Rectangle {
                visible: messageList.count === 0
                anchors.fill: parent
                color: "transparent"
                z: -1

                Text {
                    anchors.centerIn: parent
                    text: qsTr("Start a conversation!")
                    color: Theme.ThemeConfig.textHint
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                }
            }
        }

        // ── Typing indicator ────────────────────────────────────
        C.TypingIndicator {
            Layout.fillWidth: true
            typing: chatPage.isAiTyping
            anchors.left: parent.left
            anchors.leftMargin: Theme.ThemeConfig.spacingLarge
        }

        // ── Message Input ──────────────────────────────────────
        C.MessageInput {
            id: messageInputBar
            Layout.fillWidth: true

            onSendMessage: function(text) {
                sendTextMessage(text)
            }

            onAttachFile: {
                // Trigger file picker via C++ bridge
                if (typeof chatService !== "undefined") {
                    // C++ ChatService handles file dialog
                }
            }
        }
    }

    // ── Delegate components ──────────────────────────────────────
    Component {
        id: chatBubbleDelegate
        C.ChatBubble {
            senderType: model.senderType || "ai"
            msgType: model.msgType || "text"
            contentJson: model.content || {}
            timestamp: model.timestamp || ""
            isComplete: model.isComplete !== undefined ? model.isComplete : true
        }
    }

    Component {
        id: mediaMessageDelegate
        Column {
            width: parent.width
            spacing: Theme.ThemeConfig.spacingSmall

            // Text content (if any)
            C.ChatBubble {
                width: parent.width
                senderType: model.senderType || "ai"
                msgType: "text"
                contentJson: {
                    if (model.content && model.content.response) {
                        return model.content
                    }
                    return { response: "" }
                }
                timestamp: model.timestamp || ""
                isComplete: model.isComplete !== undefined ? model.isComplete : true
                visible: model.content && model.content.response
            }

            // Media preview
            C.MediaPreview {
                anchors {
                    left: parent.left
                    leftMargin: model.senderType === "user"
                               ? parent.width - width - Theme.ThemeConfig.spacingLarge
                               : Theme.ThemeConfig.spacingLarge
                }
                mediaType: model.msgType || "file"
                mediaUrl: model.mediaList && model.mediaList.length > 0
                         ? model.mediaList[0].url || "" : ""
                mediaName: model.mediaList && model.mediaList.length > 0
                          ? model.mediaList[0].filename || "" : ""
            }
        }
    }

    // ── Functions ────────────────────────────────────────────────
    function sendTextMessage(text) {
        if (typeof chatService !== "undefined") {
            chatService.sendMessage(chatPage.aiUserId, text, chatPage.conversationId)
        } else {
            console.log("ChatService not available, message:", text)
        }
    }

    function closeChat() {
        if (typeof unreadTracker !== "undefined") {
            unreadTracker.setActiveAiUser("")
        }
        // Pop from StackView if inside one
        var sv = findStackView()
        if (sv) sv.pop()
    }

    function findStackView() {
        var obj = chatPage.parent
        while (obj) {
            if (obj.objectName === "stackView" || obj instanceof StackView) {
                return obj
            }
            obj = obj.parent
        }
        return null
    }

    // ── C++ connections ─────────────────────────────────────────
    Connections {
        target: typeof chatService !== "undefined" ? chatService : null
        enabled: typeof chatService !== "undefined"

        function onStreamInitReceived(aiUserId, conversationId, messageId) {
            if (aiUserId === chatPage.aiUserId) {
                chatPage.isAiTyping = true
                if (conversationId && !chatPage.conversationId) {
                    chatPage.conversationId = conversationId
                }
            }
        }

        function onStreamChunkReceived(aiUserId, content) {
            if (aiUserId === chatPage.aiUserId) {
                chatPage.isAiTyping = true
            }
        }

        function onStreamDoneReceived(aiUserId, messageId, content) {
            if (aiUserId === chatPage.aiUserId) {
                chatPage.isAiTyping = false
            }
        }

        function onStreamErrorReceived(aiUserId, code, message) {
            if (aiUserId === chatPage.aiUserId) {
                chatPage.isAiTyping = false
            }
        }
    }

    // Set active AI user for unread tracking
    Component.onCompleted: {
        if (typeof unreadTracker !== "undefined" && chatPage.aiUserId) {
            unreadTracker.setActiveAiUser(chatPage.aiUserId)
        }
    }

    Component.onDestruction: {
        if (typeof unreadTracker !== "undefined") {
            unreadTracker.setActiveAiUser("")
        }
    }
}
