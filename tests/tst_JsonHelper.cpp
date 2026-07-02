#include <QtTest>
#include <QJsonObject>
#include <QJsonArray>
#include "../src/core/utils/JsonHelper.h"

class TestJsonHelper : public QObject {
    Q_OBJECT
private slots:
    void testGetString()
    {
        QJsonObject obj;
        obj["name"] = "Alice";

        auto result = JsonHelper::getString(obj, "name");
        QCOMPARE(result, std::string("Alice"));
    }

    void testGetStringDefault()
    {
        QJsonObject obj;
        auto result = JsonHelper::getString(obj, "missing_key", "fallback");
        QCOMPARE(result, std::string("fallback"));
    }

    void testGetStringDefaultEmpty()
    {
        QJsonObject obj;
        // Default default is empty string
        auto result = JsonHelper::getString(obj, "missing");
        QCOMPARE(result, std::string(""));
    }

    void testGetStringFromNonString()
    {
        QJsonObject obj;
        obj["count"] = 42; // integer, not string

        auto result = JsonHelper::getString(obj, "count", "default");
        QCOMPARE(result, std::string("default"));
    }

    void testGetInt()
    {
        QJsonObject obj;
        obj["id"] = 12345.0; // JSON numbers are double

        auto result = JsonHelper::getInt(obj, "id");
        QCOMPARE(result, static_cast<int64_t>(12345));
    }

    void testGetIntFromString()
    {
        QJsonObject obj;
        obj["id"] = "999";

        auto result = JsonHelper::getInt(obj, "id");
        QCOMPARE(result, static_cast<int64_t>(999));
    }

    void testGetIntFromInvalidString()
    {
        QJsonObject obj;
        obj["id"] = "not-a-number";

        auto result = JsonHelper::getInt(obj, "id", 100);
        QCOMPARE(result, static_cast<int64_t>(100));
    }

    void testGetIntDefault()
    {
        QJsonObject obj;
        auto result = JsonHelper::getInt(obj, "missing", -1);
        QCOMPARE(result, static_cast<int64_t>(-1));
    }

    void testGetIntDefaultZero()
    {
        QJsonObject obj;
        // Default default is 0
        auto result = JsonHelper::getInt(obj, "missing");
        QCOMPARE(result, static_cast<int64_t>(0));
    }

    void testGetIntFromBool()
    {
        QJsonObject obj;
        obj["flag"] = true;

        // bool is not double and not string, so default
        auto result = JsonHelper::getInt(obj, "flag", 42);
        QCOMPARE(result, static_cast<int64_t>(42));
    }

    void testGetBool()
    {
        QJsonObject obj;
        obj["active"] = true;
        obj["deleted"] = false;

        QCOMPARE(JsonHelper::getBool(obj, "active"), true);
        QCOMPARE(JsonHelper::getBool(obj, "deleted"), false);
    }

    void testGetBoolDefault()
    {
        QJsonObject obj;
        QCOMPARE(JsonHelper::getBool(obj, "missing"), false);
        QCOMPARE(JsonHelper::getBool(obj, "missing", true), true);
    }

    void testGetBoolFromNonBool()
    {
        QJsonObject obj;
        obj["active"] = 1; // integer, not bool

        QCOMPARE(JsonHelper::getBool(obj, "active", false), false);
    }

    void testGetOptionalDateTime()
    {
        QJsonObject obj;
        obj["timestamp"] = "2026-07-02T12:00:00";

        auto result = JsonHelper::getOptionalDateTime(obj, "timestamp");
        QVERIFY(result.has_value());
        QCOMPARE(result->date().year(), 2026);
        QCOMPARE(result->date().month(), 7);
        QCOMPARE(result->date().day(), 2);
        QCOMPARE(result->time().hour(), 12);
    }

    void testGetOptionalDateTimeMissing()
    {
        QJsonObject obj;
        auto result = JsonHelper::getOptionalDateTime(obj, "timestamp");
        QVERIFY(!result.has_value());
    }

    void testGetOptionalDateTimeNotString()
    {
        QJsonObject obj;
        obj["timestamp"] = 12345;

        auto result = JsonHelper::getOptionalDateTime(obj, "timestamp");
        QVERIFY(!result.has_value());
    }

    void testGetOptionalDateTimeInvalid()
    {
        QJsonObject obj;
        obj["timestamp"] = "not-a-date";

        auto result = JsonHelper::getOptionalDateTime(obj, "timestamp");
        QVERIFY(!result.has_value());
    }

    void testGetOptionalDateTimeNull()
    {
        QJsonObject obj;
        obj["timestamp"] = QJsonValue::Null;

        auto result = JsonHelper::getOptionalDateTime(obj, "timestamp");
        QVERIFY(!result.has_value());
    }

    void testGetArray()
    {
        QJsonObject obj;
        QJsonArray arr;
        arr.append(1);
        arr.append(2);
        arr.append(3);
        obj["items"] = arr;

        auto result = JsonHelper::getArray(obj, "items");
        QCOMPARE(result.size(), 3);
        QCOMPARE(result[0].toInt(), 1);
        QCOMPARE(result[2].toInt(), 3);
    }

    void testGetArrayMissing()
    {
        QJsonObject obj;
        auto result = JsonHelper::getArray(obj, "missing");
        QVERIFY(result.isEmpty());
    }

    void testGetArrayFromNonArray()
    {
        QJsonObject obj;
        obj["items"] = "not-an-array";

        auto result = JsonHelper::getArray(obj, "items");
        QVERIFY(result.isEmpty());
    }

    void testGetArrayEmpty()
    {
        QJsonObject obj;
        obj["items"] = QJsonArray{};

        auto result = JsonHelper::getArray(obj, "items");
        QVERIFY(result.isEmpty());
    }
};

QTEST_MAIN(TestJsonHelper)
#include "tst_JsonHelper.moc"
