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

QJsonObject MemoryRepository::getMemoriesJson(const QString &aiUserId,
                                                const QStringList &types,
                                                int limit)
{
    auto r = queryMemories(aiUserId, {}, limit);
    if (!r) {
        QJsonObject e; e["error"] = QString::fromStdString(r.error().message); return e;
    }
    QJsonObject o; QJsonArray arr;
    for (const auto &m : *r) {
        // Filter by memory_type if types specified
        if (!types.isEmpty()) {
            QString mType = QString::fromStdString(m.memoryType);
            if (!types.contains(mType)) continue;
        }
        arr.append(m.toJson());
    }
    o["memories"] = arr; return o;
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
