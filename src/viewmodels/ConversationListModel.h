#pragma once
#include <QAbstractListModel>
#include <QVector>
#include "../core/models/ConversationDTO.h"

class ConversationListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        UserIdRole,
        AiUserIdRole,
        TitleRole,
        CreatedAtRole,
        UpdatedAtRole,
        EndedAtRole,
        AiUserNameRole,
        AiUserAvatarRole,
        LastMessagePreviewRole,
    };
    Q_ENUM(Roles)

    explicit ConversationListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void replaceAll(const QVector<ConversationDTO> &conversations);
    void upsertConversation(const ConversationDTO &conv);
    void removeConversation(const QString &id);
    void clear();

    const ConversationDTO &at(int row) const;
    int findById(const QString &id) const;

private:
    QVector<ConversationDTO> conversations_;
    QHash<QString, int> idIndex_;
};
