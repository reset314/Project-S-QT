#include "MemoryRepository.h"
#include "../core/network/HttpClient.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>

MemoryRepository::MemoryRepository(HttpClient *http, QObject *parent)
    : QObject(parent)
    , http_(http)
{
}

Result<QVector<MemoryDTO>> MemoryRepository::queryMemories(const QString &aiUserId,
                                                            const QString &query,
                                                            int limit)
{
    QUrlQuery params;
    params.addQueryItem("ai_user_id", aiUserId);
    params.addQueryItem("limit", QString::number(limit));
    if (!query.isEmpty())
        params.addQueryItem("query", query);

    auto result = http_->get("/memories", params);
    if (!result)
        return tl::make_unexpected(result.error());

    // Response: {success: true, data: [...]}
    // HttpClient passes through the root when data is an array.
    QJsonValue dataVal = result->value("data");
    QJsonArray arr;
    if (dataVal.isArray()) {
        arr = dataVal.toArray();
    } else if (result->contains("success")) {
        arr = result->value("data").toArray();
    }

    QVector<MemoryDTO> memories;
    for (const auto &item : arr) {
        if (item.isObject())
            memories.append(MemoryDTO::fromJson(item.toObject()));
    }
    return memories;
}
