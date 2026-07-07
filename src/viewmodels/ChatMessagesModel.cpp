#include "ChatMessagesModel.h"
#include <QJsonArray>
#include <algorithm>

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
        return msg.serverId.empty() ? QVariant{} : QString::fromStdString(msg.serverId);
    case ClientUuidRole:
        return QString::fromStdString(msg.clientUuid);
    case AiUserIdRole:
        return QString::fromStdString(msg.aiUserId);
    case ConversationIdRole:
        return QString();  // 客户端不再使用 conversation_id
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
    auto parts = expandSplits({msg});
    if (parts.isEmpty()) return;

    const int firstRow = messages_.size();
    beginInsertRows(QModelIndex(), firstRow, firstRow + parts.size() - 1);
    for (auto &part : parts)
        messages_.append(part);
    rebuildIndices();
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
    messages_ = expandSplits(msgs);
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

void ChatMessagesModel::markComplete(int row, const QString &serverId)
{
    if (row < 0 || row >= messages_.size()) return;

    auto &msg = messages_[row];
    msg.serverId = serverId.toStdString();
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

void ChatMessagesModel::markRevoked(const QString &serverId)
{
    auto it = serverIdIndex_.find(serverId);
    if (it == serverIdIndex_.end()) return;
    int row = it.value();
    messages_[row].revokedAt = QDateTime::currentDateTimeUtc()
                                    .toString(Qt::ISODate).toStdString();
    emit dataChanged(index(row), index(row), {RevokedAtRole});
}

void ChatMessagesModel::markDeleted(const QString &serverId)
{
    auto it = serverIdIndex_.find(serverId);
    if (it == serverIdIndex_.end()) return;
    int row = it.value();
    messages_[row].deletedAt = QDateTime::currentDateTimeUtc()
                                    .toString(Qt::ISODate).toStdString();
    emit dataChanged(index(row), index(row), {});
}

void ChatMessagesModel::updateContentByServerId(const QString &serverId, const QString &content)
{
    auto it = serverIdIndex_.find(serverId);
    if (it == serverIdIndex_.end()) return;
    int row = it.value();
    messages_[row].content = content.toStdString();
    emit dataChanged(index(row), index(row), {ContentRole});
}

void ChatMessagesModel::rollbackToAnchor(const QString &anchorServerId)
{
    auto it = serverIdIndex_.find(anchorServerId);
    if (it == serverIdIndex_.end()) return;

    int anchorRow = it.value();
    // Remove all messages after the anchor
    int removeCount = messages_.size() - anchorRow - 1;
    if (removeCount <= 0) return;
    beginRemoveRows({}, anchorRow + 1, messages_.size() - 1);
    messages_.resize(anchorRow + 1);
    rebuildIndices();
    endRemoveRows();
}

const MessageDTO &ChatMessagesModel::messageAt(int row) const
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
        if (!msg.serverId.empty())
            serverIdIndex_[QString::fromStdString(msg.serverId)] = i;
    }
}

QVariant ChatMessagesModel::contentToVariant(const std::string &content) const
{
    // Content is a raw string. If it's valid JSON, return parsed object for QML.
    // Otherwise return the raw string.
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(content).toUtf8());
    if (doc.isObject() && !doc.object().isEmpty())
        return QVariant(doc.object());
    return QVariant(QString::fromStdString(content));
}

QVariant ChatMessagesModel::mediaListToVariant(const std::vector<MediaDTO> &mediaList) const
{
    if (mediaList.empty()) return QVariant{};
    QJsonArray arr;
    for (const auto &m : mediaList)
        arr.append(m.toJson());
    return QVariant(arr);
}

void ChatMessagesModel::setActiveAiUserId(const QString &aiUserId)
{
    if (activeAiUserId_ == aiUserId)
        return;
    activeAiUserId_ = aiUserId;
    emit activeAiUserIdChanged();
    emit loadRequested(aiUserId);
}

void ChatMessagesModel::onHistoryLoaded(const QVector<MessageDTO> &messages)
{
    // Sort by server timestamp ascending (oldest first in model → top of ListView)
    auto sorted = messages;
    std::sort(sorted.begin(), sorted.end(), [](const MessageDTO &a, const MessageDTO &b) {
        return a.timestamp < b.timestamp;
    });
    replaceAll(sorted);
    // Notify QML to scroll to bottom after layout
    emit historyLoaded();
}

void ChatMessagesModel::onOlderMessagesLoaded(const QVector<MessageDTO> &messages)
{
    if (messages.isEmpty()) {
        emit olderMessagesLoaded(0);
        return;
    }
    auto sorted = messages;
    std::sort(sorted.begin(), sorted.end(), [](const MessageDTO &a, const MessageDTO &b) {
        return a.timestamp < b.timestamp;
    });
    // Prepend at the top of the existing list
    prependMessages(sorted);
    emit olderMessagesLoaded(sorted.size());
}

void ChatMessagesModel::loadOlder()
{
    QString ts = oldestTimestamp();
    emit loadOlderRequested(activeAiUserId_, ts);
}

QString ChatMessagesModel::oldestTimestamp() const
{
    if (messages_.isEmpty()) return {};
    // Use serverId as the cursor (API expects message UUID for before_id)
    // Fall back to timestamp if no serverId
    const auto &oldest = messages_.first();
    if (!oldest.serverId.empty())
        return QString::fromStdString(oldest.serverId);
    return QString::fromStdString(oldest.timestamp);
}

// ── <split> expansion ────────────────────────────────────────────────
// Matches Flutter's _expandMessages() in chat_page.dart:
//   splitParts = cleaned.split('<split>').where((p) => p.trim().isNotEmpty)
// Each part becomes its own visual bubble with the same message metadata.

QVector<MessageDTO> ChatMessagesModel::expandSplits(const QVector<MessageDTO> &msgs) const
{
    QVector<MessageDTO> result;
    result.reserve(msgs.size()); // will grow if splits found

    for (const auto &msg : msgs) {
        // Extract response text from content (may be JSON string or plain text)
        QString responseText;
        QString rawContent = QString::fromStdString(msg.content);
        if (rawContent.trimmed().startsWith('{')) {
            QJsonDocument doc = QJsonDocument::fromJson(rawContent.toUtf8());
            if (doc.isObject()) {
                auto it = doc.object().find("response");
                if (it != doc.object().end() && it->isString())
                    responseText = it->toString();
            }
        }
        if (responseText.isEmpty())
            responseText = rawContent;

        // Only expand complete messages that contain <split>
        if (!msg.isComplete || responseText.isEmpty() || !responseText.contains("<split>")) {
            result.append(msg);
            continue;
        }

        const QStringList parts = responseText.split("<split>");
        QVector<QString> nonEmpty;
        for (const auto &p : parts) {
            QString t = p.trimmed();
            if (!t.isEmpty())
                nonEmpty.append(t);
        }

        if (nonEmpty.isEmpty()) {
            result.append(msg);
            continue;
        }

        for (int i = 0; i < nonEmpty.size(); ++i) {
            MessageDTO part = msg;
            part.clientUuid = msg.clientUuid + "_s" + std::to_string(i);
            // serverId stays with original — only first part carries it
            if (i > 0)
                part.serverId.clear();
            part.content = QJsonObject{};
            part.content["response"] = nonEmpty[i];
            // All parts except the last are "complete"; last inherits original
            part.isComplete = (i < nonEmpty.size() - 1) || msg.isComplete;
            result.append(part);
        }
    }
    return result;
}
