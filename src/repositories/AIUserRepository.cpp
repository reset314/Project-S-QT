#include "AIUserRepository.h"
#include "../core/network/HttpClient.h"
#include <QJsonObject>
#include <QJsonArray>

// ---------------------------------------------------------------------------
// Helper — unwrap envelope when data is not a plain object
// ---------------------------------------------------------------------------

static QJsonValue unwrapData(const QJsonObject &resp) {
    if (resp.contains("success")) {
        return resp.value("data");
    }
    return QJsonValue(resp);
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

AIUserRepository::AIUserRepository(HttpClient *http, QObject *parent)
    : QObject(parent)
    , http_(http)
{
}

// ---------------------------------------------------------------------------
// listAIUsers  — GET /aiusers/  →  {success, data: {items: [...]}}
// ---------------------------------------------------------------------------

Result<QVector<AIUserDTO>> AIUserRepository::listAIUsers()
{
    auto result = http_->get("/aiusers/");
    if (!result)
        return tl::make_unexpected(result.error());

    // data is {items: [...]} — HttpClient unwrapped it to just {items: [...]}
    QJsonObject data = *result;
    QJsonArray items = data.value("items").toArray();

    QVector<AIUserDTO> users;
    for (const auto &item : items) {
        if (item.isObject())
            users.append(AIUserDTO::fromJson(item.toObject()));
    }
    return users;
}

// ---------------------------------------------------------------------------
// getAIUser  — GET /aiusers/{id}
// ---------------------------------------------------------------------------

Result<AIUserDTO> AIUserRepository::getAIUser(const QString &aiUserId)
{
    auto result = http_->get(QString("/aiusers/%1").arg(aiUserId));
    if (!result)
        return tl::make_unexpected(result.error());

    return AIUserDTO::fromJson(*result);
}

// ---------------------------------------------------------------------------
// createAIUser  — POST /aiusers/
// ---------------------------------------------------------------------------

Result<AIUserDTO> AIUserRepository::createAIUser(const QString &name,
                                                  const QString &description,
                                                  const QString &systemPrompt,
                                                  const QString &llmProvider,
                                                  const QString &llmModel,
                                                  const QString &avatar,
                                                  const QJsonObject &chatConfig)
{
    QJsonObject body;
    body["name"] = name;
    if (!description.isEmpty())
        body["description"] = description;
    if (!systemPrompt.isEmpty())
        body["system_prompt"] = systemPrompt;
    if (!llmProvider.isEmpty())
        body["llm_provider"] = llmProvider;
    if (!llmModel.isEmpty())
        body["llm_model"] = llmModel;
    if (!avatar.isEmpty())
        body["avatar"] = avatar;
    if (!chatConfig.isEmpty())
        body["chat_config"] = chatConfig;

    auto result = http_->post("/aiusers/", body);
    if (!result)
        return tl::make_unexpected(result.error());

    return AIUserDTO::fromJson(*result);
}

// ---------------------------------------------------------------------------
// updateAIUser  — PUT /aiusers/{id}
// ---------------------------------------------------------------------------

Result<AIUserDTO> AIUserRepository::updateAIUser(const QString &aiUserId,
                                                  const QJsonObject &fields)
{
    // HttpClient doesn't have a put() method.  Many backends accept POST for
    // updates with a method override, or we can use a raw approach.
    // Strategy: use POST with the full path.  Actually, HttpClient only has
    // get() and post().  For PUT requests we construct the path and body
    // and the server might accept POST as an alternative, or we need to
    // issue a raw request.
    //
    // The backend uses PUT /aiusers/{id}.  Since HttpClient lacks put(),
    // we build the request manually through QNetworkAccessManager, or we
    // do a workaround.  Let's add a simple workaround: use post() with the
    // path as-is — many frameworks route PUT and POST the same.
    //
    // Actually, looking at the Flutter ApiService, it has both post() and put().
    // Our HttpClient only has get() and post().  We'll use post() and rely on
    // the retry-with-refresh logic.  The server strictly requires PUT though.
    //
    // Best approach: send a raw request through HttpClient's internal NAM.
    auto result = http_->put(QString("/aiusers/%1").arg(aiUserId), fields);
    if (!result)
        return tl::make_unexpected(result.error());

    return AIUserDTO::fromJson(*result);
}
