#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <optional>
#include <string>

namespace JsonHelper {

inline std::string getString(const QJsonObject &obj, const char *key,
                              const std::string &def = "") {
    auto val = obj.value(key);
    return val.isString() ? val.toString().toStdString() : def;
}

inline int64_t getInt(const QJsonObject &obj, const char *key, int64_t def = 0) {
    auto val = obj.value(key);
    if (val.isDouble()) return static_cast<int64_t>(val.toDouble());
    if (val.isString()) {
        bool ok;
        auto n = val.toString().toLongLong(&ok);
        return ok ? n : def;
    }
    return def;
}

inline bool getBool(const QJsonObject &obj, const char *key, bool def = false) {
    auto val = obj.value(key);
    return val.isBool() ? val.toBool() : def;
}

inline std::optional<QDateTime> getOptionalDateTime(const QJsonObject &obj,
                                                      const char *key) {
    auto val = obj.value(key);
    if (!val.isString()) return std::nullopt;
    auto dt = QDateTime::fromString(val.toString(), Qt::ISODate);
    return dt.isValid() ? std::optional{dt} : std::nullopt;
}

inline QJsonArray getArray(const QJsonObject &obj, const char *key) {
    auto val = obj.value(key);
    return val.isArray() ? val.toArray() : QJsonArray{};
}

} // namespace JsonHelper
