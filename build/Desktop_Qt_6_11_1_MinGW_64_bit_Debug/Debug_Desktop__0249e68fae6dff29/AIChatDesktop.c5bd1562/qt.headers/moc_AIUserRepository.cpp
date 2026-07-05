/****************************************************************************
** Meta object code from reading C++ file 'AIUserRepository.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/repositories/AIUserRepository.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AIUserRepository.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16AIUserRepositoryE_t {};
} // unnamed namespace

template <> constexpr inline auto AIUserRepository::qt_create_metaobjectdata<qt_meta_tag_ZN16AIUserRepositoryE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AIUserRepository",
        "getAIUserJson",
        "QJsonObject",
        "",
        "aiUserId",
        "updateAIUserJson",
        "fields"
    };

    QtMocHelpers::UintData qt_methods {
        // Method 'getAIUserJson'
        QtMocHelpers::MethodData<QJsonObject(const QString &)>(1, 3, QMC::AccessPublic, 0x80000000 | 2, {{
            { QMetaType::QString, 4 },
        }}),
        // Method 'updateAIUserJson'
        QtMocHelpers::MethodData<QJsonObject(const QString &, const QJsonObject &)>(5, 3, QMC::AccessPublic, 0x80000000 | 2, {{
            { QMetaType::QString, 4 }, { 0x80000000 | 2, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AIUserRepository, qt_meta_tag_ZN16AIUserRepositoryE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AIUserRepository::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16AIUserRepositoryE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16AIUserRepositoryE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16AIUserRepositoryE_t>.metaTypes,
    nullptr
} };

void AIUserRepository::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AIUserRepository *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QJsonObject _r = _t->getAIUserJson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QJsonObject*>(_a[0]) = std::move(_r); }  break;
        case 1: { QJsonObject _r = _t->updateAIUserJson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QJsonObject*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject *AIUserRepository::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AIUserRepository::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16AIUserRepositoryE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AIUserRepository::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
