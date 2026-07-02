#include "ContactListModel.h"

ContactListModel::ContactListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ContactListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return contacts_.size();
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= contacts_.size())
        return {};

    const auto &c = contacts_.at(index.row());

    switch (role) {
    case IdRole:
        return QString::fromStdString(c.id);
    case NameRole:
        return QString::fromStdString(c.name);
    case DescriptionRole:
        return c.description ? QVariant(QString::fromStdString(*c.description)) : QVariant{};
    case AvatarRole:
        return c.avatar ? QVariant(QString::fromStdString(*c.avatar)) : QVariant{};
    case CreatedByRole:
        return QString::fromStdString(c.createdBy);
    case LlmProviderRole:
        return c.llmProvider ? QVariant(QString::fromStdString(*c.llmProvider)) : QVariant{};
    case LlmModelRole:
        return c.llmModel ? QVariant(QString::fromStdString(*c.llmModel)) : QVariant{};
    case CreatedAtRole:
        return QString::fromStdString(c.createdAt);
    default:
        return {};
    }
}

QHash<int, QByteArray> ContactListModel::roleNames() const
{
    return {
        {IdRole,            "id"},
        {NameRole,          "name"},
        {DescriptionRole,   "description"},
        {AvatarRole,        "avatar"},
        {CreatedByRole,     "createdBy"},
        {LlmProviderRole,   "llmProvider"},
        {LlmModelRole,      "llmModel"},
        {CreatedAtRole,     "createdAt"},
    };
}

void ContactListModel::replaceAll(const QVector<AIUserDTO> &contacts)
{
    beginResetModel();
    contacts_ = contacts;
    idIndex_.clear();
    for (int i = 0; i < contacts_.size(); ++i)
        idIndex_[QString::fromStdString(contacts_.at(i).id)] = i;
    endResetModel();
}

void ContactListModel::clear()
{
    beginResetModel();
    contacts_.clear();
    idIndex_.clear();
    endResetModel();
}

const AIUserDTO &ContactListModel::at(int row) const
{
    return contacts_.at(row);
}

int ContactListModel::findById(const QString &id) const
{
    return idIndex_.value(id, -1);
}
