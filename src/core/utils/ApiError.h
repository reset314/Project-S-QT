#pragma once
#include <string>
#include "../vendor/tl/expected.hpp"
#include <QJsonObject>
#include <QString>

struct ApiError {
    std::string code;
    std::string message;

    static ApiError fromJson(const QJsonObject &obj) {
        return ApiError{
            .code = obj.value("code").toString().toStdString(),
            .message = obj.value("message").toString().toStdString(),
        };
    }

    static ApiError networkError(const std::string &msg) {
        return ApiError{.code = "NETWORK_ERROR", .message = msg};
    }
};

template<typename T>
using Result = tl::expected<T, ApiError>;
