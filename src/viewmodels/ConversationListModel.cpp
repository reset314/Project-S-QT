#include "ConversationListModel.h"

ConversationListModel::ConversationListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ConversationListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return conversations_.size();
}

QVariant ConversationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= conversations_.size())
        return {};

    const auto &c = conversations_.at(index.row());

    switch (role) {
    case IdRole:
        return QString::fromStdString(c.id);
    case UserIdRole:
        return QString::fromStdString(c.userId);
    case AiUserIdRole:
        return c.aiUserId ? QVariant(QString::fromStdString(*c.aiUserId)) : QVariant{};
    case TitleRole:
        return QString::fromStdString(c.title);
    case CreatedAtRole:
        return QString::fromStdString(c.createdAt);
    case UpdatedAtRole:
        return QString::fromStdString(c.updatedAt);
    case EndedAtRole:
        return c.endedAt ? QVariant(QString::fromStdString(*c.endedAt)) : QVariant{};
    case AiUserNameRole:
        return c.aiUserName ? QVariant(QString::fromStdString(*c.aiUserName)) : QVariant{};
    case AiUserAvatarRole:
        return c.aiUserAvatar ? QVariant(QString::fromStdString(*c.aiUserAvatar)) : QVariant{};
    case LastMessagePreviewRole:
        return c.lastMessagePreview ? QVariant(QString::fromStdString(*c.lastMessagePreview)) : QVariant{};
    default:
        return {};
    }
}

QHash<int, QByteArray> ConversationListModel::roleNames() const
{
    return {
        {IdRole,                 "id"},
        {UserIdRole,             "userId"},
        {AiUserIdRole,           "aiUserId"},
        {TitleRole,              "title"},
        {CreatedAtRole,          "createdAt"},
        {UpdatedAtRole,          "updatedAt"},
        {EndedAtRole,            "endedAt"},
        {AiUserNameRole,         "aiUserName"},
        {AiUserAvatarRole,       "aiUserAvatar"},
        {LastMessagePreviewRole, "lastMessagePreview"},
    };
}

void ConversationListModel::replaceAll(const QVector<ConversationDTO> &conversations)
{
    beginResetModel();
    conversations_ = conversations;
    idIndex_.clear();
    for (int i = 0; i < conversations_.size(); ++i) {
        idIndex_[QString::fromStdString(conversations_.at(i).id)] = i;
    }
    endResetModel();
}

void ConversationListModel::upsertConversation(const ConversationDTO &conv)
{
    const auto qid = QString::fromStdString(conv.id);
    auto it = idIndex_.find(qid);
    if (it != idIndex_.end()) {
        const int row = it.value();
        conversations_[row] = conv;
        emit dataChanged(index(row), index(row));
    } else {
        const int row = conversations_.size();
        beginInsertRows(QModelIndex(), row, row);
        conversations_.append(conv);
        idIndex_[qid] = row;
        endInsertRows();
    }
}

void ConversationListModel::removeConversation(const QString &id)
{
    auto it = idIndex_.find(id);
    if (it == idIndex_.end()) return;

    const int row = it.value();
    beginRemoveRows(QModelIndex(), row, row);
    conversations_.removeAt(row);
    // Rebuild index for all entries shifted down
    idIndex_.clear();
    for (int i = 0; i < conversations_.size(); ++i)
        idIndex_[QString::fromStdString(conversations_.at(i).id)] = i;
    endRemoveRows();
}

void ConversationListModel::clear()
{
    beginResetModel();
    conversations_.clear();
    idIndex_.clear();
    endResetModel();
}

const ConversationDTO &ConversationListModel::at(int row) const
{
    return conversations_.at(row);
}

int ConversationListModel::findById(const QString &id) const
{
    return idIndex_.value(id, -1);
}
