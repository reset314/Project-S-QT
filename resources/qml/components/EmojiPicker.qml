import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../theme/ThemeConfig.qml" as Theme

Popup {
    id: emojiPicker
    width: 320
    height: 260
    padding: Theme.ThemeConfig.spacingSmall

    signal emojiSelected(string emoji)

    // Common emoji grid
    property var emojiList: [
        "😀", "😃", "😄", "😁", "😅", "😂", "🤣", "😊",
        "😇", "🙂", "😉", "😌", "😍", "🥰", "😘", "😗",
        "😋", "😛", "😜", "🤪", "😝", "🤑", "🤗", "🤭",
        "🤔", "🤐", "😐", "😑", "😶", "😏", "😒", "🙄",
        "😬", "🤥", "😪", "😴", "🥱", "😷", "🤒", "🤕",
        "👍", "👎", "👏", "🙌", "🤝", "💪", "✌️", "🤞",
        "❤️", "🧡", "💛", "💚", "💙", "💜", "🖤", "🤍",
        "🎉", "🎊", "🎂", "🔥", "⭐", "💯", "✅", "❌"
    ]

    background: Rectangle {
        color: Theme.ThemeConfig.surfaceColor
        border.color: Theme.ThemeConfig.dividerColor
        border.width: 1
        radius: Theme.ThemeConfig.cardRadius

        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0; verticalOffset: 4
            radius: 12
            samples: 17
            color: "#20000000"
        }
    }

    contentItem: ColumnLayout {
        spacing: 0

        // Search/filter
        TextField {
            id: searchField
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            placeholderText: qsTr("Search emoji...")
            font.pixelSize: Theme.ThemeConfig.fontSizeSmall
        }

        // Tabs
        Row {
            Layout.fillWidth: true
            spacing: 0

            Repeater {
                model: ["😀", "👍", "❤️", "⭐"]

                Rectangle {
                    width: 40; height: 32
                    color: emojiTabBar.currentIndex === index
                           ? Theme.ThemeConfig.sidebarSelected
                           : "transparent"
                    radius: 4

                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: emojiTabBar.currentIndex = index
                    }
                }
            }
        }

        // Emoji grid
        property int currentTab: emojiTabBar.currentIndex

        GridView {
            id: emojiGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: 36
            cellHeight: 36
            clip: true
            model: filteredEmojis()
            delegate: emojiDelegate
        }
    }

    component EmojiTabBar: Item {
        id: emojiTabBar
        property int currentIndex: 0
    }

    EmojiTabBar { id: emojiTabBar }

    component EmojiDelegate: Item {
        width: 36; height: 36

        Text {
            anchors.centerIn: parent
            text: modelData
            font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                emojiPicker.emojiSelected(modelData)
                emojiPicker.close()
            }
        }
    }

    Component {
        id: emojiDelegate
        Item {
            width: 36; height: 36

            Text {
                anchors.centerIn: parent
                text: modelData
                font.pixelSize: Theme.ThemeConfig.fontSizeSubheading
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    emojiPicker.emojiSelected(modelData)
                    emojiPicker.close()
                }
            }
        }
    }

    function filteredEmojis() {
        var q = searchField.text.toLowerCase().trim()
        if (q.length === 0) return emojiList

        // Simple filtering — real implementation would have emoji name mapping
        return emojiList.filter(function(emoji) {
            return emoji.indexOf(q) >= 0
        })
    }
}
