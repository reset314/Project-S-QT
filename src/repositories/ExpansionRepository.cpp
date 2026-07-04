#include "ExpansionRepository.h"
#include "../core/network/HttpClient.h"
#include <QJsonObject>
#include <QJsonArray>

ExpansionRepository::ExpansionRepository(HttpClient *http, QObject *parent)
    : QObject(parent)
    , http_(http)
{
}

Result<QVector<ExpansionModuleDTO>> ExpansionRepository::listModules()
{
    auto result = http_->get("/expansion/modules");
    if (!result)
        return tl::make_unexpected(result.error());

    // Response: {success: true, data: {modules: [...]}}
    // HttpClient unwraps data object → we get {modules: [...]}
    QJsonObject data = *result;
    QJsonArray modules = data.value("modules").toArray();

    QVector<ExpansionModuleDTO> list;
    for (const auto &item : modules) {
        if (item.isObject())
            list.append(ExpansionModuleDTO::fromJson(item.toObject()));
    }
    return list;
}

Result<ExpansionModuleDTO> ExpansionRepository::toggleModule(const QString &name,
                                                               bool enabled)
{
    QJsonObject body;
    body["enabled"] = enabled;

    auto result = http_->post(QString("/expansion/modules/%1/toggle").arg(name), body);
    if (!result)
        return tl::make_unexpected(result.error());

    return ExpansionModuleDTO::fromJson(*result);
}

QJsonObject ExpansionRepository::listModulesJson() {
    auto r = listModules();
    if (!r) { QJsonObject e; e["error"] = QString::fromStdString(r.error().message); return e; }
    QJsonObject o; QJsonArray arr;
    for (const auto &m : *r) arr.append(m.toJson());
    o["modules"] = arr; return o;
}

QJsonObject ExpansionRepository::toggleModuleJson(const QString &name, bool enabled) {
    auto r = toggleModule(name, enabled);
    if (!r) { QJsonObject e; e["error"] = QString::fromStdString(r.error().message); return e; }
    return r->toJson();
}
