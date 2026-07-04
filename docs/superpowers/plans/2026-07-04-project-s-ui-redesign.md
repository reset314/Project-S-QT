# Project-S UI Redesign — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Complete UI redesign — CSS theme engine, WeChat-style two-zone layout, full contact details editing, Discord-style input bar, memory viewer, SVG icon migration.

**Architecture:** Integrate `ableton/aqt-stylesheets` CSS engine. Restructure MainShell into vertical-tab sidebar + proportional-split content area. SVG icon system via Lucide. Secondary windows (QQuickWindow) for settings sub-pages.

**Tech Stack:** Qt 6.11.1 + CMake + MinGW GCC 13.1.0, aqt-stylesheets (Boost ≥ 1.54), Lucide SVG icons

## Global Constraints

- No Unicode icons — all icons via `qrc:/icons/<name>.svg`
- Window min 800×500
- Sidebar min-width only, draggable divider, no collapse
- Split ratios proportional (3:7 default), not fixed px
- `chatConfig.is_display_think` and `is_display_emotion` (booleans)
- Settings sub-pages open as standalone QQuickWindow, not StackView push
- Backend changes minimal — one field each in `AIUser.chat_config` and `ExtensionInstall`

---

### Task 1: Integrate aqt-stylesheets

**Files:**
- Modify: `CMakeLists.txt`
- Create: `src/core/theme/ThemeManager.h`
- Create: `src/core/theme/ThemeManager.cpp`

**Interfaces:**
- Produces: `ThemeManager` singleton — `void loadTheme(const QString &cssPath)`, `void reload()`, `QString currentThemePath()`

- [ ] **Step 1: Install Boost via vcpkg**

```bash
cd C:\Users\root\Documents\Programs\aaa2
vcpkg install boost --triplet=x64-mingw-static
```

- [ ] **Step 2: Add aqt-stylesheets source to vendor**

Clone `https://github.com/Ableton/aqt-stylesheets` into `src/core/vendor/aqt-stylesheets/`.

- [ ] **Step 3: Update CMakeLists.txt — add Boost + aqt-stylesheets**

```cmake
# After existing find_package lines:
find_package(Boost REQUIRED)

# After existing SOURCES:
list(APPEND SOURCES
    src/core/theme/ThemeManager.cpp
    src/core/vendor/aqt-stylesheets/source/StyleEngine.cpp
    src/core/vendor/aqt-stylesheets/source/StyleSet.cpp
    # ... all aqt-stylesheets source files
)

# In target_include_directories, add:
target_include_directories(${PROJECT_NAME} PRIVATE
    ${Boost_INCLUDE_DIRS}
    src/core/vendor/aqt-stylesheets/source
)

# In target_link_libraries, add:
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Boost_LIBRARIES}
)
```

- [ ] **Step 4: Write ThemeManager**

```cpp
// src/core/theme/ThemeManager.h
#pragma once
#include <QObject>
#include <QString>

class StyleEngine; // aqt-stylesheets

class ThemeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentThemePath READ currentThemePath NOTIFY themeChanged)
public:
    explicit ThemeManager(QObject *parent = nullptr);

    Q_INVOKABLE void loadTheme(const QString &cssPath);
    Q_INVOKABLE void reload();
    Q_INVOKABLE QString currentThemePath() const { return currentThemePath_; }

signals:
    void themeChanged();

private:
    StyleEngine *engine_ = nullptr;
    QString currentThemePath_;
};
```

```cpp
// src/core/theme/ThemeManager.cpp
#include "ThemeManager.h"
#include <StyleEngine.hpp> // aqt-stylesheets
#include <QFileInfo>
#include <QDir>
#include <QDebug>

ThemeManager::ThemeManager(QObject *parent) : QObject(parent) {
    engine_ = new StyleEngine(this);
}

void ThemeManager::loadTheme(const QString &cssPath) {
    QFileInfo fi(cssPath);
    if (!fi.exists()) {
        qWarning() << "ThemeManager: CSS file not found:" << cssPath;
        return;
    }
    engine_->loadStyleSheet(cssPath);
    currentThemePath_ = cssPath;
    emit themeChanged();
    qDebug() << "ThemeManager: loaded" << cssPath;
}

void ThemeManager::reload() {
    if (!currentThemePath_.isEmpty())
        loadTheme(currentThemePath_);
}
```

- [ ] **Step 5: Wire ThemeManager in main.cpp**

```cpp
// In main.cpp, after QApplication:
auto themeManager = new ThemeManager(&app);
ctx->setContextProperty("themeManager", themeManager);

// Read saved theme from QSettings, fall back to built-in light.css
QString savedTheme = QSettings("Project-S", "Project-S")
    .value("settings/themePath", ":/themes/light.css").toString();
themeManager->loadTheme(savedTheme);
```

- [ ] **Step 6: Build and verify**

```bash
cmake --build C:\Users\root\Documents\Programs\aaa2\_b --target AIChatDesktop
```

- [ ] **Step 7: Commit**

```bash
git add -A && git commit -m "feat(theme): integrate aqt-stylesheets, add ThemeManager"
```

---

### Task 2: Write 3 built-in CSS themes

**Files:**
- Create: `resources/themes/light.css`
- Create: `resources/themes/dark.css`
- Create: `resources/themes/glass.css`
- Modify: `resources/qml.qrc` (add 3 theme files under `/themes` prefix)

- [ ] **Step 1: light.css — Full CSS with :root variables**

```css
/* resources/themes/light.css */
:root {
  --primary: #1976D2; --primary-light: #63A4FF; --primary-dark: #004BA0;
  --accent: #FF6F00; --accent-light: #FFA040; --accent-dark: #C43E00;
  --bg-main: #F5F5F5; --bg-surface: #FFFFFF; --bg-sidebar: #FAFAFA;
  --bg-titlebar: #1565C0; --bg-input: #FFFFFF;
  --divider: #E0E0E0;
  --text-primary: #212121; --text-secondary: #757575; --text-hint: #BDBDBD;
  --text-on-primary: #FFFFFF; --text-on-accent: #FFFFFF;
  --user-bubble: #1976D2; --user-bubble-text: #FFFFFF;
  --ai-bubble: #FFFFFF; --ai-bubble-text: #212121;
  --system-bubble: #FFF3E0; --system-bubble-text: #E65100;
  --sidebar-hover: #EEEEEE; --sidebar-selected: #E3F2FD;
  --badge: #F44336; --badge-text: #FFFFFF;
  --online: #4CAF50; --offline: #BDBDBD;
  --error: #D32F2F; --warning: #F57C00; --success: #388E3C;
  --font-family: ""; --font-scale: 1.0;
  --font-caption: 11px; --font-small: 12px; --font-body: 14px;
  --font-subheading: 16px; --font-heading: 18px; --font-title: 20px; --font-display: 24px;
  --chat-bg: #F5F5F5;
  --bubble-radius: 12px; --card-radius: 8px; --button-radius: 4px;
  --sidebar-width: 300px; --sidebar-min-width: 200px;
  --titlebar-height: 32px; --input-min-height: 48px; --input-max-height: 120px;
  --avatar-sm: 32px; --avatar-md: 40px; --avatar-lg: 56px; --avatar-xl: 80px;
  --icon-sm: 16px; --icon-md: 20px; --icon-lg: 24px;
  --spacing-tiny: 4px; --spacing-sm: 8px; --spacing-md: 12px;
  --spacing-lg: 16px; --spacing-xl: 24px;
  --border-width: 1px;
  --shadow-low: 2; --shadow-md: 4; --shadow-high: 8;
  --glass-blur: 0; --glass-opacity: 1.0;
  --anim-fast: 150ms; --anim-normal: 250ms; --anim-slow: 400ms;
}

#title-bar { background: var(--bg-titlebar); }
.title-text { color: var(--text-on-primary); font-size: var(--font-small); }

#sidebar { background: var(--bg-sidebar); }
.tab-btn { padding: var(--spacing-sm) var(--spacing-md); color: var(--text-secondary); }
.tab-btn.selected { color: var(--primary); border-left: 2px solid var(--primary); }

.list-item { padding: var(--spacing-sm) var(--spacing-lg); border-bottom: var(--border-width) solid var(--divider); background: var(--bg-surface); }
.list-item:hover { background: var(--sidebar-hover); }
.list-item.selected { background: var(--sidebar-selected); }
.list-item-name { font-size: var(--font-body); font-weight: bold; color: var(--text-primary); }
.list-item-subtitle { font-size: var(--font-small); color: var(--text-secondary); }
.list-item-time { font-size: var(--font-caption); color: var(--text-hint); }
.badge { background: var(--badge); color: var(--badge-text); border-radius: 8px; font-size: var(--font-caption); }

#page-header { background: var(--bg-surface); border-bottom: var(--border-width) solid var(--divider); }
.header-title { color: var(--text-primary); font-size: var(--font-subheading); font-weight: bold; }

#chat-area { background: var(--bg-main); }
.bubble { border-radius: var(--bubble-radius); padding: var(--spacing-sm) var(--spacing-md); }
.bubble.user { background: var(--user-bubble); color: var(--user-bubble-text); }
.bubble.ai { background: var(--ai-bubble); color: var(--ai-bubble-text); }
.bubble.system { background: var(--system-bubble); color: var(--system-bubble-text); }
.bubble-text { font-size: var(--font-body); }
.bubble-time { font-size: var(--font-caption); color: var(--text-hint); }
.think-bubble { background: #FFFDE7; font-size: var(--font-small); border-radius: 6px; }
.emotion-label { font-size: var(--font-caption); color: var(--text-secondary); }

#input-bar { background: var(--bg-surface); border-top: var(--border-width) solid var(--divider); min-height: var(--input-min-height); }
#input-field { background: var(--bg-input); border-radius: var(--button-radius); padding: var(--spacing-sm) var(--spacing-md); }
.input-btn { width: 36px; height: 36px; border-radius: 18px; }
.send-btn { background: var(--primary); color: var(--text-on-primary); border-radius: var(--button-radius); }

.card { background: var(--bg-surface); border-radius: var(--card-radius); border: var(--border-width) solid var(--divider); padding: var(--spacing-lg); }
.card-label { font-size: var(--font-caption); color: var(--text-hint); }
.card-value { font-size: var(--font-body); color: var(--text-primary); }

.settings-item { padding: var(--spacing-md) var(--spacing-lg); border-bottom: var(--border-width) solid var(--divider); }
.section-header { font-size: var(--font-caption); color: var(--text-hint); font-weight: bold; padding: var(--spacing-sm) var(--spacing-lg); }

.btn { border-radius: var(--button-radius); padding: var(--spacing-sm) var(--spacing-lg); font-size: var(--font-body); }
.btn.primary { background: var(--primary); color: var(--text-on-primary); }
.btn.danger { background: var(--error); color: var(--text-on-primary); }

.dialog { background: var(--bg-surface); border-radius: var(--card-radius); box-shadow: 0 4px 12px rgba(0,0,0,0.15); }

.empty-state { color: var(--text-hint); font-size: var(--font-body); }
```

- [ ] **Step 2: dark.css — Dark theme variables**

Copy `light.css`, replace `:root` block with dark colors. Key differences:
```css
--primary: #90CAF9; --primary-dark: #42A5F5;
--bg-main: #121212; --bg-surface: #1E1E1E; --bg-sidebar: #181818;
--bg-titlebar: #0D0D0D; --bg-input: #2A2A2A;
--divider: #333333;
--text-primary: #E0E0E0; --text-secondary: #9E9E9E; --text-hint: #616161;
--text-on-primary: #121212;
--user-bubble: #1A73E8; --ai-bubble: #2A2A2A; --ai-bubble-text: #E0E0E0;
--system-bubble: #3E2723; --system-bubble-text: #FFCC80;
--sidebar-hover: #2A2A2A; --sidebar-selected: #1E3A5F;
```

- [ ] **Step 3: glass.css — Glassmorphism theme**

Key differences from dark:
```css
--bg-main: transparent; --bg-surface: rgba(255,255,255,0.06);
--bg-sidebar: rgba(0,0,0,0.4); --bg-titlebar: rgba(0,0,0,0.5);
--glass-blur: 20px; --glass-opacity: 0.75;
--bg-input: rgba(255,255,255,0.08);
--divider: rgba(255,255,255,0.1);
--ai-bubble: rgba(255,255,255,0.08);
```
Rest same as dark.

- [ ] **Step 4: Add themes to QRC**

```xml
<qresource prefix="/themes">
    <file alias="light.css">../resources/themes/light.css</file>
    <file alias="dark.css">../resources/themes/dark.css</file>
    <file alias="glass.css">../resources/themes/glass.css</file>
</qresource>
```

- [ ] **Step 5: Build verify**

```bash
cmake --build C:\Users\root\Documents\Programs\aaa2\_b --target AIChatDesktop
```

- [ ] **Step 6: Commit**

---

### Task 3: Theme switching + import in Settings

**Files:**
- Modify: `resources/qml/pages/SettingsPage.qml`
- Create: `resources/qml/pages/ThemeSettingsPage.qml`

- [ ] **Step 1: Add theme dropdown to SettingsPage**

```qml
// Inside SettingsPage's ListModel, add at top:
ListElement {
    title: "主题模式"
    subtitle: "选择或导入自定义主题"
    icon: "palette"
    page: "ThemeSettingsPage"
}
```

- [ ] **Step 2: ThemeSettingsPage — dropdown + import button**

```qml
// resources/qml/pages/ThemeSettingsPage.qml
Item {
    ColumnLayout {
        C.BackHeader { title: qsTr("Theme"); onBackClicked: closePage() }
        // Theme dropdown
        ComboBox {
            model: ["Light", "Dark", "Glass", "Custom"]
            currentIndex: /* read from QSettings */
            onActivated: {
                var paths = [":/themes/light.css", ":/themes/dark.css", ":/themes/glass.css", customPath]
                themeManager.loadTheme(paths[index])
                QSettings("Project-S", "Project-S").setValue("settings/themePath", paths[index])
            }
        }
        // Import custom theme
        Button {
            text: qsTr("Import CSS Theme")
            onClicked: fileDialog.open()
        }
    }
    FileDialog {
        id: fileDialog
        title: qsTr("Import CSS Theme")
        nameFilters: ["CSS Files (*.css)"]
        onAccepted: {
            var srcPath = fileUrl.toString().replace("file:///", "")
            var destDir = QStandardPaths.writableLocation(QStandardPaths.AppDataLocation) + "/themes/"
            // Copy file to app data, load it
            themeManager.loadTheme(destPath)
            QSettings("Project-S", "Project-S").setValue("settings/themePath", destPath)
        }
    }
}
```

- [ ] **Step 3: Build verify, commit**

---

### Task 4: MainShell rewrite — vertical tabs + split view

**Files:**
- Rewrite: `resources/qml/shell/MainShell.qml`
- Rewrite: `resources/qml/shell/Sidebar.qml`
- Create: `resources/qml/components/TabButton.qml`

- [ ] **Step 1: TabButton component**

```qml
// resources/qml/components/TabButton.qml
Rectangle {
    property string iconName: ""
    property string label: ""
    property bool selected: false
    signal clicked()
    width: 64; height: 48
    color: selected ? Theme.sidebarSelected : "transparent"
    Column {
        anchors.centerIn: parent; spacing: 2
        Icon { name: iconName; size: 22; anchors.horizontalCenter: parent.horizontalCenter }
        Text { text: label; font.pixelSize: 10; color: Theme.textSecondary; anchors.horizontalCenter: parent.horizontalCenter }
    }
    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: parent.clicked() }
}
```

- [ ] **Step 2: New Sidebar — tabs + list area**

```qml
// resources/qml/shell/Sidebar.qml
Rectangle {
    id: sidebar
    property int currentTab: 0  // 0=chats, 1=contacts, 2=extensions
    property int minWidth: 200
    property int defaultWidth: 300
    width: defaultWidth
    color: Theme.sidebarBackground

    RowLayout {
        anchors.fill: parent; spacing: 0
        // ── Tab bar (vertical) ──
        ColumnLayout {
            Layout.preferredWidth: 64
            spacing: 0
            C.TabButton { iconName: "message-circle"; label: qsTr("Chats"); selected: sidebar.currentTab === 0; onClicked: sidebar.currentTab = 0 }
            C.TabButton { iconName: "users"; label: qsTr("Contacts"); selected: sidebar.currentTab === 1; onClicked: sidebar.currentTab = 1 }
            C.TabButton { iconName: "puzzle"; label: qsTr("Extensions"); selected: sidebar.currentTab === 2; onClicked: sidebar.currentTab = 2 }
            Item { Layout.fillHeight: true }
            // Settings gear at bottom
            C.TabButton { iconName: "settings"; label: qsTr("Settings"); onClicked: sidebar.settingsClicked() }
        }
        // ── Vertical divider ──
        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: Theme.dividerColor }
        // ── List area ──
        ColumnLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 0
            // Header row: avatar + action icon
            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 48
                RowLayout {
                    // avatar, spacer, action icon (new contact / import extension)
                }
            }
            // Dynamic list based on tab
            StackLayout {
                Layout.fillWidth: true; Layout.fillHeight: true
                currentIndex: sidebar.currentTab
                ChatListView { }      // Task 6
                ContactListView { }   // Task 7
                ExpansionListView { } // Task 20 (existing, adapted)
            }
        }
    }

    signal chatSelected(string aiUserId, string aiUserName)
    signal contactSelected(string aiUserId)
    signal settingsClicked()
}
```

- [ ] **Step 3: New MainShell — split layout**

```qml
// resources/qml/shell/MainShell.qml
Item {
    RowLayout {
        anchors.fill: parent; spacing: 0
        Sidebar { id: sidebar; Layout.preferredWidth: sidebar.width; Layout.fillHeight: true
            // signals handled inside
        }
        // ── Draggable divider ──
        Rectangle {
            Layout.preferredWidth: 4; Layout.fillHeight: true
            color: "transparent"
            MouseArea {
                anchors.fill: parent; cursorShape: Qt.SplitHCursor
                property real startX: 0
                onPressed: function(m) { startX = m.x }
                onPositionChanged: function(m) {
                    var newW = sidebar.width + (m.x - startX)
                    if (newW >= sidebar.minWidth) sidebar.width = newW
                }
            }
        }
        // ── Content StackView ──
        StackView { id: stackView; Layout.fillWidth: true; Layout.fillHeight: true; initialItem: welcomePlaceholder }
    }
    // ... component cache, timers, navigateToChat
}
```

- [ ] **Step 4: Build verify, commit**

---

### Task 5: Chat list view (sidebar)

**Files:**
- Create: `resources/qml/components/ChatListDelegate.qml`
- Modify: `resources/qml/shell/Sidebar.qml` (ChatListView section)

- [ ] **Step 1: ChatListDelegate**

```qml
// resources/qml/components/ChatListDelegate.qml
Rectangle {
    property string aiUserId: ""; property string name: ""; property string avatar: ""
    property string lastMessage: ""; property string lastTime: ""; property int unread: 0
    signal clicked()
    width: parent.width; height: 64
    RowLayout {
        anchors { fill: parent; margins: Theme.spacingMedium }; spacing: Theme.spacingMedium
        C.UserAvatar { Layout.preferredWidth: 40; Layout.preferredHeight: 40; name: name; imageSource: avatar }
        ColumnLayout {
            Layout.fillWidth: true; spacing: 2
            RowLayout {
                Text { text: name; font.pixelSize: Theme.fontSizeBody; font.weight: Font.Bold; color: Theme.textPrimary; Layout.fillWidth: true; elide: Text.ElideRight }
                Text { text: lastTime; font.pixelSize: Theme.fontSizeCaption; color: Theme.textHint }
            }
            RowLayout {
                Text { text: lastMessage; font.pixelSize: Theme.fontSizeSmall; color: Theme.textSecondary; Layout.fillWidth: true; elide: Text.ElideRight; maximumLineCount: 1 }
                Rectangle { visible: unread > 0; width: 18; height: 18; radius: 9; color: Theme.badgeColor
                    Text { anchors.centerIn: parent; text: unread > 99 ? "99+" : unread; font.pixelSize: 10; color: "white" }
                }
            }
        }
    }
    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: parent.clicked() }
}
```

- [ ] **Step 2: Wire into sidebar ChatListView**

```qml
ListView {
    id: chatListView
    Layout.fillWidth: true; Layout.fillHeight: true; clip: true
    model: typeof conversationListModel !== "undefined" ? conversationListModel : null
    delegate: C.ChatListDelegate {
        width: chatListView.width
        aiUserId: model.aiUserId; name: model.aiUserName || model.title
        lastMessage: model.lastMessagePreview; lastTime: formatTime(model.updatedAt)
        unread: /* from unreadTracker */
        onClicked: sidebar.chatSelected(model.aiUserId, model.aiUserName || model.title)
    }
}
```

- [ ] **Step 3: Build verify, commit**

---

### Task 6: Contact list view (sidebar) + new AI user button

**Files:**
- Modify: `resources/qml/components/ContactCard.qml` (simplify, keep as delegate)
- Modify: `resources/qml/shell/Sidebar.qml`

- [ ] **Step 1: Simplify ContactCard** — Remove chat button, keep avatar+name+description, click → contactSelected

- [ ] **Step 2: Add new AI User icon button** to sidebar header (user-plus icon) → emit `createAIUserClicked`

- [ ] **Step 3: Wire ContactCard** in sidebar ContactListView (same as existing)

- [ ] **Step 4: Build verify, commit**

---

### Task 7: ChatPage — split two-zone + title bar + time per bubble

**Files:**
- Modify: `resources/qml/pages/ChatPage.qml`
- Modify: `resources/qml/components/ChatBubble.qml`

- [ ] **Step 1: ChatPage two-zone layout** — Left panel: chat list (reuse ChatListDelegate), Right panel: current ChatPage content, split with draggable divider (same pattern as MainShell)

- [ ] **Step 2: Title bar** — Only show name, `C.BackHeader` with just the name, no avatar/online

- [ ] **Step 3: Timestamp per bubble** — Each ChatBubble renders its own `.bubble-time` Text below. Uses `formatTime(chatBubble.timestamp)` (HH:mm)

- [ ] **Step 4: think/emotion conditional** — Read `chatConfig.is_display_think` / `is_display_emotion` from AIUser.chatConfig. Only render think/emotion sections if enabled.

- [ ] **Step 5: Build verify, commit**

---

### Task 8: Message input bar — Discord style

**Files:**
- Rewrite: `resources/qml/components/MessageInput.qml`
- Create: `resources/qml/components/MoreMenu.qml`

- [ ] **Step 1: Rewrite MessageInput**

```qml
RowLayout {
    // [📎 upload] [🎤 voice] [  TextArea  ] [↑ send] [··· more]
    C.InputIconBtn { iconName: "paperclip"; onClicked: messageInput.attachFile() }
    C.InputIconBtn { iconName: "mic"; onClicked: toggleVoiceMode() }
    TextArea { Layout.fillWidth: true; ... }
    C.InputIconBtn { iconName: "arrow-up"; isSend: true; onClicked: sendCurrentText() }
    C.InputIconBtn { iconName: "more-horizontal"; onClicked: moreMenu.open() }
    MoreMenu { id: moreMenu }
}
```

- [ ] **Step 2: MoreMenu floating card**

```qml
Popup {
    ColumnLayout {
        MenuItem { iconName: "phone"; text: qsTr("Voice Call"); onClicked: startCall() }
        // Extension-registered items from input_bar_menu registrations
    }
}
```

- [ ] **Step 3: Build verify, commit**

---

### Task 9: ContactDetailPage — full field editing

**Files:**
- Rewrite: `resources/qml/pages/ContactDetailPage.qml`

- [ ] **Step 1: Edit/save mode** — `property bool editing: false` toggles Text vs TextField/ComboBox/Switch

- [ ] **Step 2: All fields** per AIUser model (see design spec §4). Two columns using `FormLayout` or `GridLayout`.

- [ ] **Step 3: "View Memories" button** → opens Memory floating card (Task 11)

- [ ] **Step 4: "Save" calls `aiUserRepo->updateAIUser()`**

- [ ] **Step 5: Build verify, commit**

---

### Task 10: CreateAIUserPage — all fields

**Files:**
- Modify: `resources/qml/pages/CreateAIUserPage.qml`

- [ ] **Step 1: Add missing fields** — LLM Image Provider/Model, Multimodal, Audio Transcribe, Audio Understand, TTS, chatConfig switches

- [ ] **Step 2: POST all fields to API**

- [ ] **Step 3: Build verify, commit**

---

### Task 11: Memory — DB table + repository

**Files:**
- Modify: `src/core/storage/Database.cpp` (migration)
- Modify: `src/core/storage/Database.h`
- Modify: `src/repositories/MemoryRepository.h`
- Modify: `src/repositories/MemoryRepository.cpp`
- Modify: `src/core/models/MemoryDTO.h` (already exists)

- [ ] **Step 1: Add `memories` table migration 18**

```sql
CREATE TABLE IF NOT EXISTS memories (
    id TEXT PRIMARY KEY,
    ai_user_id TEXT NOT NULL,
    content TEXT NOT NULL,
    summary TEXT,
    memory_type TEXT DEFAULT 'STM',
    category TEXT DEFAULT 'general',
    importance INTEGER DEFAULT 5,
    confidence REAL DEFAULT 1.0,
    is_archived INTEGER DEFAULT 0,
    source_memory_id TEXT,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_mem_ai_user ON memories(ai_user_id);
CREATE INDEX IF NOT EXISTS idx_mem_type ON memories(memory_type);
```

- [ ] **Step 2: MemoryRepository::getMemories**

```cpp
QVector<MemoryDTO> getMemories(const QString &aiUserId,
                                const QStringList &memoryTypes = {"MTM","LTM"},
                                const QString &beforeId = {},
                                int limit = 30);
```

- [ ] **Step 3: Build verify, commit**

---

### Task 12: Memory floating card UI

**Files:**
- Create: `resources/qml/components/MemoryViewer.qml`

- [ ] **Step 1: 3-zone layout**

```qml
Popup {
    // Zone 1: Top bar — close button
    // Zone 2: Filter — STM/MTM/LTM checkboxes (MTM+LTM default checked)
    // Zone 3: ScrollView + ListView — each memory as a card
    //   Card: summary (bold) + content (100 char truncate) + category badge + importance stars + time
}
```

- [ ] **Step 2: Wire to MemoryRepository** — call `getMemories` on open, paginate on scroll to top

- [ ] **Step 3: Build verify, commit**

---

### Task 13: Settings page sync

**Files:**
- Modify: `resources/qml/pages/SettingsPage.qml`

- [ ] **Step 1: Replace ListModel with 7 settings items** (no sections, no expansions, no clear data):

1. Theme → ThemeSettingsPage (Task 3)
2. Personal Info → PersonalInfoPage (new window, Task 14)
3. User Profiles → ProfilePage (new window)
4. TTS Voices → TTSVoicesPage (new window)
5. Chat Config → ChatConfigPage (new window, Task 14)
6. LLM Providers → LlmProviderPage (new window, Task 14)
7. Account → AccountPage (new window)

- [ ] **Step 2: Replace all Unicode icon Text with `Icon {}`**

- [ ] **Step 3: Build verify, commit**

---

### Task 14: Secondary window system + missing pages

**Files:**
- Create: `src/core/utils/SecondaryWindow.h`
- Create: `src/core/utils/SecondaryWindow.cpp`
- Create: `resources/qml/pages/ChatConfigPage.qml`
- Create: `resources/qml/pages/LlmProviderPage.qml`
- Create: `resources/qml/pages/PersonalInfoPage.qml`

- [ ] **Step 1: SecondaryWindow factory**

```cpp
// src/core/utils/SecondaryWindow.h
class SecondaryWindow : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE void open(const QString &qmlPath, const QVariantMap &props = {});
    // Creates a new QQuickWindow, loads qmlPath component, sets context props
};
```

Usage in QML: `secondaryWindow.open("qrc:/qml/pages/ChatConfigPage.qml", {aiUserId: "..."})`

- [ ] **Step 2: ChatConfigPage** — LLM config form (provider/model selection + parameters)

- [ ] **Step 3: LlmProviderPage** — list/settings for LLM providers

- [ ] **Step 4: PersonalInfoPage** — user display name, avatar URL

- [ ] **Step 5: Build verify, commit**

---

### Task 15: Expansion page adapt

**Files:**
- Modify: `resources/qml/pages/ExpansionPage.qml`
- Create: `resources/qml/pages/UploadExpansionPage.qml`

- [ ] **Step 1: Add status badges** to list items (installed/pending/approved/error)

- [ ] **Step 2: "Add Extension" button** top-right → opens UploadExpansionPage as new SecondaryWindow

- [ ] **Step 3: UploadExpansionPage** — FilePicker for ZIP + fields (name, description) → POST to API

- [ ] **Step 4: Build verify, commit**

---

### Task 16: Backend changes (aaa)

**Files:**
- Modify: `C:\Users\root\Documents\Programs\aaa\src\services\models\model.py`
- Modify: `C:\Users\root\Documents\Programs\aaa\src\services\services\aiuser\schemas.py`
- Modify: `C:\Users\root\Documents\Programs\aaa\src\services\services\expansion\models.py`

- [ ] **Step 1: AIUser — ensure `is_display_emotion` in chatConfig defaults** (add to chatConfig default dict if not present)

- [ ] **Step 2: ExtensionInstall — add `review_status` column** (String, default "pending")

- [ ] **Step 3: Generate Alembic migration**

```bash
cd C:\Users\root\Documents\Programs\aaa
alembic -c src/configs/alembic.ini revision --autogenerate -m "add review_status to extension_install"
alembic -c src/configs/alembic.ini upgrade head
```

- [ ] **Step 4: Commit**

---

### Task 17: Unicode icon cleanup

**Files:**
- All remaining QML using Unicode text ←/↓/⚙/📎/↑/▶/💬/👤/👥/🧩/...

- [ ] **Step 1: Grep all QML for Unicode pictographs**

```bash
grep -rn '"←"\|"↓"\|"⚙"\|"📎"\|"↑"\|"▶"\|"💬"\|"👤"\|"👥"\|"🧩"\|"🎨"\|"🔊"\|"📋"\|"🌐"\|"ℹ️"\|"›"\|"+"' resources/qml/
```

- [ ] **Step 2: Replace each with `Icon { name: "..." }`**

- [ ] **Step 3: Remove EmojiPicker Unicode emoji, replace with Lucide `smile` icon only** (emoji picker can come later)

- [ ] **Step 4: Build verify, commit**

---

### Task 18: Final build + smoke test

- [ ] **Step 1: Clean build**

```bash
cmake --build C:\Users\root\Documents\Programs\aaa2\_b --target AIChatDesktop --clean-first
```

- [ ] **Step 2: Run and verify**

```bash
cd C:\Users\root\Documents\Programs\aaa2\_b && ./AIChatDesktop.exe
```

- [ ] **Step 3: Smoke test checklist**
  1. Login → sidebar tabs visible
  2. Chats tab → select → ChatPage opens with split layout
  3. Send message → appears + AI responds
  4. Contacts tab → click → ContactDetailPage with all fields
  5. Edit AI user → save → verify
  6. Memory viewer opens
  7. Settings → each item opens (new window or inline)
  8. Theme switch → UI updates
  9. Resize window → dividers work, min size enforced
  10. All icons are SVG (no Unicode visible)

- [ ] **Step 4: Commit**

---

**Total: 18 tasks**
