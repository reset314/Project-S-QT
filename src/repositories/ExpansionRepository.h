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

    /// GET /expansion/modules
    Result<QVector<ExpansionModuleDTO>> listModules();

    /// POST /expansion/modules/{name}/toggle
    Result<ExpansionModuleDTO> toggleModule(const QString &name, bool enabled);

private:
    HttpClient *http_;
};
