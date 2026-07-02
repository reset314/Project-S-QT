#pragma once
#include <QString>
#include "../utils/ApiError.h"

class Keychain {
public:
    static Result<void> write(const QString &key, const QString &value);
    static Result<QString> read(const QString &key);
    static Result<void> remove(const QString &key);

private:
    static QString serviceName() { return "AIChatDesktop"; }
};
