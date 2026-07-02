#include "ChatMessagesModel.h"
#include <QJsonArray>

ChatMessagesModel::ChatMessagesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ChatMessagesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return messages_.size();
}

QVariant ChatMessagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= messages_.size())
        return {};

    const auto &msg = messages_.at(index.row());

    switch (role) {
    case ServerIdRole:
        return msg.serverId ? QVariant::fromValue(static_cast<qint64>(*msg.serverId)) : QVariant{};
    case ClientUuidRole:
        return QString::fromStdString(msg.clientUuid);
    case AiUserIdRole:
        return QString::fromStdString(msg.aiUserId);
    case ConversationIdRole:
        return QString::fromStdString(msg.conversationId);
    case SenderTypeRole:
        return QString::fromStdString(msg.senderType);
    case MsgTypeRole:
        return QString::fromStdString(msg.msgType);
    case ContentRole:
        return contentToVariant(msg.content);
    case MediaListRole:
        return mediaListToVariant(msg.mediaList);
    case IsCompleteRole:
        return msg.isComplete;
    case RevokedAtRole:
        return msg.revokedAt ? QVariant(*msg.revokedAt) : QVariant{};
    case TimestampRole:
        return QString::fromStdString(msg.timestamp);
    default:
        return {};
    }
}

QHash<int, QByteArray> ChatMessagesModel::roleNames() const
{
    return {
        {ServerIdRole,       "serverId"},
        {ClientUuidRole,     "clientUuid"},
        {AiUserIdRole,       "aiUserId"},
        {ConversationIdRole, "conversationId"},
        {SenderTypeRole,     "senderType"},
        {MsgTypeRole,        "msgType"},
        {ContentRole,        "content"},
        {MediaListRole,      "mediaList"},
        {IsCompleteRole,     "isComplete"},
        {RevokedAtRole,      "revokedAt"},
        {TimestampRole,      "timestamp"},
    };
}

void ChatMessagesModel::appendMessage(const MessageDTO &msg)
{
    const int row = messages_.size();
    beginInsertRows(QModelIndex(), row, row);
    messages_.append(msg);

    const auto uuid = QString::fromStdString(msg.clientUuid);
    if (!uuid.isEmpty())
        clientUuidIndex_[uuid] = row;
    if (msg.serverId)
        serverIdIndex_[*msg.serverId] = row;

    endInsertRows();
    emit messageAppended();
}

void ChatMessagesModel::prependMessages(const QVector<MessageDTO> &msgs)
{
    if (msgs.isEmpty()) return;

    beginInsertRows(QModelIndex(), 0, msgs.size() - 1);

    // Insert at front and shift all existing indices
    for (auto it = msgs.rbegin(); it != msgs.rend(); ++it)
        messages_.prepend(*it);

    rebuildIndices();
    endInsertRows();
}

void ChatMessagesModel::replaceAll(const QVector<MessageDTO> &msgs)
{
    beginResetModel();
    messages_ = msgs;
    rebuildIndices();
    endResetModel();
}

void ChatMessagesModel::updateContent(int row, const QJsonObject &content)
{
    if (row < 0 || row >= messages_.size()) return;
    messages_[row].content = content;
    emit dataChanged(index(row), index(row), {ContentRole});
}

void ChatMessagesModel::removeMessage(const QString &clientUuid)
{
    auto it = clientUuidIndex_.find(clientUuid);
    if (it == clientUuidIndex_.end()) return;

    const int row = it.value();
    beginRemoveRows(QModelIndex(), row, row);
    messages_.removeAt(row);
    rebuildIndices();
    endRemoveRows();
}

void ChatMessagesModel::markComplete(int row, int64_t serverId)
{
    if (row < 0 || row >= messages_.size()) return;

    auto &msg = messages_[row];
    msg.serverId = serverId;
    msg.isComplete = true;
    serverIdIndex_[serverId] = row;

    emit dataChanged(index(row), index(row), {ServerIdRole, IsCompleteRole});
}

void ChatMessagesModel::clear()
{
    beginResetModel();
    messages_.clear();
    clientUuidIndex_.clear();
    serverIdIndex_.clear();
    endResetModel();
}

const MessageDTO &ChatMessagesModel::at(int row) const
{
    return messages_.at(row);
}

int ChatMessagesModel::findByClientUuid(const QString &uuid) const
{
    return clientUuidIndex_.value(uuid, -1);
}

void ChatMessagesModel::rebuildIndices()
{
    clientUuidIndex_.clear();
    serverIdIndex_.clear();
    for (int i = 0; i < messages_.size(); ++i) {
        const auto &msg = messages_.at(i);
        const auto uuid = QString::fromStdString(msg.clientUuid);
        if (!uuid.isEmpty())
            clientUuidIndex_[uuid] = i;
        if (msg.serverId)
            serverIdIndex_[*msg.serverId] = i;
    }
}

QVariant ChatMessagesModel::contentToVariant(const QJsonObject &content) const
{
    return content.isEmpty() ? QVariant{} : QVariant(content);
}

QVariant ChatMessagesModel::mediaListToVariant(const std::vector<MediaDTO> &mediaList) const
{
    if (mediaList.empty()) return QVariant{};
    QJsonArray arr;
    for (const auto &m : mediaList)
        arr.append(m.toJson());
    return QVariant(arr);
}
