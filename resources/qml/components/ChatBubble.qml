import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: chatBubble
    width: parent ? parent.width : 400
    height: bubbleColumn.implicitHeight + Theme.spacingMedium

    property string senderType: "user"     // "user" | "ai" | "system"
    property string msgType: "text"        // "text" | "image" | "video" | "file" | "audio"
    property var contentJson: ({})         // Parsed JSON: {think, emotion, response, functions}
    property string timestamp: ""
    property bool isComplete: true

    // Computed properties
    property bool isUser: senderType === "user"
    property bool isAi: senderType === "ai"
    property bool isSystem: senderType === "system"
    property string responseText: {
        if (!contentJson) return ""
        if (typeof contentJson === "string") return contentJson
        if (contentJson.response && contentJson.response !== "") return contentJson.response
        // Fallback: server might store plain text in content
        return ""
    }
    property string thinkText: contentJson && contentJson.think ? contentJson.think : ""
    property string emotionText: contentJson && contentJson.emotion ? contentJson.emotion : ""
    property bool hasThinkContent: thinkText.length > 0

    readonly property real maxBubbleWidth: chatBubble.width * 0.78

    ColumnLayout {
        id: bubbleColumn
        spacing: Theme.spacingTiny

        // ── System message ─────────────────────────────────────
        Loader {
            active: chatBubble.isSystem
            visible: chatBubble.isSystem
            Layout.fillWidth: true
            sourceComponent: systemBubbleComponent
        }

        // ── User/AI message ────────────────────────────────────
        Loader {
            active: !chatBubble.isSystem
            visible: !chatBubble.isSystem
            Layout.fillWidth: true
            sourceComponent: chatBubble.isUser ? userBubbleComponent : aiBubbleComponent
        }
    }

    // ── System bubble template ────────────────────────────────────
    Component {
        id: systemBubbleComponent
        Rectangle {
            width: chatBubble.width
            height: sysLabel.implicitHeight + 8
            color: Theme.systemBubbleColor

            Text {
                id: sysLabel
                anchors.centerIn: parent
                text: chatBubble.responseText || chatBubble.thinkText
                color: Theme.systemBubbleText
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
            }
        }
    }

    // ── User bubble template ──────────────────────────────────────
    Component {
        id: userBubbleComponent
        Item {
            width: chatBubble.width
            height: userContent.height + Theme.spacingTiny

            // Pin to right
            Rectangle {
                id: userBubble
                anchors.right: parent.right
                anchors.rightMargin: Theme.spacingLarge
                width: Math.min(userContent.implicitWidth + 24, chatBubble.maxBubbleWidth)
                height: userContent.implicitHeight + Theme.spacingMedium + 2
                radius: Theme.bubbleRadius
                color: Theme.userBubbleColor

                Text {
                    id: userContent
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        leftMargin: Theme.spacingMedium
                        rightMargin: Theme.spacingMedium
                        topMargin: Theme.spacingSmall
                    }
                    text: chatBubble.responseText
                    color: Theme.userBubbleText
                    font.pixelSize: Theme.fontSizeBody
                    wrapMode: Text.Wrap
                    textFormat: Text.PlainText
                    linkColor: Theme.accentLight
                }
            }

            // Timestamp
            Text {
                anchors {
                    right: userBubble.right
                    top: userBubble.bottom
                    topMargin: 2
                }
                text: formatTime(chatBubble.timestamp)
                color: Theme.textHint
                font.pixelSize: Theme.fontSizeCaption
            }

            // Sending indicator
            Rectangle {
                visible: !chatBubble.isComplete
                anchors {
                    right: userBubble.left
                    rightMargin: 4
                    verticalCenter: userBubble.verticalCenter
                }
                width: 12; height: 12; radius: 6
                color: Theme.textHint

                RotationAnimation on rotation {
                    running: !chatBubble.isComplete
                    from: 0; to: 360
                    duration: 1000
                    loops: Animation.Infinite
                }
            }
        }
    }

    // ── AI bubble template ────────────────────────────────────────
    Component {
        id: aiBubbleComponent
        Item {
            width: chatBubble.width
            height: aiContentCol.implicitHeight + Theme.spacingTiny

            Column {
                id: aiContentCol
                anchors.left: parent.left
                anchors.leftMargin: Theme.spacingLarge
                width: Math.min(implicitWidth, chatBubble.maxBubbleWidth)
                spacing: Theme.spacingTiny

                // Emotion indicator
                Rectangle {
                    visible: chatBubble.emotionText.length > 0
                    width: emotionLabel.implicitWidth + 8
                    height: 18
                    radius: 9
                    color: Theme.sidebarHover

                    Text {
                        id: emotionLabel
                        anchors.centerIn: parent
                        text: chatBubble.emotionText
                        font.pixelSize: Theme.fontSizeCaption
                        color: Theme.textSecondary
                    }
                }

                // Think (collapsible)
                Loader {
                    active: chatBubble.hasThinkContent
                    visible: chatBubble.hasThinkContent
                    sourceComponent: thinkBubbleComponent
                }

                // Response text
                Rectangle {
                    id: aiBubble
                    width: Math.max(60, aiResponseText.implicitWidth + 24)
                    height: aiResponseText.implicitHeight + Theme.spacingMedium + 2
                    radius: Theme.bubbleRadius
                    color: Theme.aiBubbleColor
                    border.color: Theme.dividerColor
                    border.width: 1

                    Text {
                        id: aiResponseText
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            leftMargin: Theme.spacingMedium
                            rightMargin: Theme.spacingMedium
                            topMargin: Theme.spacingSmall
                        }
                        text: chatBubble.responseText
                        color: Theme.aiBubbleText
                        font.pixelSize: Theme.fontSizeBody
                        wrapMode: Text.Wrap
                        textFormat: Text.PlainText
                        linkColor: Theme.primaryColor
                    }
                }

                // Timestamp
                Text {
                    anchors.left: aiBubble.left
                    text: formatTime(chatBubble.timestamp)
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeCaption
                }
            }
        }
    }

    // ── Think bubble (collapsible) ────────────────────────────────
    Component {
        id: thinkBubbleComponent
        Item {
            width: Math.min(thinkContent.implicitWidth + 16, chatBubble.maxBubbleWidth)
            height: thinkCol.implicitHeight + 4

            property bool expanded: false

            Column {
                id: thinkCol
                width: parent.width
                spacing: 2

                // Toggle header
                Rectangle {
                    width: thinkHeaderText.implicitWidth + 12
                    height: 22
                    radius: 4
                    color: expanded ? Theme.sidebarHover : "transparent"

                    Text {
                        id: thinkHeaderText
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 6
                        text: expanded ? qsTr("▼ Thinking") : qsTr("▶ Thinking")
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.textSecondary
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: parent.parent.parent.expanded = !parent.parent.parent.expanded
                    }
                }

                // Think content (visible when expanded)
                Rectangle {
                    visible: expanded
                    width: Math.min(thinkContent.implicitWidth + 16, chatBubble.maxBubbleWidth - 32)
                    height: thinkContent.implicitHeight + 8
                    radius: 6
                    color: "#F5F5DC" // cream / beige

                    Text {
                        id: thinkContent
                        anchors {
                            left: parent.left
                            top: parent.top
                            leftMargin: 8
                            topMargin: 4
                            right: parent.right
                            rightMargin: 8
                        }
                        text: chatBubble.thinkText
                        color: Theme.textSecondary
                        font.pixelSize: Theme.fontSizeSmall
                        wrapMode: Text.Wrap
                        textFormat: Text.PlainText
                    }
                }
            }
        }
    }

    // ── Helper ────────────────────────────────────────────────────
    function formatTime(isoStr) {
        if (!isoStr || isoStr.length < 16) return ""
        // Extract HH:mm from ISO 8601
        var t = isoStr.substring(11, 16)
        return t
    }
}
