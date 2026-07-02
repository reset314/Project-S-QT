#pragma once
#include <QAbstractListModel>
#include <QVector>
#include "../core/models/AIUserDTO.h"

class ContactListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        AvatarRole,
        CreatedByRole,
        LlmProviderRole,
        LlmModelRole,
        CreatedAtRole,
    };
    Q_ENUM(Roles)

    explicit ContactListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void replaceAll(const QVector<AIUserDTO> &contacts);
    void clear();

    const AIUserDTO &at(int row) const;
    int findById(const QString &id) const;

private:
    QVector<AIUserDTO> contacts_;
    QHash<QString, int> idIndex_;
};
