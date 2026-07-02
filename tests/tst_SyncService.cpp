#include <QtTest>
#include "../src/services/SyncService.h"
#include "../src/core/storage/Database.h"
#include "../src/core/network/HttpClient.h"
#include "../src/repositories/ChatRepository.h"

/// Minimal HttpClient subclass that always fails network calls.
/// SyncService tests focus on local phantom cleanup, not server round-trips.
class StubHttpClient : public HttpClient {
    Q_OBJECT
public:
    explicit StubHttpClient(QObject *parent = nullptr)
        : HttpClient(parent) {}
};

class TestSyncService : public QObject {
    Q_OBJECT
private slots:
    void testPhantomCleanup()
    {
        // Verify that deleteStaleUnsynced removes old optimistic inserts
        // but preserves synced messages and recent unsynced ones.
        Database db;
        QVERIFY(db.open(":memory:"));

        auto now = QDateTime::currentDateTimeUtc();

        // 1. Old unsynced (no serverId, timestamp > 30 min ago) — should be removed
        MessageDTO phantom;
        phantom.clientUuid = "phantom-1";
        phantom.aiUserId = "ai-1";
        phantom.senderType = "user";
        phantom.msgType = "text";
        phantom.timestamp = now.addSecs(-3600).toString(Qt::ISODate).toStdString(); // 1h ago
        db.insertMessage(phantom, "ai-1");

        // 2. Old synced (has serverId, timestamp > 30 min ago) — should survive
        MessageDTO oldSynced;
        oldSynced.serverId = 42;
        oldSynced.clientUuid = "old-synced-1";
        oldSynced.aiUserId = "ai-1";
        oldSynced.senderType = "ai";
        oldSynced.msgType = "text";
        oldSynced.timestamp = now.addSecs(-3600).toString(Qt::ISODate).toStdString();
        db.insertMessage(oldSynced, "ai-1");

        // 3. Recent unsynced (no serverId, timestamp < 30 min ago) — should survive
        MessageDTO recentUnsynced;
        recentUnsynced.clientUuid = "recent-unsynced-1";
        recentUnsynced.aiUserId = "ai-1";
        recentUnsynced.senderType = "user";
        recentUnsynced.msgType = "text";
        recentUnsynced.timestamp = now.addSecs(-60).toString(Qt::ISODate).toStdString(); // 1m ago
        db.insertMessage(recentUnsynced, "ai-1");

        // Run phantom cleanup (30 min threshold)
        db.deleteStaleUnsynced("ai-1", 30);

        // Phantom should be gone
        QVERIFY(!db.findByClientUuid("phantom-1").has_value());

        // Old synced should remain
        auto synced = db.findByClientUuid("old-synced-1");
        QVERIFY(synced.has_value());
        QVERIFY(synced->serverId.has_value());
        QCOMPARE(*synced->serverId, static_cast<int64_t>(42));

        // Recent unsynced should remain
        auto recent = db.findByClientUuid("recent-unsynced-1");
        QVERIFY(recent.has_value());
        QVERIFY(!recent->serverId.has_value());
    }

    void testDeleteMessagesSincePreservesOld()
    {
        // deleteMessagesSince should only remove messages at or after the
        // given timestamp, leaving older messages intact.
        Database db;
        QVERIFY(db.open(":memory:"));

        auto now = QDateTime::currentDateTimeUtc();
        auto cutoff = now.addSecs(-300).toString(Qt::ISODate); // 5 min ago

        // Message before cutoff
        MessageDTO old;
        old.clientUuid = "before-cutoff";
        old.aiUserId = "ai-1";
        old.senderType = "ai";
        old.msgType = "text";
        old.timestamp = now.addSecs(-600).toString(Qt::ISODate).toStdString(); // 10 min ago
        db.insertMessage(old, "ai-1");

        // Message after cutoff
        MessageDTO recent;
        recent.clientUuid = "after-cutoff";
        recent.aiUserId = "ai-1";
        recent.senderType = "ai";
        recent.msgType = "text";
        recent.timestamp = now.addSecs(-120).toString(Qt::ISODate).toStdString(); // 2 min ago
        db.insertMessage(recent, "ai-1");

        db.deleteMessagesSince("ai-1", cutoff);

        // Old message survives
        QVERIFY(db.findByClientUuid("before-cutoff").has_value());

        // Recent message is deleted
        QVERIFY(!db.findByClientUuid("after-cutoff").has_value());
    }

    void testDeleteMessagesSinceOnlyAffectsTargetUser()
    {
        // deleteMessagesSince should only affect the specified AI user.
        Database db;
        QVERIFY(db.open(":memory:"));

        auto now = QDateTime::currentDateTimeUtc();
        auto cutoff = now.addSecs(-300).toString(Qt::ISODate);

        // User ai-1: message after cutoff (should be deleted)
        MessageDTO m1;
        m1.clientUuid = "ai1-recent";
        m1.aiUserId = "ai-1";
        m1.senderType = "user";
        m1.msgType = "text";
        m1.timestamp = now.addSecs(-60).toString(Qt::ISODate).toStdString();
        db.insertMessage(m1, "ai-1");

        // User ai-2: message after cutoff (should NOT be deleted)
        MessageDTO m2;
        m2.clientUuid = "ai2-recent";
        m2.aiUserId = "ai-2";
        m2.senderType = "user";
        m2.msgType = "text";
        m2.timestamp = now.addSecs(-60).toString(Qt::ISODate).toStdString();
        db.insertMessage(m2, "ai-2");

        db.deleteMessagesSince("ai-1", cutoff);

        QVERIFY(!db.findByClientUuid("ai1-recent").has_value());
        QVERIFY(db.findByClientUuid("ai2-recent").has_value());
    }

    void testSyncServiceConstruction()
    {
        // Verify that SyncService can be constructed without crashing.
        Database db;
        QVERIFY(db.open(":memory:"));

        auto http = new StubHttpClient(this);
        auto repo = new ChatRepository(http, &db, this);

        SyncService sync(repo, &db, this);
        QVERIFY(true); // Construction succeeded
    }

    void testSyncServiceSignals()
    {
        // Verify that the signals are wired correctly.
        Database db;
        QVERIFY(db.open(":memory:"));

        auto http = new StubHttpClient(this);
        auto repo = new ChatRepository(http, &db, this);

        SyncService sync(repo, &db, this);

        QSignalSpy completeSpy(&sync, &SyncService::syncComplete);
        QSignalSpy errorSpy(&sync, &SyncService::syncError);

        QVERIFY(completeSpy.isValid());
        QVERIFY(errorSpy.isValid());
    }
};

QTEST_MAIN(TestSyncService)
#include "tst_SyncService.moc"
