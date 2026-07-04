#include <QtTest>
#include <QJsonObject>
#include "../src/core/utils/ApiError.h"

class TestApiError : public QObject {
    Q_OBJECT
private slots:
    void testFromJson()
    {
        QJsonObject obj;
        obj["code"] = "NOT_FOUND";
        obj["message"] = "Resource not found";

        auto err = ApiError::fromJson(obj);
        QCOMPARE(err.code, std::string("NOT_FOUND"));
        QCOMPARE(err.message, std::string("Resource not found"));
    }

    void testFromJsonEmpty()
    {
        QJsonObject obj;
        auto err = ApiError::fromJson(obj);
        // Empty JSON fields produce empty strings
        QCOMPARE(err.code, std::string(""));
        QCOMPARE(err.message, std::string(""));
    }

    void testFromJsonMissingFields()
    {
        QJsonObject obj;
        obj["code"] = "AUTH_FAILED";
        // message is missing

        auto err = ApiError::fromJson(obj);
        QCOMPARE(err.code, std::string("AUTH_FAILED"));
        QCOMPARE(err.message, std::string(""));
    }

    void testNetworkError()
    {
        auto err = ApiError::networkError("Connection refused");
        QCOMPARE(err.code, std::string("NETWORK_ERROR"));
        QCOMPARE(err.message, std::string("Connection refused"));
    }

    void testNetworkErrorEmpty()
    {
        auto err = ApiError::networkError("");
        QCOMPARE(err.code, std::string("NETWORK_ERROR"));
        QCOMPARE(err.message, std::string(""));
    }

    void testResultSuccess()
    {
        Result<int> r = 42;
        QVERIFY(r.has_value());
        QCOMPARE(r.value(), 42);
    }

    void testResultError()
    {
        Result<int> r = tl::make_unexpected(
            ApiError{.code = "FAIL", .message = "It failed"});
        QVERIFY(!r.has_value());
        QCOMPARE(r.error().code, std::string("FAIL"));
        QCOMPARE(r.error().message, std::string("It failed"));
    }
};

QTEST_MAIN(TestApiError)
#include "tst_ApiError.moc"
