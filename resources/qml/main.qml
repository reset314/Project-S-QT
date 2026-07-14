import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "shell"
import "pages"
import "dialogs"

ApplicationWindow {
    id: root
    width: 1280
    height: 800
    visible: true
    title: "Project-S"
    color: Theme.backgroundColor
    minimumWidth: 800
    minimumHeight: 500

    // Frameless window flags
    flags: Qt.Window | Qt.FramelessWindowHint

    // ── Main Layout ──────────────────────────────────────────────
    Column {
        anchors.fill: parent
        spacing: 0

        // Custom title bar
        TitleBar {
            id: titleBar
            width: parent.width
            isMaximized: root.visibility === Window.Maximized

            onMinimizeClicked: root.showMinimized()
            onMaximizeClicked: {
                if (root.visibility === Window.Maximized) {
                    root.showNormal()
                } else {
                    root.showMaximized()
                }
            }
            onCloseClicked: root.close()
        }

        // ── Content area ─────────────────────────────────────────
        Item {
            width: parent.width
            height: parent.height - titleBar.height /*- statusBar.height*/

            // Login page or MainShell (switched after auth)
            Loader {
                id: contentLoader
                anchors.fill: parent
                sourceComponent: loginShellComponent

                // After login, switch to main shell
                function showMainShell() {
                    contentLoader.sourceComponent = mainShellComponent
                }

                function showLogin() {
                    contentLoader.sourceComponent = loginShellComponent
                }
            }
        }

        // Status bar
        // StatusBar {
        //     id: statusBar
        //     width: parent.width
        // }
    }

    // ── Component cache ──────────────────────────────────────────
    Component {
        id: loginShellComponent
        Item {
            anchors.fill: parent
            LoginPage {
                anchors.centerIn: parent
                onLoginSuccess: contentLoader.showMainShell()
            }
        }
    }

    Component {
        id: mainShellComponent
        MainShell {
            anchors.fill: parent
        }
    }

    // ── C++ Connections ──────────────────────────────────────────
    Connections {
        target: typeof loginViewModel !== "undefined" ? loginViewModel : null
        enabled: typeof loginViewModel !== "undefined"
        function onLoginSuccess() {
            contentLoader.showMainShell()
        }
    }

    Connections {
        target: typeof chatService !== "undefined" ? chatService : null
        enabled: typeof chatService !== "undefined"
        function onStreamInitReceived(aiUserId, conversationId, messageId) {
            // Handled in ChatPage
        }
        function onStreamChunkReceived(aiUserId, content) {
            // Handled in ChatPage
        }
        function onStreamDoneReceived(aiUserId, messageId, content) {
            // Handled in ChatPage
        }
        function onStreamErrorReceived(aiUserId, code, message) {
            console.error("Stream error for", aiUserId, code, message)
        }
    }

    // ── Window resize edges ──────────────────────────────────────
    // Frameless window uses OS-native system resize for proper behavior
    MouseArea {
        anchors { left: parent.left; top: parent.top; bottom: parent.bottom; topMargin: 8; bottomMargin: 8 }
        width: 6
        cursorShape: Qt.SizeHorCursor
        onPressed: root.startSystemResize(Qt.LeftEdge)
    }
    MouseArea {
        anchors { right: parent.right; top: parent.top; bottom: parent.bottom; topMargin: 8; bottomMargin: 8 }
        width: 6
        cursorShape: Qt.SizeHorCursor
        onPressed: root.startSystemResize(Qt.RightEdge)
    }
    MouseArea {
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right; leftMargin: 8; rightMargin: 8 }
        height: 6
        cursorShape: Qt.SizeVerCursor
        onPressed: root.startSystemResize(Qt.BottomEdge)
    }
    MouseArea {
        anchors { top: parent.top; left: parent.left; right: parent.right; leftMargin: 8; rightMargin: 8 }
        height: 6
        cursorShape: Qt.SizeVerCursor
        onPressed: root.startSystemResize(Qt.TopEdge)
    }
    // Corners
    MouseArea {
        anchors { right: parent.right; bottom: parent.bottom }
        width: 12; height: 12
        cursorShape: Qt.SizeFDiagCursor
        onPressed: root.startSystemResize(Qt.RightEdge | Qt.BottomEdge)
    }
    MouseArea {
        anchors { left: parent.left; bottom: parent.bottom }
        width: 12; height: 12
        cursorShape: Qt.SizeBDiagCursor
        onPressed: root.startSystemResize(Qt.LeftEdge | Qt.BottomEdge)
    }
    MouseArea {
        anchors { left: parent.left; top: parent.top }
        width: 12; height: 12
        cursorShape: Qt.SizeFDiagCursor
        onPressed: root.startSystemResize(Qt.LeftEdge | Qt.TopEdge)
    }
    MouseArea {
        anchors { right: parent.right; top: parent.top }
        width: 12; height: 12
        cursorShape: Qt.SizeBDiagCursor
        onPressed: root.startSystemResize(Qt.RightEdge | Qt.TopEdge)
    }

}
