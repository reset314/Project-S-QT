/****************************************************************************
** Meta object code from reading C++ file 'LinuxDesktop.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/platform/LinuxDesktop.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LinuxDesktop.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12LinuxDesktopE_t {};
} // unnamed namespace

template <> constexpr inline auto LinuxDesktop::qt_create_metaobjectdata<qt_meta_tag_ZN12LinuxDesktopE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LinuxDesktop"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LinuxDesktop, qt_meta_tag_ZN12LinuxDesktopE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LinuxDesktop::staticMetaObject = { {
    QMetaObject::SuperData::link<PlatformIntegration::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LinuxDesktopE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LinuxDesktopE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12LinuxDesktopE_t>.metaTypes,
    nullptr
} };

void LinuxDesktop::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LinuxDesktop *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *LinuxDesktop::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LinuxDesktop::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12LinuxDesktopE_t>.strings))
        return static_cast<void*>(this);
    return PlatformIntegration::qt_metacast(_clname);
}

int LinuxDesktop::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PlatformIntegration::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
