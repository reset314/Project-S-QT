#pragma once
#include <QObject>
#include <QVector>
#include "../core/utils/ApiError.h"
#include "../core/models/MemoryDTO.h"

class HttpClient;

class MemoryRepository : public QObject {
    Q_OBJECT
public:
    explicit MemoryRepository(HttpClient *http, QObject *parent = nullptr);

    /// GET /memories?ai_user_id=&query=&limit=
    Result<QVector<MemoryDTO>> queryMemories(const QString &aiUserId,
                                              const QString &query = {},
                                              int limit = 50);

private:
    HttpClient *http_;
};
