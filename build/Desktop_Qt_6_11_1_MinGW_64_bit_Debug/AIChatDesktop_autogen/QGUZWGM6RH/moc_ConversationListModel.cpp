/****************************************************************************
** Meta object code from reading C++ file 'ConversationListModel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/viewmodels/ConversationListModel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ConversationListModel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN21ConversationListModelE_t {};
} // unnamed namespace

template <> constexpr inline auto ConversationListModel::qt_create_metaobjectdata<qt_meta_tag_ZN21ConversationListModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ConversationListModel",
        "Roles",
        "IdRole",
        "UserIdRole",
        "AiUserIdRole",
        "TitleRole",
        "CreatedAtRole",
        "UpdatedAtRole",
        "EndedAtRole",
        "AiUserNameRole",
        "AiUserAvatarRole",
        "LastMessagePreviewRole"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Roles'
        QtMocHelpers::EnumData<enum Roles>(1, 1, QMC::EnumFlags{}).add({
            {    2, Roles::IdRole },
            {    3, Roles::UserIdRole },
            {    4, Roles::AiUserIdRole },
            {    5, Roles::TitleRole },
            {    6, Roles::CreatedAtRole },
            {    7, Roles::UpdatedAtRole },
            {    8, Roles::EndedAtRole },
            {    9, Roles::AiUserNameRole },
            {   10, Roles::AiUserAvatarRole },
            {   11, Roles::LastMessagePreviewRole },
        }),
    };
    return QtMocHelpers::metaObjectData<ConversationListModel, qt_meta_tag_ZN21ConversationListModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ConversationListModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21ConversationListModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21ConversationListModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN21ConversationListModelE_t>.metaTypes,
    nullptr
} };

void ConversationListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ConversationListModel *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *ConversationListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConversationListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21ConversationListModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int ConversationListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
