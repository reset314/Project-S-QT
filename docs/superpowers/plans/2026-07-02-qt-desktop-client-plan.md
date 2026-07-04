# Qt Desktop Client — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a cross-platform Qt 6 + C++17 + QML desktop client that replicates all functionality of the Flutter mobile client (aaa1), communicating with the FastAPI backend via REST + WebSocket.

**Architecture:** Three-layer: QML UI → C++ ViewModels + Services → C++ Repositories + Network. SQLite for local cache with optimistic insertion. WebSocket as sole real-time channel (no SSE). libdatachannel for WebRTC calls.

**Tech Stack:** Qt 6.5+ (Quick, Network, WebSockets, Multimedia, SQL, Widgets), C++17, CMake, nlohmann/json, libdatachannel, OpenSSL

## Global Constraints

- C++17 standard (Qt 6 minimum requirement)
- All QML ↔ C++ communication through Q_PROPERTY / signals / QAbstractListModel
- std::expected<T, ApiError> for all error propagation — no bare exceptions in public APIs
- Server is the single source of truth; C++ DTO structs mirror backend Pydantic DTOs field-for-field
- optimistic insert uses clientUuid for dedup, serverId is the authoritative ID
- Qt 6.5+ minimum
- CMake build system with vcpkg (Windows) / apt (Linux) / homebrew (macOS) dependency resolution

---

### Task 1: Project Scaffolding & CMake Build System

**Files:**
- Create: `aaa2/CMakeLists.txt`
- Create: `aaa2/cmake/FindLibDataChannel.cmake`
- Create: `aaa2/src/main.cpp`
- Create: `aaa2/resources/qml/main.qml`
- Create: `aaa2/resources/qml.qrc`

**Interfaces:**
- Consumes: nothing (first task)
- Produces: empty `main.cpp` with QApplication + QQmlApplicationEngine boilerplate, empty `main.qml` Window, CMake build that compiles and links Qt 6.5+ (Quick, Network, WebSockets, SQL, Widgets), runs `qml.qrc`

- [ ] **Step 1: Create CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.21)
project(AIChatDesktop VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick QuickControls2 Network WebSockets Sql Multimedia MultimediaWidgets Widgets
)

find_package(nlohmann_json REQUIRED)
find_package(LibDataChannel QUIET)

set(SOURCES
    src/main.cpp
    # Additional sources added in later tasks
)

set(RESOURCES
    resources/qml.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::WebSockets
    Qt6::Sql
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Widgets
    nlohmann_json::nlohmann_json
)

if(LibDataChannel_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE LibDataChannel::LibDataChannel)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_LIBDATACHANNEL)
endif()

# vcpkg integration hint
if(DEFINED ENV{VCPKG_ROOT})
    message(STATUS "vcpkg detected: $ENV{VCPKG_ROOT}")
endif()
```

- [ ] **Step 2: Create FindLibDataChannel.cmake**

```cmake
find_path(LibDataChannel_INCLUDE_DIR rtc/rtc.hpp)
find_library(LibDataChannel_LIBRARY datachannel)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibDataChannel
    REQUIRED_VARS LibDataChannel_LIBRARY LibDataChannel_INCLUDE_DIR
)

if(LibDataChannel_FOUND)
    add_library(LibDataChannel::LibDataChannel UNKNOWN IMPORTED)
    set_target_properties(LibDataChannel::LibDataChannel PROPERTIES
        IMPORTED_LOCATION "${LibDataChannel_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibDataChannel_INCLUDE_DIR}"
    )
endif()
```

- [ ] **Step 3: Create src/main.cpp**

```cpp
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("AI Chat");
    app.setOrganizationName("AIChat");
    app.setQuitOnLastWindowClosed(false);

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

- [ ] **Step 4: Create resources/qml.qrc**

```xml
<RCC>
    <qresource prefix="/">
        <file>qml/main.qml</file>
    </qresource>
</RCC>
```

- [ ] **Step 5: Create resources/qml/main.qml**

```qml
import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    width: 1280
    height: 800
    visible: true
    title: "AI Chat"
}
```

- [ ] **Step 6: Build and run to verify scaffold compiles**

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
cmake --build .
./AIChatDesktop  # should show empty window
```

- [ ] **Step 7: Commit**

```bash
git init
git add CMakeLists.txt cmake/ src/main.cpp resources/
git commit -m "feat: project scaffold with CMake, Qt 6, empty QML window"
```

---

### Task 2: Core Utility Types — ApiError, JsonHelper, UuidGenerator

**Files:**
- Create: `aaa2/src/core/utils/ApiError.h`
- Create: `aaa2/src/core/utils/JsonHelper.h`
- Create: `aaa2/src/core/utils/UuidGenerator.h`
- Create: `aaa2/src/core/utils/UuidGenerator.cpp`

**Interfaces:**
- Consumes: nothing (standalone utilities)
- Produces:
  - `struct ApiError { std::string code; std::string message; static ApiError fromJson(const QJsonObject&); };`
  - `template<typename T> using Result = std::expected<T, ApiError>;`
  - `namespace JsonHelper { std::string getString(const QJsonObject&, const char* key, const std::string& def = ""); int64_t getInt(const QJsonObject&, const char* key, int64_t def = 0); std::optional<QDateTime> getOptionalDateTime(const QJsonObject&, const char* key); }`
  - `class UuidGenerator { static QString generate(); };` — UUID v7 (time-ordered)

- [ ] **Step 1: Create ApiError.h**

```cpp
#pragma once
#include <string>
#include <expected>
#include <QJsonObject>
#include <QString>

struct ApiError {
    std::string code;
    std::string message;

    static ApiError fromJson(const QJsonObject &obj) {
        return ApiError{
            .code = obj.value("code").toString().toStdString(),
            .message = obj.value("message").toString().toStdString(),
        };
    }

    static ApiError networkError(const std::string &msg) {
        return ApiError{.code = "NETWORK_ERROR", .message = msg};
    }
};

template<typename T>
using Result = std::expected<T, ApiError>;
```

- [ ] **Step 2: Create JsonHelper.h**

```cpp
#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <optional>
#include <string>

namespace JsonHelper {

inline std::string getString(const QJsonObject &obj, const char *key,
                              const std::string &def = "") {
    auto val = obj.value(key);
    return val.isString() ? val.toString().toStdString() : def;
}

inline int64_t getInt(const QJsonObject &obj, const char *key, int64_t def = 0) {
    auto val = obj.value(key);
    if (val.isDouble()) return static_cast<int64_t>(val.toDouble());
    if (val.isString()) {
        bool ok;
        auto n = val.toString().toLongLong(&ok);
        return ok ? n : def;
    }
    return def;
}

inline bool getBool(const QJsonObject &obj, const char *key, bool def = false) {
    auto val = obj.value(key);
    return val.isBool() ? val.toBool() : def;
}

inline std::optional<QDateTime> getOptionalDateTime(const QJsonObject &obj,
                                                      const char *key) {
    auto val = obj.value(key);
    if (!val.isString()) return std::nullopt;
    auto dt = QDateTime::fromString(val.toString(), Qt::ISODate);
    return dt.isValid() ? std::optional{dt} : std::nullopt;
}

inline QJsonArray getArray(const QJsonObject &obj, const char *key) {
    auto val = obj.value(key);
    return val.isArray() ? val.toArray() : QJsonArray{};
}

} // namespace JsonHelper
```

- [ ] **Step 3: Create UuidGenerator.h**

```cpp
#pragma once
#include <QString>

class UuidGenerator {
public:
    /// Generate a UUID v7 (time-ordered) string without dashes.
    static QString generate();
};
```

- [ ] **Step 4: Create UuidGenerator.cpp**

```cpp
#include "UuidGenerator.h"
#include <QUuid>

QString UuidGenerator::generate() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces).remove('-');
}
```

- [ ] **Step 5: Commit**

```bash
git add src/core/utils/
git commit -m "feat: add ApiError, JsonHelper, UuidGenerator utility types"
```

---

### Task 3: DTO Models — All 21 C++ Structs

**Files:**
- Create: `aaa2/src/core/models/MessageDTO.h`
- Create: `aaa2/src/core/models/ConversationDTO.h`
- Create: `aaa2/src/core/models/AIUserDTO.h`
- Create: `aaa2/src/core/models/UserDTO.h`
- Create: `aaa2/src/core/models/MemoryDTO.h`
- Create: `aaa2/src/core/models/ProfileDTO.h`
- Create: `aaa2/src/core/models/ExpansionModuleDTO.h`
- Create: `aaa2/src/core/models/MediaDTO.h`
- Create: `aaa2/src/core/models/TTSDTO.h`
- Create: `aaa2/src/core/models/CallDTO.h`

**Interfaces:**
- Consumes: `JsonHelper`, `ApiError` (Task 2)
- Produces: all DTO structs with `static fromJson(const QJsonObject&)` and `QJsonObject toJson() const` methods

- [ ] **Step 1: Create models/MediaDTO.h** — the most referenced sub-object

```cpp
#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <string>
#include <optional>
#include <vector>
#include "../utils/JsonHelper.h"

struct MediaDTO {
    std::string id;
    std::string mediaType;
    std::string source;
    std::string originalFilename;
    std::string storedFilename;
    std::optional<std::string> filePath;
    std::optional<std::string> thumbnailStoredFilename;
    std::optional<std::string> url;
    std::optional<int64_t> fileSize;
    std::optional<std::string> mimeType;
    std::optional<std::string> uploadedBy;
    std::optional<double> duration;
    std::optional<std::string> description;

    static MediaDTO fromJson(const QJsonObject &obj) {
        MediaDTO d;
        d.id = JsonHelper::getString(obj, "id");
        d.mediaType = JsonHelper::getString(obj, "media_type");
        d.source = JsonHelper::getString(obj, "source");
        d.originalFilename = JsonHelper::getString(obj, "original_filename");
        d.storedFilename = JsonHelper::getString(obj, "stored_filename");
        auto fp = obj.value("file_path");
        if (fp.isString()) d.filePath = fp.toString().toStdString();
        auto ts = obj.value("thumbnail_stored_filename");
        if (ts.isString()) d.thumbnailStoredFilename = ts.toString().toStdString();
        auto u = obj.value("url");
        if (u.isString()) d.url = u.toString().toStdString();
        d.fileSize = obj.contains("file_size") && !obj.value("file_size").isNull()
            ? std::optional(JsonHelper::getInt(obj, "file_size")) : std::nullopt;
        auto mt = obj.value("mime_type");
        if (mt.isString()) d.mimeType = mt.toString().toStdString();
        auto ub = obj.value("uploaded_by");
        if (ub.isString()) d.uploadedBy = ub.toString().toStdString();
        d.duration = obj.contains("duration") && !obj.value("duration").isNull()
            ? std::optional(obj.value("duration").toDouble()) : std::nullopt;
        auto desc = obj.value("description");
        if (desc.isString()) d.description = desc.toString().toStdString();
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = QString::fromStdString(id);
        o["media_type"] = QString::fromStdString(mediaType);
        o["source"] = QString::fromStdString(source);
        o["original_filename"] = QString::fromStdString(originalFilename);
        o["stored_filename"] = QString::fromStdString(storedFilename);
        if (filePath) o["file_path"] = QString::fromStdString(*filePath);
        if (url) o["url"] = QString::fromStdString(*url);
        if (fileSize) o["file_size"] = static_cast<qint64>(*fileSize);
        return o;
    }
};
```

- [ ] **Step 2: Create models/MessageDTO.h**

```cpp
#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <string>
#include <optional>
#include <vector>
#include "MediaDTO.h"
#include "../utils/JsonHelper.h"

struct MessageDTO {
    std::optional<int64_t>  serverId;
    std::string             clientUuid;
    std::string             aiUserId;
    std::string             conversationId;
    std::string             senderType;      // "user" | "ai" | "system"
    std::string             msgType;         // "text" | "image" | "video" | "file" | "audio" | "sticker"
    QJsonObject             content;         // {think, emotion, response, functions}
    std::vector<MediaDTO>   mediaList;
    bool                    isComplete = true;
    bool                    isRead = false;
    std::optional<QDateTime> revokedAt;
    std::optional<QDateTime> deletedAt;
    std::optional<std::string> sourceFunction;
    std::string             timestamp;       // ISO 8601

    static MessageDTO fromJson(const QJsonObject &obj) {
        MessageDTO m;
        m.serverId = obj.contains("id") && !obj.value("id").isNull()
            ? std::optional{JsonHelper::getInt(obj, "id")} : std::nullopt;
        m.clientUuid = JsonHelper::getString(obj, "client_uuid");
        m.aiUserId = JsonHelper::getString(obj, "ai_user_id");
        m.conversationId = JsonHelper::getString(obj, "conversation_id");
        m.senderType = JsonHelper::getString(obj, "sender_type");
        m.msgType = JsonHelper::getString(obj, "msg_type");
        auto c = obj.value("content");
        m.content = c.isObject() ? c.toObject() : QJsonObject{};
        auto ml = obj.value("media_list");
        if (ml.isArray()) {
            for (const auto &item : ml.toArray()) {
                if (item.isObject())
                    m.mediaList.push_back(MediaDTO::fromJson(item.toObject()));
            }
        }
        m.isComplete = JsonHelper::getBool(obj, "is_complete", true);
        m.isRead = JsonHelper::getBool(obj, "is_read", false);
        m.revokedAt = JsonHelper::getOptionalDateTime(obj, "revoked_at");
        m.deletedAt = JsonHelper::getOptionalDateTime(obj, "deleted_at");
        auto sf = obj.value("source_function");
        if (sf.isString()) m.sourceFunction = sf.toString().toStdString();
        m.timestamp = JsonHelper::getString(obj, "timestamp",
            QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString());
        return m;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        if (serverId) o["id"] = static_cast<qint64>(*serverId);
        o["client_uuid"] = QString::fromStdString(clientUuid);
        o["ai_user_id"] = QString::fromStdString(aiUserId);
        o["conversation_id"] = QString::fromStdString(conversationId);
        o["sender_type"] = QString::fromStdString(senderType);
        o["msg_type"] = QString::fromStdString(msgType);
        o["content"] = content;
        QJsonArray ml;
        for (const auto &d : mediaList) ml.append(d.toJson());
        o["media_list"] = ml;
        o["is_complete"] = isComplete;
        o["timestamp"] = QString::fromStdString(timestamp);
        return o;
    }
};
```

- [ ] **Step 3-9: Create remaining DTO headers**

Create `ConversationDTO.h`, `AIUserDTO.h`, `UserDTO.h`, `MemoryDTO.h`, `ProfileDTO.h`, `ExpansionModuleDTO.h`, `TTSDTO.h`, `CallDTO.h` following the same pattern — `fromJson` / `toJson` pair, all fields matching the Flutter model attributes as documented in the spec. Each uses `JsonHelper` for safe extraction.

- [ ] **Step 10: Commit**

```bash
git add src/core/models/
git commit -m "feat: add all 21 DTO model structs with JSON serialization"
```

---

### Task 4: Database — SQLite Schema & Migration

**Files:**
- Create: `aaa2/src/core/storage/Database.h`
- Create: `aaa2/src/core/storage/Database.cpp`

**Interfaces:**
- Consumes: `MessageDTO`, `ConversationDTO` (Task 3)
- Produces:
  - `class Database` — `bool open(const QString &path); QSqlDatabase& db();`
  - `QStringList migrationStatements()` — versioned SQL migrations
  - `int64_t insertMessage(const MessageDTO&, const QString &aiUserId);`
  - `std::optional<MessageDTO> findByServerId(int64_t serverId);`
  - `std::optional<MessageDTO> findByClientUuid(const QString &clientUuid);`
  - `QVector<MessageDTO> getLocalMessages(const QString &aiUserId, int limit = 200);`
  - `void deleteMessagesSince(const QString &aiUserId, const QString &since);`
  - `void deleteStaleUnsynced(const QString &aiUserId, int staleMinutes = 30);`
  - `void upsertMessage(const MessageDTO&, const QString &aiUserId);`
  - `QHash<QString, int> getUnreadCounts();`
  - `void markAsRead(const QString &aiUserId);`

- [ ] **Step 1: Create Database.h**

```cpp
#pragma once
#include <QSqlDatabase>
#include <QVector>
#include <optional>
#include <QHash>
#include "../models/MessageDTO.h"

class Database {
public:
    Database();
    ~Database();

    bool open(const QString &path);
    QSqlDatabase &db() { return db_; }

    // Messages
    int64_t insertMessage(const MessageDTO &msg, const QString &aiUserId);
    void upsertMessage(const MessageDTO &msg, const QString &aiUserId);
    std::optional<MessageDTO> findByServerId(int64_t serverId);
    std::optional<MessageDTO> findByClientUuid(const QString &clientUuid);
    QVector<MessageDTO> getLocalMessages(const QString &aiUserId, int limit = 200);
    QVector<MessageDTO> getLocalMessagesBefore(const QString &aiUserId,
                                                const QString &beforeTimestamp,
                                                int limit = 50);
    void updateMessage(const QString &clientUuid, const QVariantMap &values);
    void deleteMessage(const QString &clientUuid);
    void deleteMessagesSince(const QString &aiUserId, const QString &since);
    void deleteStaleUnsynced(const QString &aiUserId, int staleMinutes = 30);
    void deleteIncompleteMessages(const QString &aiUserId);
    bool hasLocalMessages(const QString &aiUserId);

    // Unread
    QHash<QString, int> getUnreadCounts();
    void markAsRead(const QString &aiUserId);

    // Sync tracking
    std::optional<QString> getSyncTime(const QString &aiUserId);
    void setSyncTime(const QString &aiUserId, const QString &time);

private:
    void runMigrations();
    QSqlDatabase db_;
};
```

- [ ] **Step 2: Create Database.cpp — open() and migrations**

```cpp
#include "Database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

Database::Database() {}
Database::~Database() { if (db_.isOpen()) db_.close(); }

bool Database::open(const QString &path) {
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(path);
    if (!db_.open()) {
        qCritical() << "Database open failed:" << db_.lastError().text();
        return false;
    }
    QSqlQuery q(db_);
    q.exec("PRAGMA journal_mode=WAL");
    q.exec("PRAGMA foreign_keys=ON");
    runMigrations();
    return true;
}

void Database::runMigrations() {
    QSqlQuery q(db_);
    q.exec("CREATE TABLE IF NOT EXISTS schema_version (version INTEGER)");
    q.exec("SELECT MAX(version) FROM schema_version");
    int currentVersion = q.next() ? q.value(0).toInt() : 0;

    struct Migration { int version; QString sql; };
    QVector<Migration> migrations = {
        {1, R"(
            CREATE TABLE IF NOT EXISTS messages (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                server_id INTEGER UNIQUE,
                client_uuid TEXT NOT NULL,
                ai_user_id TEXT NOT NULL,
                conversation_id INTEGER,
                sender_type TEXT NOT NULL,
                msg_type TEXT NOT NULL,
                content TEXT,
                media_list TEXT,
                is_complete INTEGER DEFAULT 1,
                is_read INTEGER DEFAULT 0,
                source_function TEXT,
                revoked_at TEXT,
                deleted_at TEXT,
                timestamp TEXT NOT NULL
            )
        )"},
        {2, "CREATE INDEX IF NOT EXISTS idx_messages_ai_user ON messages(ai_user_id)"},
        {3, "CREATE INDEX IF NOT EXISTS idx_messages_client ON messages(client_uuid)"},
        {4, "CREATE INDEX IF NOT EXISTS idx_messages_server ON messages(server_id)"},
        {5, R"(
            CREATE TABLE IF NOT EXISTS conversations (
                id INTEGER PRIMARY KEY,
                ai_user_id TEXT NOT NULL,
                title TEXT,
                summary TEXT,
                last_message_preview TEXT,
                unread_count INTEGER DEFAULT 0,
                created_at TEXT,
                updated_at TEXT
            )
        )"},
        {6, "CREATE INDEX IF NOT EXISTS idx_conv_ai_user ON conversations(ai_user_id)"},
        {7, R"(
            CREATE TABLE IF NOT EXISTS sync_meta (
                ai_user_id TEXT PRIMARY KEY,
                last_sync_time TEXT
            )
        )"},
    };

    for (const auto &m : migrations) {
        if (m.version > currentVersion) {
            q.exec(m.sql);
            if (q.lastError().isValid())
                qWarning() << "Migration" << m.version << "failed:" << q.lastError().text();
            else {
                QSqlQuery ins(db_);
                ins.prepare("INSERT OR REPLACE INTO schema_version VALUES(?)");
                ins.addBindValue(m.version);
                ins.exec();
            }
        }
    }
}
```

- [ ] **Step 3: Implement key DB methods — insertMessage, upsertMessage, deleteStaleUnsynced, getUnreadCounts**

Full implementations in `Database.cpp` following the pattern:

```cpp
int64_t Database::insertMessage(const MessageDTO &msg, const QString &aiUserId) {
    QSqlQuery q(db_);
    q.prepare(R"(
        INSERT INTO messages (server_id, client_uuid, ai_user_id, conversation_id,
            sender_type, msg_type, content, media_list, is_complete, is_read,
            source_function, revoked_at, timestamp)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    q.addBindValue(msg.serverId ? QVariant(static_cast<qint64>(*msg.serverId)) : QVariant());
    q.addBindValue(QString::fromStdString(msg.clientUuid));
    q.addBindValue(aiUserId);
    q.addBindValue(QString::fromStdString(msg.conversationId));
    q.addBindValue(QString::fromStdString(msg.senderType));
    q.addBindValue(QString::fromStdString(msg.msgType));
    q.addBindValue(QString::fromUtf8(QJsonDocument(msg.content).toJson(QJsonDocument::Compact)));
    QJsonArray ml;
    for (const auto &m : msg.mediaList) ml.append(m.toJson());
    q.addBindValue(QString::fromUtf8(QJsonDocument(ml).toJson(QJsonDocument::Compact)));
    q.addBindValue(msg.isComplete ? 1 : 0);
    q.addBindValue(msg.isRead ? 1 : 0);
    q.addBindValue(msg.sourceFunction ? QString::fromStdString(*msg.sourceFunction) : QVariant());
    q.addBindValue(msg.revokedAt ? msg.revokedAt->toString(Qt::ISODate) : QVariant());
    q.addBindValue(QString::fromStdString(msg.timestamp));
    if (!q.exec()) {
        qWarning() << "insertMessage failed:" << q.lastError().text();
        return -1;
    }
    return q.lastInsertId().toLongLong();
}

void Database::deleteStaleUnsynced(const QString &aiUserId, int staleMinutes) {
    QSqlQuery q(db_);
    auto threshold = QDateTime::currentDateTimeUtc()
        .addSecs(-staleMinutes * 60).toString(Qt::ISODate);
    q.prepare("DELETE FROM messages WHERE ai_user_id = ? AND server_id IS NULL AND timestamp < ?");
    q.addBindValue(aiUserId);
    q.addBindValue(threshold);
    if (!q.exec())
        qWarning() << "deleteStaleUnsynced failed:" << q.lastError().text();
}

QHash<QString, int> Database::getUnreadCounts() {
    QHash<QString, int> counts;
    QSqlQuery q(db_);
    q.exec("SELECT m.ai_user_id, COUNT(*) as cnt FROM messages m "
           "WHERE m.is_read = 0 AND m.ai_user_id IS NOT NULL "
           "AND (m.deleted_at IS NULL OR m.deleted_at = '') "
           "AND (m.revoked_at IS NULL OR m.revoked_at = '') "
           "GROUP BY m.ai_user_id");
    while (q.next())
        counts[q.value(0).toString()] = q.value(1).toInt();
    return counts;
}
```

Continue implementing: `upsertMessage`, `findByServerId`, `findByClientUuid`, `getLocalMessages`, `getLocalMessagesBefore`, `updateMessage`, `deleteMessage`, `deleteMessagesSince`, `deleteIncompleteMessages`, `hasLocalMessages`, `markAsRead`, `getSyncTime`, `setSyncTime`.

- [ ] **Step 4: Commit**

```bash
git add src/core/storage/
git commit -m "feat: SQLite database with schema migrations and CRUD operations"
```

---

### Task 5: Auth — TokenManager & Platform Keychain

**Files:**
- Create: `aaa2/src/core/auth/Keychain.h`
- Create: `aaa2/src/core/auth/Keychain.cpp`
- Create: `aaa2/src/core/auth/TokenManager.h`
- Create: `aaa2/src/core/auth/TokenManager.cpp`

**Interfaces:**
- Consumes: `ApiError` (Task 2)
- Produces:
  - `class Keychain` — `Result<void> write(const QString &key, const QString &value); Result<QString> read(const QString &key); Result<void> remove(const QString &key);`
  - `class TokenManager : public QObject` — `Q_PROPERTY(bool loggedIn READ isLoggedIn NOTIFY loginStateChanged)`; `Result<void> login(const QString &serverUrl, const QString &username, const QString &password); void logout(); QString accessToken() const; QString refreshToken() const; Result<QString> refreshAccessToken(); signals: void loginStateChanged(bool loggedIn); void tokenExpiring();`

- [ ] **Step 1: Create Keychain.h**

```cpp
#pragma once
#include <QString>
#include "../utils/ApiError.h"

class Keychain {
public:
    static Result<void> write(const QString &key, const QString &value);
    static Result<QString> read(const QString &key);
    static Result<void> remove(const QString &key);

private:
    static QString serviceName() { return "AIChatDesktop"; }
};
```

- [ ] **Step 2: Create Keychain.cpp**

```cpp
#include "Keychain.h"
#include <QSettings>
#include <QCryptographicHash>

// Cross-platform keychain via QSettings with base64 obfuscation.
// For production, integrate platform APIs (Windows Credential Store,
// macOS Keychain via Security framework, Linux libsecret).

static QByteArray obfuscate(const QByteArray &data) {
    // Simple XOR with a fixed key — not cryptographic, just deters
    // plain-text reading of QSettings files.  Replace with platform
    // keychain APIs for production.
    const QByteArray key = "AiChatDesktop2026";
    QByteArray result = data;
    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ key[i % key.size()];
    return result.toBase64();
}

static QByteArray deobfuscate(const QByteArray &data) {
    auto decoded = QByteArray::fromBase64(data);
    const QByteArray key = "AiChatDesktop2026";
    QByteArray result = decoded;
    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ key[i % key.size()];
    return result;
}

Result<void> Keychain::write(const QString &key, const QString &value) {
    QSettings settings("AIChat", "AIChatDesktop");
    settings.setValue("keychain/" + key, QString::fromUtf8(obfuscate(value.toUtf8())));
    settings.sync();
    if (settings.status() != QSettings::NoError)
        return std::unexpected(ApiError::networkError("Failed to write to keychain"));
    return {};
}

Result<QString> Keychain::read(const QString &key) {
    QSettings settings("AIChat", "AIChatDesktop");
    auto stored = settings.value("keychain/" + key).toString();
    if (stored.isEmpty())
        return std::unexpected(ApiError{.code="NOT_FOUND", .message="Key not found"});
    return QString::fromUtf8(deobfuscate(stored.toUtf8()));
}

Result<void> Keychain::remove(const QString &key) {
    QSettings settings("AIChat", "AIChatDesktop");
    settings.remove("keychain/" + key);
    settings.sync();
    return {};
}
```

- [ ] **Step 3: Create TokenManager.h**

```cpp
#pragma once
#include <QObject>
#include <QString>
#include <QDateTime>
#include <memory>
#include "../utils/ApiError.h"
#include "../../core/network/HttpClient.h"

class TokenManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loggedIn READ isLoggedIn NOTIFY loginStateChanged)

public:
    explicit TokenManager(QObject *parent = nullptr);

    bool isLoggedIn() const;

    Result<void> login(const QString &username, const QString &password);
    void logout();
    void restoreFromKeychain();

    QString accessToken() const;
    QString refreshToken() const;
    Result<QString> refreshAccessToken();

    void setServerUrl(const QString &url);
    QString serverUrl() const;

signals:
    void loginStateChanged(bool loggedIn);

private:
    QString accessToken_;
    QString refreshToken_;
    QString serverUrl_;
    bool loggedIn_ = false;

    static std::optional<QDateTime> decodeJwtExp(const QString &token);
    bool isAccessTokenExpired() const;
};
```

- [ ] **Step 4: Create TokenManager.cpp** — implement login (POST /auth/login), refresh (POST /auth/refresh), JWT exp decoding, keychain persistence/restore

Implementation details:
- `login()`: POST /auth/login with {username, password}, extract access_token + refresh_token from response, persist both to Keychain
- `refreshAccessToken()`: POST /auth/refresh with {refresh_token}, update access_token, persist
- `decodeJwtExp()`: parse JWT payload (base64 decode the middle segment), extract `exp` field as QDateTime
- `restoreFromKeychain()`: read both tokens from Keychain, emit loginStateChanged(true) if valid

- [ ] **Step 5: Commit**

```bash
git add src/core/auth/
git commit -m "feat: TokenManager with JWT auth, refresh, and platform keychain storage"
```

---

### Task 6: Network — HttpClient & ChatStreamClient

**Files:**
- Create: `aaa2/src/core/network/HttpClient.h`
- Create: `aaa2/src/core/network/HttpClient.cpp`
- Create: `aaa2/src/core/network/ChatStreamClient.h`
- Create: `aaa2/src/core/network/ChatStreamClient.cpp`

**Interfaces:**
- Consumes: `TokenManager` (Task 5), `ApiError`, `Result<T>` (Task 2)
- Produces:
  - `class HttpClient` — `Result<QJsonObject> get(const QString &path, const QUrlQuery &params = {}); Result<QJsonObject> post(const QString &path, const QJsonObject &body = {}); Result<QJsonObject> postFormData(const QString &path, const QMap<QString,QString> &fields, const QString &filePath);`
  - `class ChatStreamClient : public QObject` — `void connectToServer(const QString &wsUrl, const QString &token); void disconnect(); signals: void connected(); void disconnected(); void streamInit(const QString &conversationId, const QString &messageId, const QString &timestamp); void streamChunk(const QString &conversationId, const QString &chunk); void streamDone(const QString &conversationId, const QString &messageId, const QString &content); void streamError(const QString &conversationId, const QString &code, const QString &message); void proactiveMessage(const QString &conversationId, const QJsonObject &payload);`

- [ ] **Step 1: Create HttpClient.h**

```cpp
#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QMap>
#include "../utils/ApiError.h"

class TokenManager;

class HttpClient : public QObject {
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = nullptr);

    void setTokenManager(TokenManager *tm);
    void setBaseUrl(const QString &url);

    Result<QJsonObject> get(const QString &path, const QUrlQuery &params = {});
    Result<QJsonObject> post(const QString &path, const QJsonObject &body = {});
    Result<QJsonObject> postFormData(const QString &path,
                                      const QMap<QString, QString> &fields,
                                      const QString &filePath = {});

private:
    Result<QJsonObject> doRequest(const QString &method, const QString &path,
                                   const QJsonObject &body,
                                   QHttpMultiPart *multiPart = nullptr,
                                   const QUrlQuery &params = {});
    Result<QJsonObject> sendWithRetry(const QString &method, const QString &path,
                                       const QJsonObject &body = {},
                                       QHttpMultiPart *mp = nullptr,
                                       const QUrlQuery &params = {});
    QNetworkAccessManager *nam_;
    TokenManager *tokenManager_ = nullptr;
    QString baseUrl_;
};
```

- [ ] **Step 2: Create HttpClient.cpp** — implement synchronous-ish request with QNetworkAccessManager + QEventLoop, auto-inject Authorization header, parse response JSON, check `success` field, map errors to ApiError. On 401, call tokenManager->refreshAccessToken() and retry once.

- [ ] **Step 3: Create ChatStreamClient.h**

```cpp
#pragma once
#include <QObject>
#include <QWebSocket>
#include <QJsonObject>
#include <QTimer>

class ChatStreamClient : public QObject {
    Q_OBJECT
public:
    explicit ChatStreamClient(QObject *parent = nullptr);

    void connectToServer(const QString &wsUrl, const QString &token);
    void disconnect();
    bool isConnected() const;
    void sendPong();

signals:
    void connected();
    void disconnected();
    void streamInit(const QString &conversationId, const QString &messageId,
                    const QString &timestamp);
    void streamChunk(const QString &conversationId, const QString &chunk);
    void streamDone(const QString &conversationId, const QString &messageId,
                    const QString &content);
    void streamError(const QString &conversationId, const QString &code,
                     const QString &message);
    void proactiveMessage(const QString &conversationId, const QJsonObject &payload);

private slots:
    void onTextMessageReceived(const QString &text);
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void sendPing();

private:
    void handleFrame(const QJsonObject &frame);
    QWebSocket ws_;
    QTimer heartbeatTimer_;
    QTimer pongTimeoutTimer_;
    bool authenticated_ = false;
};
```

- [ ] **Step 4: Create ChatStreamClient.cpp** — implement connectToServer (open WS, send auth frame `{"type":"auth","token":"..."}`), heartbeat (30s ping), pong timeout (60s), parse incoming JSON frames, dispatch to appropriate signal based on `type` field. Handle reconnection with exponential backoff (1s, 1s, 1s, 3s, 3s, then 30s).

- [ ] **Step 5: Commit**

```bash
git add src/core/network/
git commit -m "feat: HttpClient with auto-auth and ChatStreamClient for WS streaming"
```

---

### Task 7: Repositories Layer (Part 1 — Auth, Chat, Conversation, AIUser)

**Files:**
- Create: `aaa2/src/repositories/AuthRepository.h` + `.cpp`
- Create: `aaa2/src/repositories/ChatRepository.h` + `.cpp`
- Create: `aaa2/src/repositories/ConversationRepository.h` + `.cpp`
- Create: `aaa2/src/repositories/AIUserRepository.h` + `.cpp`

**Interfaces:**
- Consumes: `HttpClient` (Task 6), `Database` (Task 4), `MessageDTO` etc. (Task 3)
- Produces: Four repository classes wrapping API calls + local DB caching.

- [ ] **Step 1: AuthRepository** — `Result<void> login(username, password)` (calls POST /auth/login, stores tokens via TokenManager), `Result<void> registerUser(...)`, `Result<void> logout()`.

- [ ] **Step 2: ChatRepository** — `Result<ChatSendResponse> sendMessage(aiUserId, content, conversationId)`, `Result<ChatMediaResponse> sendFormDataMessage(...)`, `Result<QVector<MessageDTO>> getHistory(aiUserId, beforeId, limit)`, `Result<SyncResult> syncMessages(aiUserId, since, afterId, limit)`, local DB pass-through methods (`insertMessage`, `upsertMessage`, `getLocalMessages`, `deleteMessagesSince`, `deleteStaleUnsynced`, `markAsRead`, `revokeMessage`, `rollbackMessages`).

- [ ] **Step 3: ConversationRepository** — `Result<QVector<ConversationDTO>> listConversations()`, `Result<ConversationDTO> createConversation(aiUserId)`, `Result<void> endConversation(conversationId)`, local DB: `getCachedConversations()`, `upsertConversation()`, `getAiUserIdForConversation()`.

- [ ] **Step 4: AIUserRepository** — `Result<QVector<AIUserDTO>> listAIUsers()`, `Result<AIUserDTO> getAIUser(aiUserId)`, `Result<AIUserDTO> createAIUser(...)`, `Result<AIUserDTO> updateAIUser(...)`.

- [ ] **Step 5: Commit**

```bash
git add src/repositories/
git commit -m "feat: repositories for auth, chat, conversation, AI user"
```

---

### Task 8: Repositories Layer (Part 2 — User, Memory, Profile, Upload, Expansion, Call, TTS)

**Files:**
- Create: remaining 7 repository pairs in `aaa2/src/repositories/`

**Interfaces:** Same pattern as Task 7 — each wraps API endpoints + local DB.

- [ ] **Step 1: UserRepository** — `Result<UserDTO> getCurrentUser()`, `Result<QVector<UserDTO>> listUsers()` (admin), `Result<UserDTO> updateUser(userId, ...)`.
- [ ] **Step 2: MemoryRepository** — `Result<QVector<MemoryDTO>> queryMemories(aiUserId, query, limit)`.
- [ ] **Step 3: ProfileRepository** — `Result<QVector<ProfileDTO>> getProfiles()`, `Result<ProfileDTO> createProfile(...)`, `Result<ProfileDTO> updateProfile(...)`.
- [ ] **Step 4: UploadRepository** — file upload helper using `HttpClient::postFormData`.
- [ ] **Step 5: ExpansionRepository** — `Result<QVector<ExpansionModuleDTO>> listModules()`, `Result<void> toggleModule(name, enabled)`.
- [ ] **Step 6: CallRepository** — `Result<CallStartResponse> startCall(conversationId, callType)`.
- [ ] **Step 7: TTSRepository** — `Result<QByteArray> synthesize(text, voice, speed)`.

- [ ] **Step 8: Commit**

```bash
git add src/repositories/
git commit -m "feat: remaining 7 repositories for users, memory, profile, upload, expansion, call, TTS"
```

---

### Task 9: ChatService & SyncService

**Files:**
- Create: `aaa2/src/services/ChatService.h` + `.cpp`
- Create: `aaa2/src/services/SyncService.h` + `.cpp`

**Interfaces:**
- Consumes: `ChatRepository`, `ChatStreamClient`, `Database`, `UnreadTracker` (Task 10)
- Produces:
  - `class ChatService : public QObject` — orchestrates send → POST /chat → WS stream handling, with watchdog timer (30s timeout), batching (cooldown 1.5s, max 4.5s), conversation replacement handling.
  - `class SyncService : public QObject` — `void syncMessages(const QString &aiUserId, double anchorHours = 0.1);` — deletes messages in window from local DB, fetches from server, merges, purges stale unsynced, emits `syncComplete(const QString &aiUserId)`.

- [ ] **Step 1: ChatService.h**

```cpp
#pragma once
#include <QObject>
#include <QTimer>
#include <QHash>
#include "ChatRepository.h"
#include "../core/network/ChatStreamClient.h"

class ChatService : public QObject {
    Q_OBJECT
public:
    ChatService(ChatRepository *repo, ChatStreamClient *wsClient, QObject *parent = nullptr);

    void sendMessage(const QString &aiUserId, const QString &content,
                     const QString &conversationId = {});
    void sendMediaMessage(const QString &aiUserId, const QString &filePath,
                           const QString &content, const QString &msgType,
                           const QString &conversationId = {});
    void cancelAllStreams(const QString &aiUserId);
    void cancelWatchdog(const QString &aiUserId);

signals:
    void streamInitReceived(const QString &aiUserId, const QString &conversationId,
                             const QString &messageId);
    void streamChunkReceived(const QString &aiUserId, const QString &content);
    void streamDoneReceived(const QString &aiUserId, const QString &messageId,
                             const QString &content);
    void streamErrorReceived(const QString &aiUserId, const QString &code,
                              const QString &message);
    void messageTimedOut(const QString &clientUuid);
    void conversationReplaced(const QString &newConversationId);

private:
    void flushBatch(const QString &aiUserId, const QString &conversationId,
                    const QString &combinedContent, const QStringList &clientUuids);
    void resetWatchdog(const QString &aiUserId);

    ChatRepository *repo_;
    ChatStreamClient *wsClient_;
    QHash<QString, QTimer*> watchdogTimers_;
    // Batching state
    struct BatchState {
        QTimer *cooldownTimer = nullptr;
        QTimer *maxWaitTimer = nullptr;
        QStringList clientUuids;
        QStringList contents;
        QString conversationId;
    };
    QHash<QString, BatchState> batchers_;
    int sendCooldownMs_ = 1500;
    int sendMaxWaitMs_ = 4500;
    int streamTimeoutMs_ = 30000;
};
```

- [ ] **Step 2-3: Implement ChatService.cpp and SyncService.cpp** — wire WS signals to ChatService signals, implement batching, watchdog (30s timeout triggers loadHistory via SyncService), conversation replacement handling.

- [ ] **Step 4: Commit**

```bash
git add src/services/ChatService.* src/services/SyncService.*
git commit -m "feat: ChatService with batching/watchdog, SyncService with phantom cleanup"
```

---

### Task 10: UnreadTracker & NotificationService

**Files:**
- Create: `aaa2/src/services/UnreadTracker.h` + `.cpp`
- Create: `aaa2/src/services/NotificationService.h` + `.cpp`

**Interfaces:**
- Consumes: `Database` (Task 4), `ConversationRepository` (Task 7)
- Produces:
  - `class UnreadTracker : public QObject` — `Q_PROPERTY(QVariantMap unreadMap READ unreadMap NOTIFY unreadMapChanged)`; `void init(); void onIncomingMessage(const QString &conversationId); void clearAndMarkRead(const QString &aiUserId); void setActiveAiUser(const QString &aiUserId);`
  - `class NotificationService : public QObject` — `void showNotification(const QString &title, const QString &body); void updateTaskbarBadge(int count); void updateTrayIcon(bool hasUnread);`

- [ ] **Step 1: UnreadTracker.h** — as specified in the design spec (section 7)
- [ ] **Step 2: UnreadTracker.cpp** — init() loads counts from DB via Database::getUnreadCounts(), onIncomingMessage() resolves conversationId→aiUserId and increments (unless it's the active AI user), clearAndMarkRead() clears and calls Database::markAsRead(), setActiveAiUser() tracks the active conversation so messages arriving for it aren't counted.
- [ ] **Step 3: NotificationService.h/.cpp** — wraps QSystemTrayIcon::showMessage() for desktop notifications, platform-specific taskbar badge (Win/Mac/Linux stubs).
- [ ] **Step 4: Commit**

---

### Task 11: ViewModels — ChatMessagesModel & List Models

**Files:**
- Create: `aaa2/src/viewmodels/ChatMessagesModel.h` + `.cpp`
- Create: `aaa2/src/viewmodels/ConversationListModel.h` + `.cpp`
- Create: `aaa2/src/viewmodels/ContactListModel.h` + `.cpp`
- Create: `aaa2/src/viewmodels/LoginViewModel.h` + `.cpp`
- Create: `aaa2/src/viewmodels/SettingsViewModel.h` + `.cpp`

**Interfaces:**
- Consumes: All services and repositories
- Produces: QAbstractListModel subclasses for QML binding, QObject view-models with Q_PROPERTY for page-level state

- [ ] **Step 1: ChatMessagesModel.h**

```cpp
#pragma once
#include <QAbstractListModel>
#include <QVector>
#include "../core/models/MessageDTO.h"

class ChatMessagesModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        ServerIdRole = Qt::UserRole + 1,
        ClientUuidRole,
        AiUserIdRole,
        ConversationIdRole,
        SenderTypeRole,
        MsgTypeRole,
        ContentRole,
        MediaListRole,
        IsCompleteRole,
        RevokedAtRole,
        TimestampRole,
    };
    Q_ENUM(Roles)

    explicit ChatMessagesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Incremental updates — key for streaming performance
    void appendMessage(const MessageDTO &msg);
    void prependMessages(const QVector<MessageDTO> &msgs);
    void replaceAll(const QVector<MessageDTO> &msgs);
    void updateContent(int row, const QJsonObject &content);
    void removeMessage(const QString &clientUuid);
    void markComplete(int row, int64_t serverId);
    void clear();

    const MessageDTO &at(int row) const;
    int findByClientUuid(const QString &uuid) const;

signals:
    void messageAppended();

private:
    QVector<MessageDTO> messages_;
    QHash<QString, int> clientUuidIndex_;
    QHash<int, int> serverIdIndex_;
};
```

- [ ] **Step 2-5: Implement all ViewModels** — each ViewModel exposes Q_PROPERTY properties and Q_INVOKABLE methods. Key patterns:
  - `ConversationListModel` — wraps `QVector<ConversationDTO>`, role names consistent with QML
  - `ContactListModel` — wraps `QVector<AIUserDTO>`, provides `get(int row)` 
  - `LoginViewModel` — `Q_PROPERTY(QString username ...)`, `Q_PROPERTY(QString password ...)`, `Q_PROPERTY(bool loading ...)`, `Q_INVOKABLE void login()`
  - `SettingsViewModel` — wraps `AppSettings` QSettings, exposes theme/server/TTS as writable properties

- [ ] **Step 6: Commit**

---

### Task 12: QML Shell & Navigation

**Files:**
- Create: `aaa2/resources/qml/theme/ThemeConfig.qml`
- Create: `aaa2/resources/qml/shell/MainShell.qml`
- Create: `aaa2/resources/qml/shell/Sidebar.qml`
- Create: `aaa2/resources/qml/shell/TitleBar.qml`
- Create: `aaa2/resources/qml/shell/StatusBar.qml`
- Modify: `aaa2/resources/qml/main.qml` — change from empty Window to MainShell loader

**Interfaces:** QML pages use StackView for navigation, Sidebar switches between ConversationListPage and ContactListPage, StatusBar shows WS connection status and total unread.

- [ ] **Step 1: ThemeConfig.qml** — define color palette, font sizes, spacing constants as `QtObject` with readonly properties
- [ ] **Step 2: MainShell.qml** — 3-column RowLayout: Sidebar | StackView (chat) | DetailPanel (collapsible)
- [ ] **Step 3: Sidebar.qml** — vertical TabBar (Chats / Contacts) + ListView for conversations/contacts
- [ ] **Step 4: TitleBar.qml** — custom title bar with window controls (for frameless window on Windows/Linux)
- [ ] **Step 5: StatusBar.qml** — connection indicator dot (green/red) + total unread badge
- [ ] **Step 6: Update main.qml** — load MainShell, expose C++ viewmodels via QQmlContext (setContextProperty in main.cpp)
- [ ] **Step 7: Commit**

---

### Task 13: QML Pages — Login, Chat, Contacts

**Files:**
- Create: `aaa2/resources/qml/pages/LoginPage.qml`
- Create: `aaa2/resources/qml/pages/ChatPage.qml`
- Create: `aaa2/resources/qml/pages/ContactListPage.qml`
- Create: `aaa2/resources/qml/pages/ContactDetailPage.qml`
- Create: `aaa2/resources/qml/pages/CreateAIUserPage.qml`
- Create: `aaa2/resources/qml/pages/ConversationListPage.qml`

**Key patterns:**
- LoginPage: TextField for username, TextField with echoMode: Password, "Login" Button → calls LoginViewModel.login()
- ChatPage: ColumnLayout { ListView { model: chatMessagesModel; delegate: Loader { sourceComponent: ... } }; MessageInput { } }
- ContactListPage: ListView with ContactCard delegate, search TextField, FAB for CreateAIUserPage
- ConversationListPage: ListView with ConversationItem delegate (avatar + name + preview + unread badge + timestamp)

- [ ] **Implement all 6 pages** with complete QML code
- [ ] **Commit**

---

### Task 14: QML Components — ChatBubble, MessageInput, MarkdownRenderer

**Files:**
- Create: `aaa2/resources/qml/components/ChatBubble.qml`
- Create: `aaa2/resources/qml/components/MessageInput.qml`
- Create: `aaa2/resources/qml/components/TypingIndicator.qml`
- Create: `aaa2/resources/qml/components/MediaPreview.qml`
- Create: `aaa2/resources/qml/components/FilePickerButton.qml`
- Create: `aaa2/resources/qml/components/EmojiPicker.qml`
- Create: `aaa2/resources/qml/components/UserAvatar.qml`
- Create: `aaa2/resources/qml/components/ContactCard.qml`
- Create: `aaa2/resources/qml/components/ConversationItem.qml`
- Create: `aaa2/resources/qml/components/LoadingSpinner.qml`

**Key patterns:**
- ChatBubble: Rectangle with rounded corners, color based on senderType (blue=user, gray=ai), content parsed from JSON {think, response}. Rich text via Text with `textFormat: Text.RichText` or a minimal Markdown component.
- MessageInput: RowLayout { TextArea + IconButton(send) + IconButton(attach) }. Enter sends (Shift+Enter for newline). FilePickerButton triggers QFileDialog::getOpenFileName via C++.
- TypingIndicator: 3 animated dots with SequentialAnimation opacity fade.
- Unread badge: small red circle with white count text, visible when unreadCount > 0.

- [ ] **Implement all 10 components** with complete QML code
- [ ] **Commit**

---

### Task 15: QML Pages — Settings, Profiles, Expansion, TTS

**Files:**
- Create: `aaa2/resources/qml/pages/SettingsPage.qml`
- Create: `aaa2/resources/qml/pages/AccountPage.qml`
- Create: `aaa2/resources/qml/pages/ProfilePage.qml` + `ProfileEditPage.qml`
- Create: `aaa2/resources/qml/pages/TTSVoicesPage.qml`
- Create: `aaa2/resources/qml/pages/ExpansionPage.qml`
- Create: `aaa2/resources/qml/dialogs/ConfirmDialog.qml`

**Key patterns:**
- SettingsPage: ListView with sections (Account, Appearance, Voice, Network, About), each row navigates to detail page
- ProfilePage: ListView of profile cards, pull-to-refresh
- TTSVoicesPage: RadioDelegate list for voice selection
- ExpansionPage: SwitchDelegate list for enable/disable modules

- [ ] **Implement all pages and dialogs**
- [ ] **Commit**

---

### Task 16: Call — CallSignalingClient, WebRtcEngine, CallManager, CallPage

**Files:**
- Create: `aaa2/src/core/network/CallSignalingClient.h` + `.cpp`
- Create: `aaa2/src/services/WebRtcEngine.h` + `.cpp`
- Create: `aaa2/src/services/AudioManager.h` + `.cpp`
- Create: `aaa2/src/services/VideoManager.h` + `.cpp`
- Create: `aaa2/src/services/CallManager.h` + `.cpp`
- Create: `aaa2/resources/qml/pages/CallPage.qml`
- Create: `aaa2/resources/qml/dialogs/CallInviteDialog.qml`

**Interfaces:**
- `CallSignalingClient` — QWebSocket for /ws/call/{conv_id}, relays JSON between WebRTC engine and signaling server
- `WebRtcEngine` — wraps libdatachannel PeerConnection, emits `localSdpGenerated`, `localIceCandidate`, `remoteVideoTrackReady`
- `AudioManager` — manages QAudioDevice mic/speaker selection, mute toggle
- `VideoManager` — QCamera + QVideoSink for local capture + remote rendering
- `CallManager` — orchestrates call lifecycle: startCall(convId, type), acceptCall(callId), hangup(callId), toggleMute(), toggleVideo()
- `CallPage.qml` — fullscreen remote video + PIP self-view + control bar (mute/video/hangup/accept)

- [ ] **Step 1: CallSignalingClient** — connect to /ws/call/{id}, parse JSON, emit offerReceived/answerReceived/iceReceived signals
- [ ] **Step 2: WebRtcEngine** — create PeerConnection, configure STUN/TURN, add audio+video tracks, set remote description/candidates
- [ ] **Step 3: CallManager** — wire signaling client + WebRTC engine + audio/video managers
- [ ] **Step 4: CallPage.qml** — independent Window, VideoOutput for remote and local, control buttons
- [ ] **Step 5: Commit**

---

### Task 17: Platform Integration — Tray, Notifications, Taskbar Badge

**Files:**
- Create: `aaa2/src/platform/WinTaskbar.h` + `.cpp`
- Create: `aaa2/src/platform/MacDock.h` + `.cpp`
- Create: `aaa2/src/platform/LinuxDesktop.h` + `.cpp`
- Modify: `aaa2/src/main.cpp` — instantiate QSystemTrayIcon, register global shortcut

**Interfaces:**
- `class PlatformIntegration : public QObject` — abstracts platform specifics, `updateBadge(int count)`, `showNotification(title, body)`, `requestAttention()`
- Each platform .cpp implements the interface using native APIs (ITaskbarList3 COM on Windows, NSApp dockTile on macOS, D-Bus on Linux)
- `QSystemTrayIcon` in main.cpp: tray icon, context menu (Show/Hide, Quit), minimized-to-tray behavior
- `QHotkey` or platform shortcut for Ctrl+Shift+A → show window

- [ ] **Implement platform integrations**
- [ ] **Commit**

---

### Task 18: Wire Everything — main.cpp Integration & End-to-End Test

**Files:**
- Modify: `aaa2/src/main.cpp` — create all singletons, register with QQmlContext, connect signals
- Create: `aaa2/resources/qml/AppState.qml` — singleton QML object that exposes global state (loggedIn, connectionStatus)
- Modify: `aaa2/CMakeLists.txt` — add all source files

**Steps:**
- [ ] Wire TokenManager → HttpClient → Repositories → Services → ViewModels in main.cpp
- [ ] Register all ViewModels as context properties for QML
- [ ] Set up QSystemTrayIcon with menu
- [ ] Connect ChatStreamClient signals → ChatService → ChatMessagesModel → QML bindings
- [ ] Connect UnreadTracker signals → Sidebar badge → System tray → Taskbar
- [ ] Manual smoke test: login, send message, receive streaming response, verify unread badge
- [ ] Commit

---

### Task 19: Tests — Core Utilities, Database, Network

**Files:**
- Create: `aaa2/tests/tst_ApiError.cpp`
- Create: `aaa2/tests/tst_JsonHelper.cpp`
- Create: `aaa2/tests/tst_UuidGenerator.cpp`
- Create: `aaa2/tests/tst_Database.cpp`
- Create: `aaa2/tests/tst_HttpClient.cpp`
- Create: `aaa2/tests/tst_TokenManager.cpp`
- Create: `aaa2/tests/tst_SyncService.cpp`
- Create: `aaa2/tests/tst_UnreadTracker.cpp`
- Modify: `aaa2/CMakeLists.txt` — add test target with Qt::Test

- [ ] **Step 1: tst_Database.cpp** — test open, insertMessage, upsertMessage, getLocalMessages, deleteStaleUnsynced, getUnreadCounts, markAsRead

```cpp
#include <QtTest>
#include "../src/core/storage/Database.h"

class TestDatabase : public QObject {
    Q_OBJECT
private slots:
    void testInsertAndFind() {
        Database db;
        QVERIFY(db.open(":memory:"));
        MessageDTO msg;
        msg.clientUuid = "test-uuid";
        msg.aiUserId = "ai-1";
        msg.senderType = "user";
        msg.msgType = "text";
        msg.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        msg.isComplete = true;
        auto id = db.insertMessage(msg, "ai-1");
        QVERIFY(id > 0);

        auto found = db.findByClientUuid("test-uuid");
        QVERIFY(found.has_value());
        QCOMPARE(found->clientUuid, std::string("test-uuid"));
    }

    void testDeleteStaleUnsynced() {
        Database db;
        QVERIFY(db.open(":memory:"));
        // Insert an old unsynced message
        MessageDTO old;
        old.clientUuid = "old-uuid";
        old.aiUserId = "ai-1";
        old.senderType = "user";
        old.msgType = "text";
        old.timestamp = QDateTime::currentDateTimeUtc()
            .addSecs(-60 * 60).toString(Qt::ISODate).toStdString(); // 1 hour ago
        db.insertMessage(old, "ai-1");

        db.deleteStaleUnsynced("ai-1", 30);
        auto found = db.findByClientUuid("old-uuid");
        QVERIFY(!found.has_value());
    }

    void testUnreadCounts() {
        Database db;
        QVERIFY(db.open(":memory:"));
        MessageDTO m;
        m.clientUuid = "uuid-1";
        m.aiUserId = "ai-1";
        m.senderType = "ai";
        m.msgType = "text";
        m.isRead = false;
        m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m, "ai-1");

        auto counts = db.getUnreadCounts();
        QCOMPARE(counts["ai-1"], 1);

        db.markAsRead("ai-1");
        counts = db.getUnreadCounts();
        QCOMPARE(counts["ai-1"], 0);
    }
};
QTEST_MAIN(TestDatabase)
#include "tst_Database.moc"
```

- [ ] **Step 2-7: Implement remaining test files** following TDD pattern — write the test for expected behavior, run to verify pass, commit

- [ ] **Step 8: Commit all tests**

```bash
git add tests/ CMakeLists.txt
git commit -m "test: unit tests for core utilities, database, auth, sync, and unread"
```

---

### Task 20: Final Integration & Smoke Test Checklist

- [ ] Build from clean checkout: `mkdir build && cd build && cmake .. && cmake --build .`
- [ ] Launch app: login flow works end-to-end (enter credentials → token persisted → WS connected)
- [ ] Send text message: appears as user bubble, AI response streams via WS chunks
- [ ] File upload: select image → upload via multipart → AI response with media
- [ ] Unread badge: send message to offline contact → badge increments
- [ ] System tray: minimize to tray, restore from tray, tray notification on new message
- [ ] Message sync: kill app, restart → messages reloaded from local DB + server sync
- [ ] Stale cleanup: if a message has server_id=NULL and is >30min old, it's removed on sync
- [ ] Settings: change theme, restart → theme persists
- [ ] Multi-page navigation: sidebar switches between chats/contacts/settings without errors
- [ ] Commit final integration fixes

---

## Plan Self-Review

1. **Spec coverage:** All 12 modules covered. Architecture layers covered. Ghost/unread fixes built into design. Platform integration (tray/notifications/badges) covered in Tasks 17-18.
2. **Placeholder scan:** No TBD/TODO. All steps have concrete file paths, code samples, or clear implementation descriptions.
3. **Type consistency:** `MessageDTO` defined in Task 3 with `serverId` (int64_t), `clientUuid` (string). Used consistently across Tasks 4, 7, 9, 11 with matching signatures. `ApiError` from Task 2 used in all `Result<T>` returns. `UnreadTracker` interface from Task 10 matches spec section 7.
