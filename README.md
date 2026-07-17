# Project-S (Qt Desktop)

C++ Qt 6 桌面端 AI 聊天客户端（Windows / macOS / Linux）。

后端服务位于 [`Project-S-backend`](https://github.com/reset314/Project-S-backend)。

## 技术栈

| 模块 | 方案 |
|------|------|
| 框架 | C++ Qt 6.11+ / QML / CMake |
| HTTP | QNetworkAccessManager |
| WebSocket | QWebSocket |
| 存储 | SQLite (QSqlDatabase) + QSettings |
| 媒体 | Qt Multimedia |
| WebRTC | LibDataChannel（通话）|
| 主题 | aqt-stylesheets（CSS）|

## 环境要求

- Qt 6.5+
- CMake 3.16+
- C++17 编译器（GCC / MSVC / Clang）
- nlohmann_json（vendored）

## 快速开始

```bash
# 构建
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# 运行
./build/ProjectS
```

## 项目结构

```
src/
├── main.cpp                 # 入口 + 依赖注入 + 信号绑定
├── core/
│   ├── auth/                # TokenManager + 系统密钥链
│   ├── models/              # MessageDTO, AIUserDTO, CallDTO 等
│   ├── network/             # HttpClient, ChatStreamClient, CallSignalingClient
│   ├── storage/             # SQLite Database（含迁移）
│   ├── theme/               # CSS 主题引擎
│   └── utils/               # JsonHelper, UuidGenerator
├── repositories/            # 11 个 Repository 类
├── services/                # ChatService, SyncService, CallManager, UnreadTracker
├── viewmodels/              # ChatMessagesModel, ConversationListModel 等
└── platform/                # WinTaskbar, MacDock, LinuxDesktop
```

## 关键特性

- **消息批处理**：1.5s 冷却 / 4.5s 最大等待
- **流式响应**：WebSocket `stream_chunk` → `stream_message_done`
- **多设备同步**：eventReceived 信号处理 `message.sent/revoked/edited/deleted` 广播
- **断线重连**：指数退避 + sync 拉取错失事件
- **WebRTC 通话**：语音 / 视频
- **系统托盘**：最小化到托盘 + 未读徽标
- **深色/浅色主题**：CSS 引擎驱动

## 通信架构

```
WebSocket (wss://.../ws/connect)
├── auth (device_id + JWT)
├── ping/pong (心跳)
├── stream_message_init → chunk → done (AI 流式回复)
├── event (message.sent / revoked / edited / deleted)
└── sync (断线后拉取错失事件)

HTTP REST (https://.../)
├── /chat (发送消息)
├── /aiusers/* (AI 角色 CRUD)
├── /sync (事件兜底同步)
└── /upload (文件上传)
```

### 致谢

本软件使用了以下开源组件：

| 组件 | 协议 | 版权 |
|------|------|------|
| **Ableton Link**（vendored） | MIT | Copyright (c) 2014 Ableton AG, Berlin |
| **Catch**（vendored） | Boost Software License 1.0 | Copyright (c) 2014, Catch Authors |
| **cpp-peglib**（vendored） | MIT | Copyright (c) 2015 yhirose |
