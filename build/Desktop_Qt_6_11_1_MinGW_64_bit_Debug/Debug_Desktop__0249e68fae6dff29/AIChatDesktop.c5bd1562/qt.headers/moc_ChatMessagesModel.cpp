/****************************************************************************
** Meta object code from reading C++ file 'ChatMessagesModel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/viewmodels/ChatMessagesModel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChatMessagesModel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17ChatMessagesModelE_t {};
} // unnamed namespace

template <> constexpr inline auto ChatMessagesModel::qt_create_metaobjectdata<qt_meta_tag_ZN17ChatMessagesModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ChatMessagesModel",
        "activeAiUserIdChanged",
        "",
        "loadRequested",
        "aiUserId",
        "loadOlderRequested",
        "beforeTimestamp",
        "olderMessagesLoaded",
        "prependCount",
        "historyLoaded",
        "messageAppended",
        "setActiveAiUserId",
        "loadOlder",
        "oldestTimestamp",
        "activeAiUserId",
        "Roles",
        "ServerIdRole",
        "ClientUuidRole",
        "AiUserIdRole",
        "ConversationIdRole",
        "SenderTypeRole",
        "MsgTypeRole",
        "ContentRole",
        "MediaListRole",
        "IsCompleteRole",
        "RevokedAtRole",
        "TimestampRole"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'activeAiUserIdChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'loadRequested'
        QtMocHelpers::SignalData<void(const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'loadOlderRequested'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'olderMessagesLoaded'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'historyLoaded'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'messageAppended'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'setActiveAiUserId'
        QtMocHelpers::MethodData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Method 'loadOlder'
        QtMocHelpers::MethodData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'oldestTimestamp'
        QtMocHelpers::MethodData<QString() const>(13, 2, QMC::AccessPublic, QMetaType::QString),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'activeAiUserId'
        QtMocHelpers::PropertyData<QString>(14, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Roles'
        QtMocHelpers::EnumData<enum Roles>(15, 15, QMC::EnumFlags{}).add({
            {   16, Roles::ServerIdRole },
            {   17, Roles::ClientUuidRole },
            {   18, Roles::AiUserIdRole },
            {   19, Roles::ConversationIdRole },
            {   20, Roles::SenderTypeRole },
            {   21, Roles::MsgTypeRole },
            {   22, Roles::ContentRole },
            {   23, Roles::MediaListRole },
            {   24, Roles::IsCompleteRole },
            {   25, Roles::RevokedAtRole },
            {   26, Roles::TimestampRole },
        }),
    };
    return QtMocHelpers::metaObjectData<ChatMessagesModel, qt_meta_tag_ZN17ChatMessagesModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ChatMessagesModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ChatMessagesModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ChatMessagesModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17ChatMessagesModelE_t>.metaTypes,
    nullptr
} };

void ChatMessagesModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ChatMessagesModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->activeAiUserIdChanged(); break;
        case 1: _t->loadRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->loadOlderRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->olderMessagesLoaded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->historyLoaded(); break;
        case 5: _t->messageAppended(); break;
        case 6: _t->setActiveAiUserId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->loadOlder(); break;
        case 8: { QString _r = _t->oldestTimestamp();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ChatMessagesModel::*)()>(_a, &ChatMessagesModel::activeAiUserIdChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatMessagesModel::*)(const QString & )>(_a, &ChatMessagesModel::loadRequested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatMessagesModel::*)(const QString & , const QString & )>(_a, &ChatMessagesModel::loadOlderRequested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatMessagesModel::*)(int )>(_a, &ChatMessagesModel::olderMessagesLoaded, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatMessagesModel::*)()>(_a, &ChatMessagesModel::historyLoaded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChatMessagesModel::*)()>(_a, &ChatMessagesModel::messageAppended, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->activeAiUserId(); break;
        default: break;
        }
    }
}

const QMetaObject *ChatMessagesModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChatMessagesModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ChatMessagesModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int ChatMessagesModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ChatMessagesModel::activeAiUserIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ChatMessagesModel::loadRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ChatMessagesModel::loadOlderRequested(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void ChatMessagesModel::olderMessagesLoaded(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ChatMessagesModel::historyLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ChatMessagesModel::messageAppended()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
