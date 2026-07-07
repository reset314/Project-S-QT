#pragma once
#include <QObject>
#include <QTimer>
#include <QHash>
#include <QStringList>
#include "../repositories/ChatRepository.h"
#include "../core/network/ChatStreamClient.h"

class SyncService;

/// Per-aiUserId batching state.
/// Messages are collected during a cooldown window (1.5 s) and flushed
/// together.  A hard max-wait timer (4.5 s) guarantees an upper bound.
struct BatchState {
    QTimer *cooldownTimer = nullptr;
    QTimer *maxWaitTimer = nullptr;
    QStringList clientUuids;
    QStringList contents;
    QString conversationId;
};

/// Orchestrates the full send-message lifecycle:
///   1. Optimistic insert into local DB via ChatRepository
///   2. Batch collection (cooldown / max-wait)
///   3. POST /chat flush via ChatRepository
///   4. Server-id backfill on the optimistic row
///   5. Watchdog timer (30 s) — if stream_message_done does not arrive,
///      triggers SyncService recovery
///   6. ChatStreamClient signal wiring (streamInit → streamChunk → streamDone)
///
/// Conversation replacement: when the server returns a new conversationId
/// (e.g. the previous one was ended), the service emits
/// conversationReplaced() so upstream can swap.
class ChatService : public QObject {
    Q_OBJECT
public:
    explicit ChatService(ChatRepository *repo,
                         ChatStreamClient *wsClient,
                         SyncService *syncService = nullptr,
                         QObject *parent = nullptr);

    /// Send a text message.  If conversationId is empty the server will
    /// create (or reuse) one automatically.
    Q_INVOKABLE void sendMessage(const QString &aiUserId,
                                  const QString &content,
                                  const QString &conversationId = {});

    /// Send a media (image/video/audio/file) message via multipart/form-data.
    void sendMediaMessage(const QString &aiUserId,
                          const QString &filePath,
                          const QString &content = {},
                          const QString &msgType = {},
                          const QString &conversationId = {});

    /// Cancel all in-flight streams and watchdog timers for an AI user.
    void cancelAllStreams(const QString &aiUserId);

    /// Cancel only the watchdog timer (stream still in progress).
    void cancelWatchdog(const QString &aiUserId);

signals:
    /// Emitted when the WS stream_init frame arrives for a conversation.
    void streamInitReceived(const QString &aiUserId,
                            const QString &conversationId,
                            const QString &messageId);

    /// Emitted for each stream_chunk frame (incremental text).
    void streamChunkReceived(const QString &aiUserId,
                             const QString &content);

    /// Emitted when the WS stream_done frame arrives (final content).
    void streamDoneReceived(const QString &aiUserId,
                            const QString &messageId,
                            const QString &content);

    /// Emitted when the WS stream_error frame arrives.
    void streamErrorReceived(const QString &aiUserId,
                             const QString &code,
                             const QString &message);

    /// Watchdog expired — the server did not deliver a complete stream in
    /// time.  Callers should trigger a sync to recover.
    void messageTimedOut(const QString &aiUserId);

    /// The POST /chat response indicated a conversation replacement.
    /// Upstream must update its active conversationId.
    void conversationReplaced(const QString &newConversationId);

    /// Emitted whenever message state changes (send, stream_init, stream_chunk,
    /// stream_done, stream_error).  Upstream should reload the model from DB.
    void messagesChanged(const QString &aiUserId);

    /// Incremental signals — avoid full model reset (List flicker).
    void messageAppended(const QString &aiUserId, const MessageDTO &msg);
    void messageContentUpdated(const QString &aiUserId,
                               const QString &clientUuid,
                               const QJsonObject &content);
    void messageMarkedComplete(const QString &aiUserId,
                               const QString &clientUuid,
                               const QString &serverId);

private slots:
    void onStreamInit(const QString &aiUserId,
                      const QString &messageId,
                      const QString &timestamp);
    void onStreamChunk(const QString &aiUserId,
                       const QString &chunk);
    void onStreamDone(const QString &aiUserId,
                      const QString &messageId,
                      const QString &content);
    void onStreamError(const QString &aiUserId,
                       const QString &code,
                       const QString &message);

private:
    /// Flush accumulated batch contents to the server.
    void flushBatch(const QString &aiUserId);

    /// Start (or restart) the 30 s watchdog for a given AI user.
    void resetWatchdog(const QString &aiUserId);

    /// Stop and delete the watchdog timer for a given AI user.
    void stopWatchdog(const QString &aiUserId);

    /// Resolve conversationId → aiUserId via the local conversations cache.
    QString resolveAiUserId(const QString &conversationId) const;

    /// Perform an optimistic insert and return the generated clientUuid.
    QString optimisticInsert(const QString &aiUserId,
                             const QString &content,
                             const QString &conversationId,
                             const QString &senderType,
                             const QString &msgType);

    ChatRepository *repo_;
    ChatStreamClient *wsClient_;
    SyncService *syncService_;

    /// One watchdog timer per AI user (keyed by aiUserId).
    QHash<QString, QTimer *> watchdogTimers_;

    /// Batching state keyed by aiUserId.
    QHash<QString, BatchState> batchers_;

    /// Track which aiUserId is associated with each conversationId
    /// (populated at sendMessage time so WS signals can be mapped back).
    QHash<QString, QString> convToAiUser_;

    /// Track in-progress AI streaming messages by conversationId → clientUuid
    /// so we can update content incrementally across stream_chunk frames.
    QHash<QString, QString> streamingAiMessages_;

    int sendCooldownMs_ = 1500;
    int sendMaxWaitMs_  = 4500;
    int streamTimeoutMs_ = 30000;
};
