#include <QtTest>
#include <QSet>
#include "../src/core/utils/UuidGenerator.h"

class TestUuidGenerator : public QObject {
    Q_OBJECT
private slots:
    void testNotEmpty()
    {
        auto uuid = UuidGenerator::generate();
        QVERIFY(!uuid.isEmpty());
    }

    void testNoDashes()
    {
        auto uuid = UuidGenerator::generate();
        QVERIFY(!uuid.contains('-'));
    }

    void testLength()
    {
        // UUID v4 without dashes is 32 hex characters
        auto uuid = UuidGenerator::generate();
        QCOMPARE(uuid.length(), 32);
    }

    void testOnlyHexCharacters()
    {
        auto uuid = UuidGenerator::generate();
        QRegularExpression hexOnly("^[0-9a-fA-F]{32}$");
        QVERIFY(hexOnly.match(uuid).hasMatch());
    }

    void testUnique()
    {
        // Generate 1000 UUIDs and verify all are unique
        QSet<QString> seen;
        for (int i = 0; i < 1000; ++i) {
            auto uuid = UuidGenerator::generate();
            QVERIFY(!seen.contains(uuid));
            seen.insert(uuid);
        }
        QCOMPARE(seen.size(), 1000);
    }

    void testV4Format()
    {
        // QUuid::createUuid() creates v4 (random) UUIDs.
        // The version nibble should be 4, but since we strip dashes
        // the position of the version character depends on parsing.
        // With dashes removed: xxxxxxxx-xxxx-Vxxx-xxxx-xxxxxxxxxxxx
        // Position 12 (0-indexed) of the 36-char string
        // After removing dashes (32 chars), the version nibble is at index 12.
        auto uuid = UuidGenerator::generate();
        QVERIFY(uuid.length() == 32);
        // The version character is at index 12 in the 32-char dashless form
        QCOMPARE(uuid[12].toLower(), QChar('4'));
    }
};

QTEST_MAIN(TestUuidGenerator)
#include "tst_UuidGenerator.moc"
