#include <QtTest>
#include "../src/core/storage/Database.h"

class TestDatabase : public QObject {
    Q_OBJECT
private slots:
    void initTestCase()
    {
        // Ensure clean state before all tests
    }

    void testInsertAndFind()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO msg;
        msg.clientUuid = "test-uuid";
        msg.aiUserId = "ai-1";
        msg.senderType = "user";
        msg.msgType = "text";
        msg.isComplete = true;
        msg.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();

        auto id = db.insertMessage(msg, "ai-1");
        QVERIFY(id > 0);

        auto found = db.findByClientUuid("test-uuid");
        QVERIFY(found.has_value());
        QCOMPARE(found->clientUuid, std::string("test-uuid"));
        QCOMPARE(found->senderType, std::string("user"));
        QCOMPARE(found->msgType, std::string("text"));
    }

    void testInsertWithServerId()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO msg;
        msg.serverId = 42;
        msg.clientUuid = "with-server-id";
        msg.aiUserId = "ai-1";
        msg.senderType = "ai";
        msg.msgType = "text";
        msg.isComplete = true;
        msg.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();

        db.insertMessage(msg, "ai-1");

        auto found = db.findByServerId("42");
        QVERIFY(found.has_value());
        QVERIFY(!found->serverId.empty());
        QCOMPARE(found->serverId, std::string("42"));
        QCOMPARE(found->clientUuid, std::string("with-server-id"));
    }

    void testFindByServerIdNotFound()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        auto found = db.findByServerId("99999");
        QVERIFY(!found.has_value());
    }

    void testFindByClientUuidNotFound()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        auto found = db.findByClientUuid("nonexistent");
        QVERIFY(!found.has_value());
    }

    void testDeleteStaleUnsynced()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        // Insert an old unsynced message (no server_id, old timestamp)
        MessageDTO old;
        old.clientUuid = "old-unsynced";
        old.aiUserId = "ai-1";
        old.senderType = "user";
        old.msgType = "text";
        old.isComplete = true;
        old.timestamp = QDateTime::currentDateTimeUtc()
            .addSecs(-60 * 60).toString(Qt::ISODate).toStdString(); // 1 hour ago

        db.insertMessage(old, "ai-1");

        // Insert a recent unsynced message (should survive)
        MessageDTO recent;
        recent.clientUuid = "recent-unsynced";
        recent.aiUserId = "ai-1";
        recent.senderType = "user";
        recent.msgType = "text";
        recent.isComplete = true;
        recent.timestamp = QDateTime::currentDateTimeUtc()
            .addSecs(-60).toString(Qt::ISODate).toStdString(); // 1 minute ago

        db.insertMessage(recent, "ai-1");

        // Delete stale (threshold: 30 minutes)
        db.deleteStaleUnsynced("ai-1", 30);

        // The old message should be gone
        auto oldFound = db.findByClientUuid("old-unsynced");
        QVERIFY(!oldFound.has_value());

        // The recent message should still exist
        auto recentFound = db.findByClientUuid("recent-unsynced");
        QVERIFY(recentFound.has_value());
        QCOMPARE(recentFound->clientUuid, std::string("recent-unsynced"));
    }

    void testDeleteStaleUnsyncedSkipsSyncedMessages()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        // Insert an old message WITH a serverId (synced)
        MessageDTO synced;
        synced.serverId = 100;
        synced.clientUuid = "old-synced";
        synced.aiUserId = "ai-1";
        synced.senderType = "user";
        synced.msgType = "text";
        synced.isComplete = true;
        synced.timestamp = QDateTime::currentDateTimeUtc()
            .addSecs(-60 * 60).toString(Qt::ISODate).toStdString();

        db.insertMessage(synced, "ai-1");

        db.deleteStaleUnsynced("ai-1", 30);

        // The synced message should remain even though its timestamp is old,
        // because it has a serverId
        auto found = db.findByClientUuid("old-synced");
        QVERIFY(found.has_value());
        QCOMPARE(found->clientUuid, std::string("old-synced"));
    }

    void testUnreadCounts()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        // Insert an unread AI message
        MessageDTO m1;
        m1.clientUuid = "uuid-1";
        m1.aiUserId = "ai-1";
        m1.senderType = "ai";
        m1.msgType = "text";
        m1.isRead = false;
        m1.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m1, "ai-1");

        // Insert a read message (should not count)
        MessageDTO m2;
        m2.clientUuid = "uuid-2";
        m2.aiUserId = "ai-1";
        m2.senderType = "ai";
        m2.msgType = "text";
        m2.isRead = true;
        m2.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m2, "ai-1");

        // Insert an unread message for a different AI user
        MessageDTO m3;
        m3.clientUuid = "uuid-3";
        m3.aiUserId = "ai-2";
        m3.senderType = "ai";
        m3.msgType = "text";
        m3.isRead = false;
        m3.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m3, "ai-2");

        auto counts = db.getUnreadCounts();
        QCOMPARE(counts["ai-1"], 1);
        QCOMPARE(counts["ai-2"], 1);

        // Mark ai-1 as read
        db.markAsRead("ai-1");
        counts = db.getUnreadCounts();
        QCOMPARE(counts["ai-1"], 0);
        // ai-2 should be unaffected
        QCOMPARE(counts["ai-2"], 1);
    }

    void testMarkAsRead()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO m;
        m.clientUuid = "to-read";
        m.aiUserId = "ai-1";
        m.senderType = "ai";
        m.msgType = "text";
        m.isRead = false;
        m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m, "ai-1");

        db.markAsRead("ai-1");

        // Verify the message is now marked as read
        auto found = db.findByClientUuid("to-read");
        QVERIFY(found.has_value());
        QVERIFY(found->isRead);
    }

    void testUpsertInsertNew()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO msg;
        msg.clientUuid = "upsert-new";
        msg.aiUserId = "ai-1";
        msg.senderType = "user";
        msg.msgType = "text";
    msg.content = QString::fromUtf8(QJsonDocument(QJsonObject{{"text", "hello"}}).toJson(QJsonDocument::Compact)).toStdString();
        msg.isComplete = true;
        msg.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();

        db.upsertMessage(msg, "ai-1");

        auto found = db.findByClientUuid("upsert-new");
        QVERIFY(found.has_value());
        QCOMPARE(found->clientUuid, std::string("upsert-new"));
    }

    void testUpsertUpdateExisting()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        // Insert initial message
        MessageDTO msg;
        msg.clientUuid = "upsert-update";
        msg.aiUserId = "ai-1";
        msg.senderType = "user";
        msg.msgType = "text";
        msg.isComplete = false;  // incomplete initially
        msg.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(msg, "ai-1");

        // Now upsert with updated data
        MessageDTO updated;
        updated.clientUuid = "upsert-update";
        updated.aiUserId = "ai-1";
        updated.senderType = "ai";      // changed
        updated.msgType = "text";
        updated.isComplete = true;       // changed
        updated.serverId = 42;           // now has serverId
        updated.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.upsertMessage(updated, "ai-1");

        auto found = db.findByClientUuid("upsert-update");
        QVERIFY(found.has_value());
        QCOMPARE(found->senderType, std::string("ai"));
        QVERIFY(found->isComplete);
        QVERIFY(!found->serverId.empty());
        QCOMPARE(found->serverId, std::string("42"));
    }

    void testGetLocalMessages()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        // Insert messages for two different AI users
        for (int i = 0; i < 5; ++i) {
            MessageDTO m;
            m.clientUuid = QString("local-ai1-%1").arg(i).toStdString();
            m.aiUserId = "ai-1";
            m.senderType = "ai";
            m.msgType = "text";
            m.timestamp = QDateTime::currentDateTimeUtc()
                .addSecs(-i * 60).toString(Qt::ISODate).toStdString();
            db.insertMessage(m, "ai-1");

            MessageDTO m2;
            m2.clientUuid = QString("local-ai2-%1").arg(i).toStdString();
            m2.aiUserId = "ai-2";
            m2.senderType = "ai";
            m2.msgType = "text";
            m2.timestamp = QDateTime::currentDateTimeUtc()
                .addSecs(-i * 60).toString(Qt::ISODate).toStdString();
            db.insertMessage(m2, "ai-2");
        }

        auto msgs1 = db.getLocalMessages("ai-1", 200);
        QCOMPARE(msgs1.size(), 5);

        auto msgs2 = db.getLocalMessages("ai-2", 2);
        QCOMPARE(msgs2.size(), 2);
    }

    void testGetLocalMessagesBefore()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        auto now = QDateTime::currentDateTimeUtc();

        // Insert 3 messages with timestamps 0, -60s, -120s
        for (int i = 0; i < 3; ++i) {
            MessageDTO m;
            m.clientUuid = QString("before-msg-%1").arg(i).toStdString();
            m.aiUserId = "ai-1";
            m.senderType = "ai";
            m.msgType = "text";
            m.timestamp = now.addSecs(-i * 60).toString(Qt::ISODate).toStdString();
            db.insertMessage(m, "ai-1");
        }

        // Query before now+1s (should get all 3)
        auto all = db.getLocalMessagesBefore("ai-1",
            now.addSecs(1).toString(Qt::ISODate), 50);
        QCOMPARE(all.size(), 3);

        // Query before -30s (should get only the 2 older ones)
        auto older = db.getLocalMessagesBefore("ai-1",
            now.addSecs(-30).toString(Qt::ISODate), 50);
        QCOMPARE(older.size(), 2);
    }

    void testDeleteMessageSoft()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO m;
        m.clientUuid = "soft-delete";
        m.aiUserId = "ai-1";
        m.senderType = "user";
        m.msgType = "text";
        m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m, "ai-1");

        db.deleteMessage("soft-delete");

        // findByClientUuid should not return soft-deleted messages
        auto found = db.findByClientUuid("soft-delete");
        QVERIFY(!found.has_value());
    }

    void testDeleteMessagesSince()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        auto now = QDateTime::currentDateTimeUtc();
        auto cutoff = now.addSecs(-300).toString(Qt::ISODate); // 5 min ago

        // Insert message before cutoff
        MessageDTO old;
        old.clientUuid = "old-msg";
        old.aiUserId = "ai-1";
        old.senderType = "ai";
        old.msgType = "text";
        old.timestamp = now.addSecs(-600).toString(Qt::ISODate).toStdString(); // 10 min ago
        db.insertMessage(old, "ai-1");

        // Insert message after cutoff
        MessageDTO recent;
        recent.clientUuid = "recent-msg";
        recent.aiUserId = "ai-1";
        recent.senderType = "ai";
        recent.msgType = "text";
        recent.timestamp = now.addSecs(-60).toString(Qt::ISODate).toStdString(); // 1 min ago
        db.insertMessage(recent, "ai-1");

        db.deleteMessagesSince("ai-1", cutoff);

        // Old message (before cutoff) should remain
        auto oldFound = db.findByClientUuid("old-msg");
        QVERIFY(oldFound.has_value());

        // Recent message (after cutoff) should be deleted
        auto recentFound = db.findByClientUuid("recent-msg");
        QVERIFY(!recentFound.has_value());
    }

    void testDeleteIncompleteMessages()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO incomplete;
        incomplete.clientUuid = "incomplete";
        incomplete.aiUserId = "ai-1";
        incomplete.senderType = "ai";
        incomplete.msgType = "text";
        incomplete.isComplete = false;
        incomplete.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(incomplete, "ai-1");

        MessageDTO complete;
        complete.clientUuid = "complete";
        complete.aiUserId = "ai-1";
        complete.senderType = "ai";
        complete.msgType = "text";
        complete.isComplete = true;
        complete.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(complete, "ai-1");

        db.deleteIncompleteMessages("ai-1");

        auto incFound = db.findByClientUuid("incomplete");
        QVERIFY(!incFound.has_value());

        auto compFound = db.findByClientUuid("complete");
        QVERIFY(compFound.has_value());
    }

    void testHasLocalMessages()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        QVERIFY(!db.hasLocalMessages("ai-1"));

        MessageDTO m;
        m.clientUuid = "exists";
        m.aiUserId = "ai-1";
        m.senderType = "user";
        m.msgType = "text";
        m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m, "ai-1");

        QVERIFY(db.hasLocalMessages("ai-1"));
        QVERIFY(!db.hasLocalMessages("ai-2"));
    }

    void testUpdateMessage()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        MessageDTO m;
        m.clientUuid = "to-update";
        m.aiUserId = "ai-1";
        m.senderType = "user";
        m.msgType = "text";
        m.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
        db.insertMessage(m, "ai-1");

        QVariantMap values;
        values["server_id"] = static_cast<qint64>(99);
        values["is_complete"] = 1;
        db.updateMessage("to-update", values);

        auto found = db.findByClientUuid("to-update");
        QVERIFY(found.has_value());
        QVERIFY(!found->serverId.empty());
        QCOMPARE(found->serverId, std::string("99"));
        QVERIFY(found->isComplete);
    }

    void testSyncMetaRoundTrip()
    {
        Database db;
        QVERIFY(db.open(":memory:"));

        // Initially no sync time
        auto initial = db.getSyncTime("ai-1");
        QVERIFY(!initial.has_value());

        // Set sync time
        QString syncTime = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        db.setSyncTime("ai-1", syncTime);

        // Read it back
        auto stored = db.getSyncTime("ai-1");
        QVERIFY(stored.has_value());
        QCOMPARE(*stored, syncTime);

        // Overwrite
        QString newSyncTime = QDateTime::currentDateTimeUtc()
            .addSecs(3600).toString(Qt::ISODate);
        db.setSyncTime("ai-1", newSyncTime);
        auto updated = db.getSyncTime("ai-1");
        QVERIFY(updated.has_value());
        QCOMPARE(*updated, newSyncTime);
    }
};

QTEST_MAIN(TestDatabase)
#include "tst_Database.moc"
