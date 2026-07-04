# Qt Desktop Client — Design Specification

- **Date**: 2026-07-02
- **Status**: Draft
- **Target**: Full-featured Qt C++ + QML desktop client for AI Chat backend

---

## 1. Overview

Build a cross-platform (Windows / macOS / Linux) desktop client using Qt 6 + C++17 + QML that replicates all functionality of the existing Flutter mobile client (`aaa1`), communicating with the same FastAPI backend (`aaa`).

### Key Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Language | C++17 | Qt 6 requires C++17 minimum; no need for C++20 coroutines (Qt signals/slots suffice) |
| UI | QML (Qt Quick) | Declarative, ideal for chat UI, good ListView virtual scrolling |
| WebRTC | libdatachannel | Lightweight (~2MB), clean C++ API, native Win/Mac/Linux support |
| JSON | nlohmann/json | Industry standard, better ergonomics than QJsonDocument |
| Build | CMake + vcpkg (Win) / apt (Linux) / homebrew (macOS) | |

### Communication Protocol

The backend uses **WebSocket only** for real-time communication. There is no SSE.

| Connection | Endpoint | Purpose |
|-----------|----------|---------|
| Main WS | `/ws/connect` | Receive LLM streaming results + ping/pong keepalive |
| Call WS | `/ws/call/{conv_id}` | WebRTC signaling relay |
| REST | Various | Auth, users, conversations, upload, memories, profiles, expansion |

---

## 2. Architecture

```
┌──────────────────────────────────────────────────────┐
│  QML UI Layer                                        │
│  (Pages, Components, Theme)                          │
├──────────────────────────────────────────────────────┤
│  C++ Core Layer                                      │
│  ┌─────────────────────┬───────────────────────────┐ │
│  │ QWebSocket (x2)     │ QNetworkAccessManager     │ │
│  │  /ws/connect (chat) │  REST API calls           │ │
│  │  /ws/call/* (信令)   │  (auth, users, upload...) │ │
│  ├─────────────────────┼───────────────────────────┤ │
│  │ Qt Multimedia       │  SQLite (QSqlDatabase)    │ │
│  │ (音视频采集/渲染)    │  (离线缓存+乐观插入)       │ │
│  └─────────────────────┴───────────────────────────┘ │
├──────────────────────────────────────────────────────┤
│  Repository Layer (11 repos → 11 API module maps)    │
├──────────────────────────────────────────────────────┤
│  Models Layer (C++ structs mirroring Pydantic DTOs)  │
├──────────────────────────────────────────────────────┤
│  Auth Layer (JWT storage/refresh/WS auth)            │
└──────────────────────────────────────────────────────┘
```

---

## 3. Feature Modules

All 12 modules from the Flutter client, mapped to Qt:

| # | Module | Core Responsibility |
|---|--------|---------------------|
| 1 | Network/Auth Base | HttpClient, ChatStreamClient, CallSignalingClient, TokenManager |
| 2 | Data Models + SQLite | 21 C++ DTO structs, local SQLite cache, optimistic insert, dedup |
| 3 | Chat | POST /chat → WS stream_message_init/chunk/done/error → incremental UI |
| 4 | Conversation Mgmt | List, create, endConversation, query by aiUserId |
| 5 | AI Contacts | List, detail, create/edit, avatar management |
| 6 | File Upload | Image/video/audio/file selection + multipart upload |
| 7 | Voice/Video Call | WS signaling + libdatachannel WebRTC + Qt Multimedia |
| 8 | Memory System | Memory query, user profile CRUD, profile version mgmt |
| 9 | Expansion Plugins | Module list, enable/disable, panel rendering, scheduled tasks |
| 10 | TTS | TTS request + audio playback |
| 11 | Settings | Account, theme, TTS voice selection, expansion config |
| 12 | User Management | Login/register, JWT storage, token refresh, admin |

---

## 4. Data Models & SQLite

### Principle

C++ structs mirror backend Pydantic DTOs field-for-field. The backend is the single source of truth.

### Core Structs

```cpp
struct MessageDTO {
    int64_t     serverId = 0;
    std::string clientUuid;
    std::string aiUserId;
    std::string conversationId;
    std::string senderType;         // "user" | "ai" | "system"
    std::string msgType;            // "text" | "image" | "video" | "file" | "audio"
    QJsonObject content;            // {think, emotion, response, functions}
    QJsonArray  mediaList;
    bool        isComplete = false;
    std::optional<QDateTime> revokedAt;
    QDateTime   createdAt;
};
// ... 20 more DTOs
```

### Local SQLite Schema

```sql
CREATE TABLE messages (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    server_id    INTEGER UNIQUE,
    client_uuid  TEXT NOT NULL,
    ai_user_id   TEXT NOT NULL,
    conversation_id INTEGER,
    sender_type  TEXT NOT NULL,
    msg_type     TEXT NOT NULL,
    content      TEXT,           -- JSON
    media_list   TEXT,           -- JSON
    is_complete  INTEGER DEFAULT 1,
    is_read      INTEGER DEFAULT 0,
    revoked_at   TEXT,
    created_at   TEXT NOT NULL
);
CREATE INDEX idx_messages_ai_user   ON messages(ai_user_id);
CREATE INDEX idx_messages_client    ON messages(client_uuid);
CREATE INDEX idx_messages_server    ON messages(server_id);
```

### Message Lifecycle

```
User sends → optimistic insert (clientUuid + serverId=0 + isComplete=1)
  → POST /chat returns metadata
  → WS stream_message_init → create AI message row (serverId pending)
  → WS chunk → append to content.response → UI incremental render
  → WS stream_message_done → serverId backfill + isComplete=true
  OR
  → WS stream_error → cleanup incomplete AI messages + watchdog cancel

Enter chat → local query by aiUserId + serverId order
  → pull server incremental → dedup by serverId + clientUuid
```

### Repository Layer Signature

```cpp
template<typename T>
using Result = std::expected<T, ApiError>;

class ChatRepository {
public:
    Result<ChatSendResponse> sendMessage(
        const QString &aiUserId, const QString &content,
        const QString &conversationId, const QFileInfo *file = nullptr);
    // ...
};
```

---

## 5. Network Communication Layer

### HttpClient (QNetworkAccessManager)

- Auto-inject `Authorization: Bearer <token>`
- Auto-refresh token on 401
- Unified error → `ApiError` mapping

### ChatStreamClient (QWebSocket → /ws/connect)

Parses 5 frame types:

| Frame | Action |
|-------|--------|
| `ping` | Reply `{"type":"pong"}` |
| `pong` | Reset pong timeout |
| `stream_message_init` | Create local AI message row, notify UI |
| `stream_chunk` | Append to content.response, incremental UI refresh |
| `stream_message_done` | Backfill serverId, isComplete=true, persist |
| `stream_error` | Cancel watchdog, cleanup incomplete messages, show error |

### CallSignalingClient (QWebSocket → /ws/call/{conv_id})

Relays SDP offer/answer and ICE candidates between WebRTC engine and signaling server.

### TokenManager

```
Login success → persist access_token + refresh_token (platform keychain)
Every request → check JWT exp locally → auto-refresh if needed
Startup → restore from keychain → connect WS
```

---

## 6. Message Failure Recovery (Three-Layer Defense)

### Layer 1: Client-Side Timeout

Every sent message gets a 30s timer. If `stream_message_init` doesn't arrive → mark as failed + show retry button.

### Layer 2: Local Phantom Cleanup (built into SyncService)

Every sync cycle deletes messages where `server_id IS NULL AND timestamp < now - 30min`.
The in-memory unsynced logic filters out messages older than 30 minutes to prevent re-importing phantoms.

### Layer 3: Reconnect Reconciliation

After WS reconnect, fetch server message IDs → delete local messages not found on server.

---

## 7. Unread System

### C++ UnreadTracker

```cpp
class UnreadTracker : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap unreadMap READ unreadMap NOTIFY unreadMapChanged)
public:
    void init(SQLiteHelper &db);
    void onIncomingMessage(const QString &conversationId, const QString &msgType);
    void clearAndMarkRead(const QString &aiUserId);
    void setActiveAiUser(const QString &aiUserId = {});
signals:
    void unreadMapChanged();
    void totalUnreadChanged(int total);
};
```

### Desktop-Only Enhancements

| Feature | Implementation |
|---------|---------------|
| Taskbar badge | Windows: ITaskbarList3; macOS: NSApp.dockTile; Linux: Unity Launcher |
| Tray icon switch | Swap icon when unread > 0 |
| Desktop notification | Windows Toast / macOS Notification Center / Linux D-Bus |
| Global shortcut | Ctrl+Shift+A to open window |

---

## 8. Voice/Video Call

### Signaling Flow

```
Client A                 WS Server                  Client B
  │── POST /call/start ──→│                            │
  │←── call_id ──────────│                            │
  │── WS /ws/call/{id} ──→│←── WS /ws/call/{id} ─────│
  │── offer ─────────────→│── relay ────────────────→│
  │                        │←── answer ──────────────│
  │←── relay ────────────│                            │
  │── ice_candidate ─────→│── relay ────────────────→│
  │◄════ P2P (WebRTC) ═════════════════════════════►│
```

### Architecture

- `CallManager` — lifecycle orchestrator (start, accept, hangup)
- `CallSignalingClient` — QWebSocket for /ws/call/{conv_id}
- `WebRtcEngine` — libdatachannel PeerConnection wrapper
- `AudioManager` — QAudioDevice mic capture + speaker output
- `VideoManager` — QCamera capture + remote video sink

### CallPage (Independent Window)

Full-screen remote video + self-view picture-in-picture + control bar (mute/video/hangup). Voice-only mode: avatar + waveform animation.

---

## 9. QML UI Structure

### Shell Layout

```
┌── Title Bar ───────────────────────────────────┐
│  File  Edit  View  Help                         │
├──────────┬───────────────────┬─────────────────┤
│ Sidebar  │   Chat Area       │  Detail Panel   │
│ (会话/联系人)│  (消息列表+输入框)   │  (可收起)       │
└──────────┴───────────────────┴─────────────────┘
      Status Bar: connection | unread count
```

### Page List

- LoginPage
- MainShell (3-column layout)
- ChatPage (message list + input)
- ContactListPage / ContactDetailPage / CreateAIUserPage
- ConversationListPage
- CallPage (independent window)
- SettingsPage / AccountPage / ProfilePage / ProfileEditPage
- TTSVoicesPage / ExpansionPage

### Key Components

- ChatBubble (text/image/video/file/audio/sticker)
- MessageInput (text input + send + file picker)
- TypingIndicator (animated dots)
- MediaPreview, FilePickerButton, EmojiPicker
- MarkdownRenderer (for AI response content)
- ContactCard, ConversationItem, UserAvatar

### C++ Model ↔ QML Binding

```cpp
class ChatMessagesModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { ServerIdRole = Qt::UserRole + 1, ContentRole, MsgTypeRole, ... };
    void appendMessage(const MessageDTO &msg);  // incremental, not full replace
    void updateContent(int row, const QString &newContent);
};
```

### System Tray

Minimize to tray, tray context menu (open/quit), unread badge, desktop notifications.

---

## 10. Expansion Plugin System

Lightweight client-side: fetch module list, toggle enable/disable, receive `proactive_message` via WS, render panels.

Built-in QML renderers:

| Panel Type | Implementation |
|-----------|---------------|
| `sticker_grid` | QML GridView |
| `document_list` | QML ListView |
| `tool_list` | Toolbar connector |
| `webview_panel` | Qt WebEngineView |

---

## 11. Settings & Configuration

### Settings Tree

- Account (profile info)
- Appearance (light/dark/system theme, font size, bubble style)
- Voice (TTS on/off, voice selection, speed)
- Expansion management
- Keyboard shortcuts
- Network (server URL, proxy)
- About

### Persistence

`QSettings` — auto-selects platform-appropriate storage (Windows Registry, macOS plist, Linux ~/.config).

---

## 12. Project File Tree

```
aaa2/
├── CMakeLists.txt
├── cmake/
│   └── FindLibDataChannel.cmake
├── resources/
│   ├── qml/
│   │   ├── main.qml
│   │   ├── shell/        (MainShell, Sidebar, TitleBar, StatusBar)
│   │   ├── pages/        (12 pages)
│   │   ├── components/   (10+ reusable components)
│   │   ├── dialogs/      (Confirm, ImagePreview, CallInvite)
│   │   └── theme/        (ThemeConfig, Dark, Light)
│   ├── assets/           (icons, sounds, fonts)
│   └── qml.qrc
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── network/      (HttpClient, ChatStreamClient, CallSignalingClient, ApiError)
│   │   ├── auth/         (TokenManager, Keychain)
│   │   ├── models/       (21 DTO headers)
│   │   ├── storage/      (Database, migrations)
│   │   └── utils/        (JsonHelper, UuidGenerator, ContentParser)
│   ├── repositories/     (11 repos)
│   ├── services/         (ChatService, SyncService, UnreadTracker,
│   │                      CallManager, WebRtcEngine, AudioManager,
│   │                      VideoManager, TTSManager, NotificationService)
│   ├── viewmodels/       (ChatMessagesModel, ConversationListModel,
│   │                      ContactListModel, LoginViewModel, SettingsViewModel)
│   └── platform/         (WinTaskbar, MacDock, LinuxDesktop)
└── tests/
```

---

## 13. Flutter Client Fixes (Record)

During design, three bugs were identified and fixed in `aaa1`:

1. **Phantom messages** — `loadHistory` unsynced logic re-imported stale messages from old in-memory state. Fixed by adding `deleteStaleUnsynced()` and 30min threshold filter.

2. **stream_error incomplete** — Missing watchdog cancel + incomplete AI message cleanup. Fixed in `chat_page.dart`.

3. **Unread not real-time** — `unreadNotifier.increment()` was never called from WS handlers. Fixed by adding global WS listener in `ws_provider.dart` with `activeChatAiUserIdProvider` tracking.
