/****************************************************************************
** Meta object code from reading C++ file 'ChatService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/services/ChatService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChatService.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11ChatServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto ChatService::qt_create_metaobjectdata<qt_meta_tag_ZN11ChatServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ChatService",
        "streamInitReceived",
        "",
        "aiUserId",
        "conversationId",
        "messageId",
        "streamChunkReceived",
        "content",
        "streamDoneReceived",
        "streamErrorReceived",
        "code",
        "message",
        "messageTimedOut",
        "conversationReplaced",
        "newConversationId",
        "messagesChanged",
        "messageAppended",
        "MessageDTO",
        "msg",
        "messageContentUpdated",
        "clientUuid",
        "QJsonObject",
        "messageMarkedComplete",
        "serverId",
        "onStreamInit",
        "timestamp",
        "onStreamChunk",
        "chunk",
        "onStreamDone",
        "onStreamError",
        "sendMessage"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'streamInitReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'streamChunkReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'streamDoneReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 5 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'streamErrorReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 10 }, { QMetaType::QString, 11 },
        }}),
        // Signal 'messageTimedOut'
        QtMocHelpers::SignalData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'conversationReplaced'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'messagesChanged'
        QtMocHelpers::SignalData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'messageAppended'
        QtMocHelpers::SignalData<void(const QString &, const MessageDTO &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 17, 18 },
        }}),
        // Signal 'messageContentUpdated'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QJsonObject &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 20 }, { 0x80000000 | 21, 7 },
        }}),
        // Signal 'messageMarkedComplete'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 20 }, { QMetaType::QString, 23 },
        }}),
        // Slot 'onStreamInit'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::QString, 5 }, { QMetaType::QString, 25 },
        }}),
        // Slot 'onStreamChunk'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::QString, 27 },
        }}),
        // Slot 'onStreamDone'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::QString, 5 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onStreamError'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &)>(29, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::QString, 10 }, { QMetaType::QString, 11 },
        }}),
        // Method 'sendMessage'
        QtMocHelpers::MethodData<void(const QString &, const QString &, const QString &)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 7 }, { QMetaType::QString, 4 },
        }}),
        // Method 'sendMessage'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(30, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ChatService, qt_meta_tag_ZN11ChatServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ChatService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11ChatServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11ChatServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11ChatServiceE_t>.metaTypes,
    nullptr
} };

void ChatService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ChatService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->streamInitReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->streamChunkReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->streamDoneReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 3: _t->streamErrorReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->messageTimedOut((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->conversationReplaced((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->messagesChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->messageAppended((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<MessageDTO>>(_a[2]))); break;
        case 8: _t->messageContentUpdated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[3]))); break;
        case 9: _t->messageMarkedComplete((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 10: _t->onStreamInit((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 11: _t->onStreamChunk((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 12: _t->onStreamDone((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 13: _t->onStreamError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 14: _t->sendMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 15: _t->sendMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< MessageDTO >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & , const QString & , const QString & )>(_a, &ChatService::streamInitReceived, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & , const QString & )>(_a, &ChatService::streamChunkReceived, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & , const QString & , const QString & )>(_a, &ChatService::streamDoneReceived, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & , const QString & , const QString & )>(_a, &ChatService::streamErrorReceived, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & )>(_a, &ChatService::messageTimedOut, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & )>(_a, &ChatService::conversationReplaced, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & )>(_a, &ChatService::messagesChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & , const MessageDTO & )>(_a, &ChatService::messageAppended, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & , const QString & , const QJsonObject & )>(_a, &ChatService::messageContentUpdated, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatService::*)(const QString & , const QString & , const QString & )>(_a, &ChatService::messageMarkedComplete, 9))
            return;
    }
}

const QMetaObject *ChatService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChatService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11ChatServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ChatService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void ChatService::streamInitReceived(const QString & _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void ChatService::streamChunkReceived(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void ChatService::streamDoneReceived(const QString & _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3);
}

// SIGNAL 3
void ChatService::streamErrorReceived(const QString & _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3);
}

// SIGNAL 4
void ChatService::messageTimedOut(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ChatService::conversationReplaced(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ChatService::messagesChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void ChatService::messageAppended(const QString & _t1, const MessageDTO & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void ChatService::messageContentUpdated(const QString & _t1, const QString & _t2, const QJsonObject & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2, _t3);
}

// SIGNAL 9
void ChatService::messageMarkedComplete(const QString & _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
