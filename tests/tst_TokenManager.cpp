#include <QtTest>
#include "../src/core/auth/TokenManager.h"
#include "../src/core/auth/Keychain.h"

class TestTokenManager : public QObject {
    Q_OBJECT
private slots:
    void initTestCase()
    {
        // Clean up any leftover test keys
        Keychain::remove("test_access");
        Keychain::remove("test_refresh");
    }

    void cleanupTestCase()
    {
        Keychain::remove("test_access");
        Keychain::remove("test_refresh");
    }

    // ------------------------------------------------------------------
    // JWT exp decoding
    // ------------------------------------------------------------------

    void testDecodeExpValid()
    {
        // Create a minimal JWT token with a known exp.
        // Header: {"alg":"HS256","typ":"JWT"}
        // Payload: {"exp":2000000000}
        // exp=2000000000 is 2033-05-18T03:33:20Z

        // Base64url-encoded header
        QString header = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9";
        // Base64url-encoded payload {"exp":2000000000}
        QString payload = "eyJleHAiOjIwMDAwMDAwMDB9";
        // Dummy signature
        QString signature = "dummy-signature";

        QString token = header + "." + payload + "." + signature;

        auto exp = TokenManager::decodeJwtExp(token);
        QVERIFY(exp.has_value());

        QDateTime expected = QDateTime::fromSecsSinceEpoch(2000000000, Qt::UTC);
        QCOMPARE(*exp, expected);
    }

    void testDecodeExpFutureToken()
    {
        // Create a token expiring far in the future
        qint64 farFuture = QDateTime::currentDateTimeUtc().addYears(10)
                               .toSecsSinceEpoch();

        QJsonObject payloadObj;
        payloadObj["exp"] = static_cast<double>(farFuture);
        QByteArray payloadJson = QJsonDocument(payloadObj).toJson(QJsonDocument::Compact);
        QString payloadB64 = payloadJson.toBase64(
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);

        QString token = "eyJhbGciOiJIUzI1NiJ9." + payloadB64 + ".sig";

        auto exp = TokenManager::decodeJwtExp(token);
        QVERIFY(exp.has_value());
        QCOMPARE(exp->toSecsSinceEpoch(), farFuture);
    }

    void testDecodeExpPastToken()
    {
        // Create a token that expired one hour ago
        qint64 pastExp = QDateTime::currentDateTimeUtc().addSecs(-3600)
                             .toSecsSinceEpoch();

        QJsonObject payloadObj;
        payloadObj["exp"] = static_cast<double>(pastExp);
        QByteArray payloadJson = QJsonDocument(payloadObj).toJson(QJsonDocument::Compact);
        QString payloadB64 = payloadJson.toBase64(
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);

        QString token = "eyJhbGciOiJIUzI1NiJ9." + payloadB64 + ".sig";

        auto exp = TokenManager::decodeJwtExp(token);
        QVERIFY(exp.has_value());
        QCOMPARE(exp->toSecsSinceEpoch(), pastExp);
    }

    void testDecodeExpMissingExp()
    {
        // Payload with no exp field
        QJsonObject payloadObj;
        payloadObj["sub"] = "user123";
        QByteArray payloadJson = QJsonDocument(payloadObj).toJson(QJsonDocument::Compact);
        QString payloadB64 = payloadJson.toBase64(
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);

        QString token = "eyJhbGciOiJIUzI1NiJ9." + payloadB64 + ".sig";

        auto exp = TokenManager::decodeJwtExp(token);
        QVERIFY(!exp.has_value());
    }

    void testDecodeExpInvalidToken()
    {
        // Not a valid JWT (only one part)
        auto exp = TokenManager::decodeJwtExp("not-a-jwt");
        QVERIFY(!exp.has_value());
    }

    void testDecodeExpEmptyToken()
    {
        auto exp = TokenManager::decodeJwtExp("");
        QVERIFY(!exp.has_value());
    }

    void testDecodeExpMalformedPayload()
    {
        // Two parts but the second is not valid base64
        auto exp = TokenManager::decodeJwtExp(
            "header.!!!not-valid-base64!!!.sig");
        QVERIFY(!exp.has_value());
    }

    // ------------------------------------------------------------------
    // Initial state
    // ------------------------------------------------------------------

    void testInitialState()
    {
        TokenManager tm;
        QVERIFY(!tm.isLoggedIn());
        QVERIFY(tm.accessToken().isEmpty());
        QVERIFY(tm.refreshToken().isEmpty());
    }

    // ------------------------------------------------------------------
    // Keychain round-trip
    // ------------------------------------------------------------------

    void testKeychainWriteRead()
    {
        // Write a test value
        auto writeResult = Keychain::write("test_access", "my-access-token-value");
        QVERIFY(writeResult.has_value());

        // Read it back
        auto readResult = Keychain::read("test_access");
        QVERIFY(readResult.has_value());
        QCOMPARE(readResult.value(), QString("my-access-token-value"));
    }

    void testKeychainOverwrite()
    {
        Keychain::write("test_access", "first-value");
        Keychain::write("test_access", "second-value");

        auto readResult = Keychain::read("test_access");
        QVERIFY(readResult.has_value());
        QCOMPARE(readResult.value(), QString("second-value"));
    }

    void testKeychainRemove()
    {
        Keychain::write("test_refresh", "refresh-token-value");

        auto removeResult = Keychain::remove("test_refresh");
        QVERIFY(removeResult.has_value());

        // Reading a removed key should fail
        auto readResult = Keychain::read("test_refresh");
        QVERIFY(!readResult.has_value());
    }

    void testKeychainReadMissing()
    {
        auto readResult = Keychain::read("key-that-does-not-exist");
        QVERIFY(!readResult.has_value());
        QCOMPARE(readResult.error().code, std::string("NOT_FOUND"));
    }

    // ------------------------------------------------------------------
    // Server URL
    // ------------------------------------------------------------------

    void testServerUrl()
    {
        TokenManager tm;
        QVERIFY(tm.serverUrl().isEmpty());

        tm.setServerUrl("https://api.example.com");
        QCOMPARE(tm.serverUrl(), QString("https://api.example.com"));

        tm.setServerUrl("http://localhost:8000");
        QCOMPARE(tm.serverUrl(), QString("http://localhost:8000"));
    }
};

QTEST_MAIN(TestTokenManager)
#include "tst_TokenManager.moc"
