# Project-S UI Redesign — Design Spec

**Date:** 2026-07-04
**Scope:** Qt desktop client (aaa2) full UI redesign + theme system

## 1. Layout: WeChat-Style Two-Zone

```
┌──────────────────────────────────────────┐
│ TitleBar: Project-S    [─] [□] [✕]      │
├──────┬──────┬────────────────────────────┤
│ 聊天  │ list │  Content Area             │
│ 通讯录│      │  (StackView)              │
│ 扩展  │      │                          │
│      │      │                          │
└──────┴──────┴────────────────────────────┘
```

- **Left sidebar**: vertical tabs (聊天/通讯录/扩展) + expandable list
- **Content area**: StackView (welcome → ChatPage / ContactDetail / Settings / etc.)
- **Split ratio**: default 3:7, draggable divider, list min-width ~200px
- **No collapse**: sidebar not collapsible, only min-width limited
- **Window**: min 800×500

### Sidebar Content Per Tab

| Tab | List Content | Click → |
|-----|-------------|---------|
| 聊天 | avatar + name + lastMessagePreview + time | ChatPage |
| 通讯录 | avatar + name + description | ContactDetailPage |
| 扩展 | name + description + version + author + toggle | Expansion panel |

### ChatPage Two-Zone
- **Left**: chat contact list (same as 聊天 tab above)
- **Right**: current ChatPage content (header + messages + input)
- **Split ratio**: 3:7, draggable

### Header
- Chat page: name only
- Detail pages: BackHeader component (← + title)
- Remove all "online" indicators

## 2. Theme System: CSS via aqt-stylesheets

Use [ableton/aqt-stylesheets](https://github.com/ableton/aqt-stylesheets) library.

- One theme = one `.css` file
- CSS custom properties (`:root { --primary: #xxx; }`) for all variables
- No JSON theme files — pure CSS
- Built-in 3 themes: `light.css`, `dark.css`, `glass.css`
- Theme switching: dropdown in Settings → reload CSS → persist path in QSettings
- Startup: read QSettings → load saved theme CSS
- Import: file dialog → pick `.css` → copy to `themes/` → set as current
- Chat background: `var(--chat-bg)` as base64 data URI
- Icons: `var(--icon-xxx)` as base64 data URI for custom icon override

### CSS Selector Tree (~50 selectors)
```
#app, #title-bar, .title-text,
#sidebar, .tab-btn, .tab-btn.selected,
.list-item, .list-item:hover, .list-item.selected,
.list-item-avatar, .list-item-name, .list-item-subtitle, .list-item-time, .badge,
#content, #page-header, .header-back-btn, .header-title,
#chat-area, .bubble, .bubble.user, .bubble.ai, .bubble.system,
.bubble-text, .bubble-time, .think-bubble, .emotion-label,
.typing-indicator, .fab,
#input-bar, #input-field, .input-btn, .send-btn,
.card, .card-label, .card-value,
.settings-item, .section-header,
.btn, .btn.primary, .btn.danger, .toggle,
.dialog, .dialog-header, .dialog-body,
.empty-state, .scrollbar
```

## 3. ChatPage Details

### Messages
- Timestamp below each bubble (independent rendering)
- think/emotion rendering controlled by `chatConfig.is_display_think` and `chatConfig.is_display_emotion`
- `<split>` expansion: both history load and incremental (stream_done)

### Input Bar (Discord-style)
```
[📎 upload] [🎤 voice toggle] [  input field  ] [↑ send] [··· more]
                                                       └─ floating card:
                                                           · voice call
                                                           · (extension-registered)
```

### Input Bar Extension Integration
- `input_bar_action` → icon on the bar (left of input)
- `input_bar_menu` → entry in the "more" floating card
- `nav_tab` → vertical tab entry in sidebar

## 4. ContactDetailPage

All fields editable (toggle edit/save mode). Based on backend `AIUser` model:

| Field | Control |
|-------|---------|
| Avatar | TextField (URL) + preview |
| Name | TextField |
| Description | TextArea |
| LLM Provider | ComboBox |
| LLM Model | TextField |
| Image Provider | TextField |
| Image Model | TextField |
| Multimodal Provider | TextField |
| Multimodal Model | TextField |
| Audio Transcribe Provider | TextField |
| Audio Transcribe Model | TextField |
| Audio Understanding Provider | TextField |
| Audio Understanding Model | TextField |
| System Prompt | TextArea |
| TTS Voice | ComboBox |
| Display Think | Switch |
| Display Emotion | Switch |
| Created | Read-only |
| Updated | Read-only |

Plus "View Memories" button → floating card (3-zone: title bar + layer filter + card list).

## 5. Memory

Qt SQLite table `memories` (matching backend schema):
- id, ai_user_id, content, summary, memory_type, category, importance, confidence, created_at, updated_at

MemoryRepository additions:
- `getMemories(aiUserId, memoryTypes, beforeId, limit)` — paginated fetch

Memory floating card UI:
- Zone 1: top bar with close button
- Zone 2: title + STM/MTM/LTM checkbox filter (MTM+LTM default checked, STM unchecked)
- Zone 3: scrollable list, each memory as a card, most recent first

## 6. Icons

All Unicode replaced with Lucide SVG icons (MIT license, 24 downloaded to `resources/icons/`):
- `message-circle`, `users`, `puzzle` — tabs
- `settings` — settings gear
- `arrow-left` — back
- `arrow-up` — send
- `paperclip` — attach
- `mic` — voice
- `more-horizontal` — more menu
- `play` — play audio
- `smile` — emoji
- `phone` — call
- `plus` — add
- `user-plus` — new AI user
- `palette` — theme
- `brain` — memory
- `image` — image upload
- `x`, `minus`, `square` — window controls
- `chevron-right`, `chevron-down` — navigation
- `panel-left-close` — sidebar toggle
- `search` — search

App icon: `curve_triangle_rectangle_优弧.svg` → copied to `resources/icons/app.svg`

## 7. Settings Page

Sync Flutter settings, exclude "clear data" and "expansions":
1. Theme Mode — dropdown (Light/Dark/Glass), placeholder until theme engine ready
2. Personal Info — opens new window
3. User Profiles → ProfilePage — new window
4. TTS Voices → TTSVoicesPage — new window
5. Chat Config → LLM config — new window
6. LLM Providers — new window
7. Account → AccountPage — new window

## 8. Secondary Windows

New window types (QQuickWindow, not StackView):
- Memory viewer
- New extension upload
- Settings sub-pages (Chat Config, LLM Providers, Personal Info)
- New AI User creation

Stay as floating card (not window):
- Chat page memory viewer
- Input bar "more" menu

## 9. Backend Changes (aaa)

- `AIUser.chat_config` — ensure `is_display_emotion` field recognized
- `ExtensionInstall` — add `review_status` field (pending/approved/rejected)
