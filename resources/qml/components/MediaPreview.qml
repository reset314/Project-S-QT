import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: mediaPreview
    width: 280
    height: mediaColumn.implicitHeight + Theme.spacingSmall

    property string mediaType: ""     // "image" | "video" | "audio" | "file"
    property string mediaUrl: ""
    property string mediaName: ""
    property int fileSize: 0          // bytes

    signal clicked()

    Column {
        id: mediaColumn
        width: parent.width
        spacing: Theme.spacingTiny

        // Image preview
        Loader {
            active: mediaType === "image"
            visible: mediaType === "image"
            width: parent.width
            sourceComponent: imagePreviewComponent
        }

        // Video preview
        Loader {
            active: mediaType === "video"
            visible: mediaType === "video"
            width: parent.width
            sourceComponent: videoPreviewComponent
        }

        // Audio player placeholder
        Loader {
            active: mediaType === "audio"
            visible: mediaType === "audio"
            width: parent.width
            sourceComponent: audioPreviewComponent
        }

        // File attachment
        Loader {
            active: mediaType === "file"
            visible: mediaType === "file"
            width: parent.width
            sourceComponent: filePreviewComponent
        }
    }

    // ── Image preview ───────────────────────────────────────────
    Component {
        id: imagePreviewComponent
        Rectangle {
            width: Math.min(280, imageLoader.implicitWidth)
            height: Math.min(200, imageLoader.implicitHeight)
            radius: Theme.bubbleRadius
            color: Theme.dividerColor
            clip: true

            Image {
                id: imageLoader
                anchors.fill: parent
                source: mediaPreview.mediaUrl
                fillMode: Image.PreserveAspectFit
                asynchronous: true

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: mediaPreview.clicked()
                }
            }

            // Loading placeholder
            Rectangle {
                anchors.fill: parent
                color: Theme.dividerColor
                visible: imageLoader.status === Image.Loading

                Text {
                    anchors.centerIn: parent
                    text: qsTr("Loading...")
                    color: Theme.textHint
                    font.pixelSize: Theme.fontSizeSmall
                }
            }
        }
    }

    // ── Video preview ───────────────────────────────────────────
    Component {
        id: videoPreviewComponent
        Rectangle {
            width: 280
            height: 160
            radius: Theme.bubbleRadius
            color: "#1A1A1A"

            Text {
                anchors.centerIn: parent
                text: "▶ " + qsTr("Play Video")
                color: "white"
                font.pixelSize: Theme.fontSizeBody
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: mediaPreview.clicked()
            }
        }
    }

    // ── Audio preview ───────────────────────────────────────────
    Component {
        id: audioPreviewComponent
        Rectangle {
            width: 240
            height: 48
            radius: Theme.bubbleRadius
            color: Theme.aiBubbleColor
            border.color: Theme.dividerColor

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.spacingMedium
                    rightMargin: Theme.spacingMedium
                }
                spacing: Theme.spacingMedium

                // Play button
                Rectangle {
                    Layout.preferredWidth: 32; Layout.preferredHeight: 32
                    radius: 16
                    color: Theme.primaryColor

                    Text {
                        anchors.centerIn: parent
                        text: "▶"
                        color: "white"
                        font.pixelSize: Theme.fontSizeSubheading
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: mediaPreview.clicked()
                    }
                }

                Text {
                    text: mediaPreview.mediaName || qsTr("Audio message")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeSmall
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
            }
        }
    }

    // ── File preview ────────────────────────────────────────────
    Component {
        id: filePreviewComponent
        Rectangle {
            width: 240
            height: 56
            radius: Theme.bubbleRadius
            color: Theme.aiBubbleColor
            border.color: Theme.dividerColor
            border.width: 1

            RowLayout {
                anchors {
                    fill: parent
                    leftMargin: Theme.spacingMedium
                    rightMargin: Theme.spacingMedium
                }
                spacing: Theme.spacingMedium

                Rectangle {
                    Layout.preferredWidth: 36; Layout.preferredHeight: 36
                    radius: 4
                    color: Theme.primaryLight

                    Text {
                        anchors.centerIn: parent
                        text: "📄"
                        font.pixelSize: Theme.fontSizeHeading
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: mediaPreview.mediaName || qsTr("File attachment")
                        color: Theme.textPrimary
                        font.pixelSize: Theme.fontSizeSmall
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    Text {
                        text: formatFileSize(mediaPreview.fileSize)
                        color: Theme.textHint
                        font.pixelSize: Theme.fontSizeCaption
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: mediaPreview.clicked()
            }
        }
    }

    // ── Helper ────────────────────────────────────────────────────
    function formatFileSize(bytes) {
        if (!bytes || bytes === 0) return ""
        if (bytes < 1024) return bytes + " B"
        if (bytes < 1048576) return (bytes / 1024).toFixed(1) + " KB"
        return (bytes / 1048576).toFixed(1) + " MB"
    }
}
