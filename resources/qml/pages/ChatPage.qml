import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components" as C

Item {
    id: chatPage
    objectName: "chatPage"

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
            color: Theme.surfaceColor
            border.color: Theme.dividerColor
            border.width: 1

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.spacingLarge
                    rightMargin: Theme.spacingMedium
                }
                spacing: Theme.spacingMedium

                // Back button
                Rectangle {
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    radius: 16
                    color: backMouse.containsMouse
                           ? Theme.sidebarHover : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "←"
                        font.pixelSize: Theme.fontSizeHeading
                        color: Theme.textPrimary
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
                    Layout.preferredWidth: Theme.avatarSizeSmall
                    Layout.preferredHeight: Theme.avatarSizeSmall
                    name: chatPage.aiUserName
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0

                    Text {
                        text: chatPage.aiUserName || qsTr("Chat")
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSizeSubheading
                        font.weight: Theme.fontWeightMedium
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    Text {
                        text: chatPage.isAiTyping ? qsTr("typing...") : qsTr("Online")
                        color: chatPage.isAiTyping
                               ? Theme.textSecondary
                               : Theme.onlineColor
                        font.pixelSize: Theme.fontSizeCaption
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

            model: typeof chatMessagesModel !== "undefined" ? chatMessagesModel : null

            delegate: C.ChatBubble {
                width: messageList.width
                senderType: model.senderType || "ai"
                msgType: model.msgType || "text"
                contentJson: model.content || {}
                timestamp: model.timestamp || ""
                isComplete: model.isComplete !== undefined ? model.isComplete : true
            }

            // ── Scroll logic ──────────────────────────────────────
            property bool autoScroll: true

            // History loaded → scroll to bottom (via C++ signal)
            function onHistoryLoaded() {
                autoScroll = true
                Qt.callLater(function() {
                    messageList.positionViewAtEnd()
                })
            }

            onCountChanged: {
                if (autoScroll) {
                    Qt.callLater(function() {
                        messageList.positionViewAtEnd()
                    })
                }
            }

            // Detect manual scroll → disable auto-scroll
            onMovementStarted: {
                autoScroll = atYEnd
            }

            function scrollToBottom() {
                autoScroll = true
                Qt.callLater(function() {
                    messageList.positionViewAtEnd()
                })
            }

            Connections {
                target: typeof chatMessagesModel !== "undefined" ? chatMessagesModel : null
                function onHistoryLoaded() {
                    messageList.onHistoryLoaded()
                }
            }

            // ── Pull to top → load older messages ──────────────────
            property bool _loadingOlder: false

            onContentYChanged: {
                if (contentY < 80 && !_loadingOlder && count > 0) {
                    _loadingOlder = true
                    loadOlderMessages()
                }
            }

            function loadOlderMessages() {
                if (typeof chatMessagesModel !== "undefined") {
                    chatMessagesModel.loadOlder()
                }
            }

            Connections {
                target: typeof chatMessagesModel !== "undefined" ? chatMessagesModel : null
                function onOlderMessagesLoaded(prependCount) {
                    messageList._loadingOlder = false
                }
            }

            // Scroll-to-bottom FAB
            Rectangle {
                visible: messageList.count > 0 && !messageList.atYEnd
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    bottomMargin: Theme.spacingLarge
                    rightMargin: Theme.spacingLarge
                }
                width: 40; height: 40; radius: 20
                color: Theme.primaryColor

                Text {
                    anchors.centerIn: parent
                    text: "↓"
                    color: "white"
                    font.pixelSize: Theme.fontSizeHeading
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: messageList.scrollToBottom()
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
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeBody
                }
            }
        }

        // ── Typing indicator ────────────────────────────────────
        C.TypingIndicator {
            Layout.fillWidth: true
            Layout.leftMargin: Theme.spacingLarge
            typing: chatPage.isAiTyping
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

    // ── Functions ────────────────────────────────────────────────
    function sendTextMessage(text) {
        if (typeof chatService !== "undefined") {
            chatService.sendMessage(chatPage.aiUserId, text, chatPage.conversationId)
            messageList.autoScroll = true
            Qt.callLater(function() {
                messageList.positionViewAtEnd()
            })
        } else {
            console.warn("ChatService not available")
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
                messageList.autoScroll = true
                Qt.callLater(function() { messageList.positionViewAtEnd() })
            }
        }

        function onStreamChunkReceived(aiUserId, content) {
            if (aiUserId === chatPage.aiUserId) {
                chatPage.isAiTyping = true
                if (messageList.autoScroll) {
                    Qt.callLater(function() { messageList.positionViewAtEnd() })
                }
            }
        }

        function onStreamDoneReceived(aiUserId, messageId, content) {
            if (aiUserId === chatPage.aiUserId) {
                chatPage.isAiTyping = false
                messageList.autoScroll = true
                Qt.callLater(function() { messageList.positionViewAtEnd() })
            }
        }

        function onStreamErrorReceived(aiUserId, code, message) {
            if (aiUserId === chatPage.aiUserId) {
                chatPage.isAiTyping = false
            }
        }
    }

    // Set active AI user for unread tracking + trigger history load
    Component.onCompleted: {
        if (chatPage.aiUserId) {
            if (typeof unreadTracker !== "undefined") {
                unreadTracker.setActiveAiUser(chatPage.aiUserId)
            }
            // Trigger history load via ChatMessagesModel
            if (typeof chatMessagesModel !== "undefined") {
                chatMessagesModel.setActiveAiUserId(chatPage.aiUserId)
            }
        }
    }

    Component.onDestruction: {
        if (typeof unreadTracker !== "undefined") {
            unreadTracker.setActiveAiUser("")
        }
    }
}
