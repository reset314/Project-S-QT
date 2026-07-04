#pragma once
#include <QObject>
#include <QVector>
#include "../core/utils/ApiError.h"
#include "../core/models/ExpansionModuleDTO.h"

class HttpClient;

class ExpansionRepository : public QObject {
    Q_OBJECT
public:
    explicit ExpansionRepository(HttpClient *http, QObject *parent = nullptr);

    Result<QVector<ExpansionModuleDTO>> listModules();
    Result<ExpansionModuleDTO> toggleModule(const QString &name, bool enabled);

    Q_INVOKABLE QJsonObject listModulesJson();
    Q_INVOKABLE QJsonObject toggleModuleJson(const QString &name, bool enabled);

private:
    HttpClient *http_;
};
