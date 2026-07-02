pragma Singleton
import QtQuick

/// Global application state singleton exposed to QML.
///
/// Provides reactive bindings for login status, connection status,
/// and other cross-cutting application concerns without needing
/// C++ context property lookups everywhere.
///
/// Updated from C++ via the "appState" context property (a plain
/// QObject registered in main.cpp).
QtObject {
    id: root

    /// Whether the user is currently authenticated.
    property bool loggedIn: false

    /// Current WebSocket / network connection status.
    /// "connected" | "disconnected" | "connecting" | "error"
    property string connectionStatus: "disconnected"

    /// Display name of the currently logged-in user.
    property string currentUsername: ""

    /// Number of total unread messages across all conversations.
    property int totalUnread: 0

    /// Whether the application window is currently visible.
    property bool windowVisible: true

    /// Platform identifier: "windows" | "macos" | "linux" | "unknown".
    readonly property string platform: {
        if (Qt.platform.os === "windows") return "windows";
        if (Qt.platform.os === "osx")     return "macos";
        if (Qt.platform.os === "linux")   return "linux";
        return "unknown";
    }

    /// Whether the current platform supports system tray.
    readonly property bool supportsTray: Qt.platform.os !== "wasm"
}
