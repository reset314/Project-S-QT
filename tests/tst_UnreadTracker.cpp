#include <QtTest>
#include "../src/services/UnreadTracker.h"
#include "../src/core/storage/Database.h"
#include "../src/repositories/ConversationRepository.h"

class TestUnreadTracker : public QObject {
    Q_OBJECT
private slots:
    void testInitLoadsCounts()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        // Insert unread messages for two AI users
        MessageDTO m1;
        m1.clientUuid = "ur-uuid-1";
        m1.aiUserId = "ai-1";
        m1.senderType = "ai";
        m1.msgType = "text";
        m1.isRead = false;
        m1.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m1, "ai-1");

        MessageDTO m2;
        m2.clientUuid = "ur-uuid-2";
        m2.aiUserId = "ai-1";
        m2.senderType = "ai";
        m2.msgType = "text";
        m2.isRead = false;
        m2.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m2, "ai-1");

        MessageDTO m3;
        m3.clientUuid = "ur-uuid-3";
        m3.aiUserId = "ai-2";
        m3.senderType = "ai";
        m3.msgType = "text";
        m3.isRead = false;
        m3.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m3, "ai-2");

        UnreadTracker tracker(&db, nullptr);
        tracker.init();

        QCOMPARE(tracker.totalUnread(), 3);

        auto map = tracker.unreadMap();
        QCOMPARE(map["ai-1"].toInt(), 2);
        QCOMPARE(map["ai-2"].toInt(), 1);
    }

    void testInitEmpty()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        UnreadTracker tracker(&db, nullptr);
        tracker.init();

        QCOMPARE(tracker.totalUnread(), 0);
        QVERIFY(tracker.unreadMap().isEmpty());
    }

    void testClearAndMarkRead()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO m;
        m.clientUuid = "clear-uuid";
        m.aiUserId = "ai-1";
        m.senderType = "ai";
        m.msgType = "text";
        m.isRead = false;
        m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m, "ai-1");

        UnreadTracker tracker(&db, nullptr);
        tracker.init();
        QCOMPARE(tracker.totalUnread(), 1);

        tracker.clearAndMarkRead("ai-1");
        QCOMPARE(tracker.totalUnread(), 0);

        // Verify database was also updated — after markAsRead,
        // getUnreadCounts() omits the key entirely (no unread messages).
        auto counts = db.getUnreadCounts();
        QVERIFY(!counts.contains("ai-1"));
    }

    void testClearAndMarkReadIdempotent()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        UnreadTracker tracker(&db, nullptr);

        // Call clear on a user that has no unread — should not crash
        tracker.clearAndMarkRead("nonexistent");
        QCOMPARE(tracker.totalUnread(), 0);

        // Call clear twice — should not crash
        tracker.clearAndMarkRead("ai-1");
        tracker.clearAndMarkRead("ai-1");
        QCOMPARE(tracker.totalUnread(), 0);
    }

    void testActiveChatExclusion()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        UnreadTracker tracker(&db, nullptr);

        // We can't fully test onIncomingMessage without a ConversationRepository
        // that resolves conversationId -> aiUserId, but we can test the
        // setActiveAiUser + clearAndMarkRead interaction.

        // Set active AI user
        tracker.setActiveAiUser("ai-1");

        // When switching to a chat, unread count should be auto-cleared
        // (setActiveAiUser calls clearAndMarkRead internally)
        QCOMPARE(tracker.totalUnread(), 0);
    }

    void testUnreadMapProperty()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        // Insert messages with different unread counts
        for (int i = 0; i < 3; ++i) {
            MessageDTO m;
            m.clientUuid = QString("map-uuid-%1").arg(i).toStdString();
            m.aiUserId = "ai-1";
            m.senderType = "ai";
            m.msgType = "text";
            m.isRead = false;
            m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
            db.insertMessage(m, "ai-1");
        }

        UnreadTracker tracker(&db, nullptr);
        tracker.init();

        auto map = tracker.unreadMap();
        QCOMPARE(map.size(), 1);
        QCOMPARE(map["ai-1"].toInt(), 3);

        // Zero-count entries should not appear in the map
        tracker.clearAndMarkRead("ai-1");
        auto clearedMap = tracker.unreadMap();
        QVERIFY(!clearedMap.contains("ai-1"));
    }

    void testTotalUnreadSignal()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO m;
        m.clientUuid = "signal-uuid";
        m.aiUserId = "ai-1";
        m.senderType = "ai";
        m.msgType = "text";
        m.isRead = false;
        m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m, "ai-1");

        UnreadTracker tracker(&db, nullptr);

        QSignalSpy spy(&tracker, &UnreadTracker::totalUnreadChanged);

        tracker.init();

        // init() emits totalUnreadChanged
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy[0][0].toInt(), 1);
    }

    void testUnreadMapSignal()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO m;
        m.clientUuid = "map-signal-uuid";
        m.aiUserId = "ai-1";
        m.senderType = "ai";
        m.msgType = "text";
        m.isRead = false;
        m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m, "ai-1");

        UnreadTracker tracker(&db, nullptr);

        QSignalSpy spy(&tracker, &UnreadTracker::unreadMapChanged);

        tracker.init();

        // init() emits unreadMapChanged
        QCOMPARE(spy.size(), 1);
    }

    void testSetActiveAiUserSwitching()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        UnreadTracker tracker(&db, nullptr);

        // Switch to ai-1
        tracker.setActiveAiUser("ai-1");

        // Switch to same user — should not trigger duplicate clear
        tracker.setActiveAiUser("ai-1");

        // Switch to different user
        tracker.setActiveAiUser("ai-2");

        // Switch to empty (no active chat)
        tracker.setActiveAiUser("");
    }

    void testInitWithNullDatabase()
    {
        // Constructor should not crash with nullptr database
        UnreadTracker tracker(nullptr, nullptr);
        tracker.init(); // Should return early, not crash
        QCOMPARE(tracker.totalUnread(), 0);
    }
};

QTEST_MAIN(TestUnreadTracker)
#include "tst_UnreadTracker.moc"
