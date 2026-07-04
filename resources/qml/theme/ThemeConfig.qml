import QtQuick 2.15

QtObject {
    // ── Color Palette ──────────────────────────────────────────────
    readonly property color primaryColor: "#1976D2"
    readonly property color primaryLight: "#63A4FF"
    readonly property color primaryDark: "#004BA0"
    readonly property color accentColor: "#FF6F00"
    readonly property color accentLight: "#FFA040"
    readonly property color accentDark: "#C43E00"

    readonly property color backgroundColor: "#F5F5F5"
    readonly property color surfaceColor: "#FFFFFF"
    readonly property color cardColor: "#FFFFFF"
    readonly property color dividerColor: "#E0E0E0"

    readonly property color textPrimary: "#212121"
    readonly property color textSecondary: "#757575"
    readonly property color textHint: "#BDBDBD"
    readonly property color textOnPrimary: "#FFFFFF"
    readonly property color textOnAccent: "#FFFFFF"

    readonly property color errorColor: "#D32F2F"
    readonly property color warningColor: "#F57C00"
    readonly property color successColor: "#388E3C"
    readonly property color infoColor: "#1976D2"

    readonly property color userBubbleColor: "#1976D2"
    readonly property color userBubbleText: "#FFFFFF"
    readonly property color aiBubbleColor: "#FFFFFF"
    readonly property color aiBubbleText: "#212121"
    readonly property color systemBubbleColor: "#FFF3E0"
    readonly property color systemBubbleText: "#E65100"

    readonly property color unreadBadgeColor: "#F44336"
    readonly property color unreadBadgeText: "#FFFFFF"
    readonly property color onlineColor: "#4CAF50"
    readonly property color offlineColor: "#BDBDBD"

    readonly property color sidebarBackground: "#FAFAFA"
    readonly property color sidebarHover: "#EEEEEE"
    readonly property color sidebarSelected: "#E3F2FD"
    readonly property color titleBarBackground: "#1565C0"

    // ── Font Sizes ─────────────────────────────────────────────────
    readonly property int fontSizeCaption: 11
    readonly property int fontSizeSmall: 12
    readonly property int fontSizeBody: 14
    readonly property int fontSizeSubheading: 16
    readonly property int fontSizeHeading: 18
    readonly property int fontSizeTitle: 20
    readonly property int fontSizeDisplay: 24

    // ── Font Weights ──────────────────────────────────────────────
    readonly property int fontWeightLight: Font.Light
    readonly property int fontWeightNormal: Font.Normal
    readonly property int fontWeightMedium: Font.Medium
    readonly property int fontWeightBold: Font.Bold

    // ── Spacing ────────────────────────────────────────────────────
    readonly property int spacingTiny: 4
    readonly property int spacingSmall: 8
    readonly property int spacingMedium: 12
    readonly property int spacingLarge: 16
    readonly property int spacingXLarge: 24

    // ── Sizing ─────────────────────────────────────────────────────
    readonly property int sidebarWidth: 300
    readonly property int detailPanelWidth: 320
    readonly property int titleBarHeight: 32
    readonly property int statusBarHeight: 24
    readonly property int chatInputMinHeight: 48
    readonly property int chatInputMaxHeight: 120

    readonly property int avatarSizeSmall: 32
    readonly property int avatarSizeMedium: 40
    readonly property int avatarSizeLarge: 56
    readonly property int avatarSizeXLarge: 80

    readonly property int iconSizeSmall: 16
    readonly property int iconSizeMedium: 20
    readonly property int iconSizeLarge: 24

    readonly property int bubbleRadius: 12
    readonly property int cardRadius: 8
    readonly property int buttonRadius: 4

    // ── Animation ──────────────────────────────────────────────────
    readonly property int animationFast: 150
    readonly property int animationNormal: 250
    readonly property int animationSlow: 400

    // ── Shadows ────────────────────────────────────────────────────
    readonly property int shadowElevationLow: 2
    readonly property int shadowElevationMedium: 4
    readonly property int shadowElevationHigh: 8
}
