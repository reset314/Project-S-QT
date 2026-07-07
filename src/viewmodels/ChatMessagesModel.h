#pragma once
#include <QAbstractListModel>
#include <QVector>
#include <QHash>
#include "../core/models/MessageDTO.h"

class ChatMessagesModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString activeAiUserId READ activeAiUserId NOTIFY activeAiUserIdChanged)
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

    /// Set which AI user's messages are currently displayed.
    /// Emits loadRequested() — upstream should load history + sync.
    Q_INVOKABLE void setActiveAiUserId(const QString &aiUserId);
    QString activeAiUserId() const { return activeAiUserId_; }

    /// Called by C++ after history/sync completes for the active AI user.
    void onHistoryLoaded(const QVector<MessageDTO> &messages);

    /// Called by C++ after older messages are loaded (pagination).
    void onOlderMessagesLoaded(const QVector<MessageDTO> &messages);

    /// QML: request to load older messages (pagination — scroll to top).
    Q_INVOKABLE void loadOlder();

    /// Return the timestamp of the oldest message in the model, or empty.
    Q_INVOKABLE QString oldestTimestamp() const;

signals:
    void activeAiUserIdChanged();
    void loadRequested(const QString &aiUserId);
    void loadOlderRequested(const QString &aiUserId, const QString &beforeTimestamp);
    void olderMessagesLoaded(int prependCount);
    void historyLoaded();
    void messageAppended();

public:
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Incremental updates for streaming performance
    void appendMessage(const MessageDTO &msg);
    void prependMessages(const QVector<MessageDTO> &msgs);
    void replaceAll(const QVector<MessageDTO> &msgs);
    void updateContent(int row, const QJsonObject &content);
    void removeMessage(const QString &clientUuid);
    void markComplete(int row, const QString &serverId);
    void clear();

    // Event-driven updates
    Q_INVOKABLE void markRevoked(const QString &serverId);
    Q_INVOKABLE void markDeleted(const QString &serverId);
    Q_INVOKABLE void updateContentByServerId(const QString &serverId, const QString &content);
    Q_INVOKABLE void rollbackToAnchor(const QString &anchorServerId);

    const MessageDTO &messageAt(int row) const;
    int findByClientUuid(const QString &uuid) const;

private:
    QVector<MessageDTO> messages_;
    QHash<QString, int> clientUuidIndex_;
    QHash<QString, int> serverIdIndex_;
    QString activeAiUserId_;

    void rebuildIndices();
    QVariant contentToVariant(const std::string &content) const;
    QVariant mediaListToVariant(const std::vector<MediaDTO> &mediaList) const;

    /// Expand messages containing <split> in response text into multiple rows.
    /// Matches Flutter's _expandMessages() in chat_page.dart.
    QVector<MessageDTO> expandSplits(const QVector<MessageDTO> &msgs) const;
};
