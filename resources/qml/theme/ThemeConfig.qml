import QtQuick 2.15

Item {
    id: theme

    // ── Active palette index ─────────────────────────────────────
    // 0=light  1=dark  2=glass
    property int activePalette: 0

    // Detect palette from themeManager URL
    function syncFromUrl() {
        if (typeof themeManager === "undefined") return
        var url = themeManager.themeUrl || ""
        if (url.includes("dark"))  { activePalette = 1; return }
        if (url.includes("glass")) { activePalette = 2; return }
        activePalette = 0
    }

    Component.onCompleted: syncFromUrl()

    Connections {
        target: typeof themeManager !== "undefined" ? themeManager : null
        enabled: typeof themeManager !== "undefined"
        function onThemeChanged() { syncFromUrl() }
    }

    // ── Color Palette ────────────────────────────────────────────
    // Light palette [0], Dark palette [1], Glass palette [2]

    readonly property var palettes: [
        // ── 0: Light ──────────────────────────────────────────
        {
            primaryColor: "#1976D2", primaryLight: "#63A4FF", primaryDark: "#004BA0",
            accentColor: "#FF6F00", accentLight: "#FFA040", accentDark: "#C43E00",
            backgroundColor: "#F5F5F5", surfaceColor: "#FFFFFF", cardColor: "#FFFFFF",
            dividerColor: "#E0E0E0",
            textPrimary: "#212121", textSecondary: "#757575", textHint: "#BDBDBD",
            textOnPrimary: "#FFFFFF", textOnAccent: "#FFFFFF",
            errorColor: "#D32F2F", warningColor: "#F57C00", successColor: "#388E3C",
            infoColor: "#1976D2",
            userBubbleColor: "#1976D2", userBubbleText: "#FFFFFF",
            aiBubbleColor: "#FFFFFF", aiBubbleText: "#212121",
            systemBubbleColor: "#FFF3E0", systemBubbleText: "#E65100",
            unreadBadgeColor: "#F44336", unreadBadgeText: "#FFFFFF",
            onlineColor: "#4CAF50", offlineColor: "#BDBDBD",
            sidebarBackground: "#FAFAFA", sidebarHover: "#EEEEEE",
            sidebarSelected: "#E3F2FD", titleBarBackground: "#1565C0"
        },
        // ── 1: Dark ────────────────────────────────────────────
        {
            primaryColor: "#90CAF9", primaryLight: "#BBDEFB", primaryDark: "#42A5F5",
            accentColor: "#FFB74D", accentLight: "#FFCC80", accentDark: "#F57C00",
            backgroundColor: "#121212", surfaceColor: "#1E1E1E", cardColor: "#1E1E1E",
            dividerColor: "#333333",
            textPrimary: "#E0E0E0", textSecondary: "#9E9E9E", textHint: "#616161",
            textOnPrimary: "#121212", textOnAccent: "#121212",
            errorColor: "#EF5350", warningColor: "#FFA726", successColor: "#66BB6A",
            infoColor: "#90CAF9",
            userBubbleColor: "#1A73E8", userBubbleText: "#FFFFFF",
            aiBubbleColor: "#2A2A2A", aiBubbleText: "#E0E0E0",
            systemBubbleColor: "#3E2723", systemBubbleText: "#FFCC80",
            unreadBadgeColor: "#F44336", unreadBadgeText: "#FFFFFF",
            onlineColor: "#4CAF50", offlineColor: "#616161",
            sidebarBackground: "#181818", sidebarHover: "#2A2A2A",
            sidebarSelected: "#1E3A5F", titleBarBackground: "#0D0D0D"
        },
        // ── 2: Glass ────────────────────────────────────────────
        {
            primaryColor: "#90CAF9", primaryLight: "#BBDEFB", primaryDark: "#42A5F5",
            accentColor: "#FFB74D", accentLight: "#FFCC80", accentDark: "#F57C00",
            backgroundColor: "#0D1117", surfaceColor: "#1C2333", cardColor: "#1C2333",
            dividerColor: "#30363D",
            textPrimary: "#E6EDF3", textSecondary: "#8B949E", textHint: "#484F58",
            textOnPrimary: "#0D1117", textOnAccent: "#0D1117",
            errorColor: "#EF5350", warningColor: "#FFA726", successColor: "#66BB6A",
            infoColor: "#90CAF9",
            userBubbleColor: "#1F6FEB", userBubbleText: "#FFFFFF",
            aiBubbleColor: "#21262D", aiBubbleText: "#E6EDF3",
            systemBubbleColor: "#2E2416", systemBubbleText: "#FFB74D",
            unreadBadgeColor: "#F44336", unreadBadgeText: "#FFFFFF",
            onlineColor: "#4CAF50", offlineColor: "#484F58",
            sidebarBackground: "#161B22", sidebarHover: "#1C2333",
            sidebarSelected: "#1F3050", titleBarBackground: "#0D1117"
        }
    ]

    // ── Exposed color properties (bound to active palette) ─────
    property color primaryColor:        palettes[activePalette].primaryColor
    property color primaryLight:        palettes[activePalette].primaryLight
    property color primaryDark:         palettes[activePalette].primaryDark
    property color accentColor:         palettes[activePalette].accentColor
    property color accentLight:         palettes[activePalette].accentLight
    property color accentDark:          palettes[activePalette].accentDark
    property color backgroundColor:     palettes[activePalette].backgroundColor
    property color surfaceColor:        palettes[activePalette].surfaceColor
    property color cardColor:           palettes[activePalette].cardColor
    property color dividerColor:        palettes[activePalette].dividerColor
    property color textPrimary:         palettes[activePalette].textPrimary
    property color textSecondary:       palettes[activePalette].textSecondary
    property color textHint:            palettes[activePalette].textHint
    property color textOnPrimary:       palettes[activePalette].textOnPrimary
    property color textOnAccent:        palettes[activePalette].textOnAccent
    property color errorColor:          palettes[activePalette].errorColor
    property color warningColor:        palettes[activePalette].warningColor
    property color successColor:        palettes[activePalette].successColor
    property color infoColor:           palettes[activePalette].infoColor
    property color userBubbleColor:     palettes[activePalette].userBubbleColor
    property color userBubbleText:      palettes[activePalette].userBubbleText
    property color aiBubbleColor:       palettes[activePalette].aiBubbleColor
    property color aiBubbleText:        palettes[activePalette].aiBubbleText
    property color systemBubbleColor:   palettes[activePalette].systemBubbleColor
    property color systemBubbleText:    palettes[activePalette].systemBubbleText
    property color unreadBadgeColor:    palettes[activePalette].unreadBadgeColor
    property color unreadBadgeText:     palettes[activePalette].unreadBadgeText
    property color onlineColor:         palettes[activePalette].onlineColor
    property color offlineColor:        palettes[activePalette].offlineColor
    property color sidebarBackground:   palettes[activePalette].sidebarBackground
    property color sidebarHover:        palettes[activePalette].sidebarHover
    property color sidebarSelected:     palettes[activePalette].sidebarSelected
    property color titleBarBackground:  palettes[activePalette].titleBarBackground

    // ── Font (theme-independent) ─────────────────────────────────
    readonly property int fontSizeCaption: 11
    readonly property int fontSizeSmall: 12
    readonly property int fontSizeBody: 14
    readonly property int fontSizeSubheading: 16
    readonly property int fontSizeHeading: 18
    readonly property int fontSizeTitle: 20
    readonly property int fontSizeDisplay: 24

    readonly property int fontWeightLight: Font.Light
    readonly property int fontWeightNormal: Font.Normal
    readonly property int fontWeightMedium: Font.Medium
    readonly property int fontWeightBold: Font.Bold

    // ── Spacing ──────────────────────────────────────────────────
    readonly property int spacingTiny: 4
    readonly property int spacingSmall: 8
    readonly property int spacingMedium: 12
    readonly property int spacingLarge: 16
    readonly property int spacingXLarge: 24

    // ── Sizing ───────────────────────────────────────────────────
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

    // ── Animation ────────────────────────────────────────────────
    readonly property int animationFast: 150
    readonly property int animationNormal: 250
    readonly property int animationSlow: 400
    readonly property int shadowElevationLow: 2
    readonly property int shadowElevationMedium: 4
    readonly property int shadowElevationHigh: 8
}
