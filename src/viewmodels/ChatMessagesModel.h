#pragma once
#include <QAbstractListModel>
#include <QVector>
#include <QHash>
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

    // Incremental updates for streaming performance
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
    QHash<int64_t, int> serverIdIndex_;

    void rebuildIndices();
    QVariant contentToVariant(const QJsonObject &content) const;
    QVariant mediaListToVariant(const std::vector<MediaDTO> &mediaList) const;
};
