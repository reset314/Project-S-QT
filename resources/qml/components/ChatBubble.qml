import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Item {
    id: chatBubble
    width: parent ? parent.width : 400
    height: bubbleColumn.implicitHeight + Theme.ThemeConfig.spacingMedium

    property string senderType: "user"     // "user" | "ai" | "system"
    property string msgType: "text"        // "text" | "image" | "video" | "file" | "audio"
    property var contentJson: ({})         // Parsed JSON: {think, emotion, response, functions}
    property string timestamp: ""
    property bool isComplete: true

    // Computed properties
    property bool isUser: senderType === "user"
    property bool isAi: senderType === "ai"
    property bool isSystem: senderType === "system"
    property string responseText: contentJson && contentJson.response ? contentJson.response : ""
    property string thinkText: contentJson && contentJson.think ? contentJson.think : ""
    property string emotionText: contentJson && contentJson.emotion ? contentJson.emotion : ""
    property bool hasThinkContent: thinkText.length > 0

    readonly property real maxBubbleWidth: chatBubble.width * 0.78

    ColumnLayout {
        id: bubbleColumn
        spacing: Theme.ThemeConfig.spacingTiny

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
            color: Theme.ThemeConfig.systemBubbleColor

            Text {
                id: sysLabel
                anchors.centerIn: parent
                text: chatBubble.responseText || chatBubble.thinkText
                color: Theme.ThemeConfig.systemBubbleText
                font.pixelSize: Theme.ThemeConfig.fontSizeSmall
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
            height: userContent.height + Theme.ThemeConfig.spacingTiny

            // Pin to right
            Rectangle {
                id: userBubble
                anchors.right: parent.right
                anchors.rightMargin: Theme.ThemeConfig.spacingLarge
                width: Math.min(userContent.implicitWidth + 24, chatBubble.maxBubbleWidth)
                height: userContent.implicitHeight + Theme.ThemeConfig.spacingMedium + 2
                radius: Theme.ThemeConfig.bubbleRadius
                color: Theme.ThemeConfig.userBubbleColor

                Text {
                    id: userContent
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        leftMargin: Theme.ThemeConfig.spacingMedium
                        rightMargin: Theme.ThemeConfig.spacingMedium
                        topMargin: Theme.ThemeConfig.spacingSmall
                    }
                    text: chatBubble.responseText
                    color: Theme.ThemeConfig.userBubbleText
                    font.pixelSize: Theme.ThemeConfig.fontSizeBody
                    wrapMode: Text.Wrap
                    textFormat: Text.PlainText
                    linkColor: Theme.ThemeConfig.accentLight
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
                color: Theme.ThemeConfig.textHint
                font.pixelSize: Theme.ThemeConfig.fontSizeCaption
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
                color: Theme.ThemeConfig.textHint

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
            height: aiContentCol.implicitHeight + Theme.ThemeConfig.spacingTiny

            Column {
                id: aiContentCol
                anchors.left: parent.left
                anchors.leftMargin: Theme.ThemeConfig.spacingLarge
                width: Math.min(implicitWidth, chatBubble.maxBubbleWidth)
                spacing: Theme.ThemeConfig.spacingTiny

                // Emotion indicator
                Rectangle {
                    visible: chatBubble.emotionText.length > 0
                    width: emotionLabel.implicitWidth + 8
                    height: 18
                    radius: 9
                    color: Theme.ThemeConfig.sidebarHover

                    Text {
                        id: emotionLabel
                        anchors.centerIn: parent
                        text: chatBubble.emotionText
                        font.pixelSize: Theme.ThemeConfig.fontSizeCaption
                        color: Theme.ThemeConfig.textSecondary
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
                    height: aiResponseText.implicitHeight + Theme.ThemeConfig.spacingMedium + 2
                    radius: Theme.ThemeConfig.bubbleRadius
                    color: Theme.ThemeConfig.aiBubbleColor
                    border.color: Theme.ThemeConfig.dividerColor
                    border.width: 1

                    Text {
                        id: aiResponseText
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            leftMargin: Theme.ThemeConfig.spacingMedium
                            rightMargin: Theme.ThemeConfig.spacingMedium
                            topMargin: Theme.ThemeConfig.spacingSmall
                        }
                        text: chatBubble.responseText
                        color: Theme.ThemeConfig.aiBubbleText
                        font.pixelSize: Theme.ThemeConfig.fontSizeBody
                        wrapMode: Text.Wrap
                        textFormat: Text.PlainText
                        linkColor: Theme.ThemeConfig.primaryColor
                    }
                }

                // Timestamp
                Text {
                    anchors.left: aiBubble.left
                    text: formatTime(chatBubble.timestamp)
                    color: Theme.ThemeConfig.textHint
                    font.pixelSize: Theme.ThemeConfig.fontSizeCaption
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
                    color: expanded ? Theme.ThemeConfig.sidebarHover : "transparent"

                    Text {
                        id: thinkHeaderText
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 6
                        text: expanded ? qsTr("▼ Thinking") : qsTr("▶ Thinking")
                        font.pixelSize: Theme.ThemeConfig.fontSizeSmall
                        color: Theme.ThemeConfig.textSecondary
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
                        color: Theme.ThemeConfig.textSecondary
                        font.pixelSize: Theme.ThemeConfig.fontSizeSmall
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
