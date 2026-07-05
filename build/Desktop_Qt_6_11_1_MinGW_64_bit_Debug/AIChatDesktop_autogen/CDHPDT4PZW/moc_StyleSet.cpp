/****************************************************************************
** Meta object code from reading C++ file 'StyleSet.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/core/vendor/aqt-stylesheets/src/StyleSet.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StyleSet.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN3aqt11stylesheets8StyleSetE_t {};
} // unnamed namespace

template <> constexpr inline auto aqt::stylesheets::StyleSet::qt_create_metaobjectdata<qt_meta_tag_ZN3aqt11stylesheets8StyleSetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "aqt::stylesheets::StyleSet",
        "propsChanged",
        "",
        "nameChanged",
        "name",
        "pathChanged",
        "onParentChanged",
        "QQuickItem*",
        "pNewParent",
        "path",
        "props",
        "aqt::stylesheets::StyleSetProps*",
        "styleInfo"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'propsChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'nameChanged'
        QtMocHelpers::SignalData<void(const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'pathChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onParentChanged'
        QtMocHelpers::SlotData<void(QQuickItem *)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'name'
        QtMocHelpers::PropertyData<QString>(4, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'path'
        QtMocHelpers::PropertyData<QString>(9, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'props'
        QtMocHelpers::PropertyData<aqt::stylesheets::StyleSetProps*>(10, 0x80000000 | 11, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'styleInfo'
        QtMocHelpers::PropertyData<QString>(12, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<StyleSet, qt_meta_tag_ZN3aqt11stylesheets8StyleSetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject aqt::stylesheets::StyleSet::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets8StyleSetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets8StyleSetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3aqt11stylesheets8StyleSetE_t>.metaTypes,
    nullptr
} };

void aqt::stylesheets::StyleSet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<StyleSet *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->propsChanged(); break;
        case 1: _t->nameChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->pathChanged(); break;
        case 3: _t->onParentChanged((*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (StyleSet::*)()>(_a, &StyleSet::propsChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleSet::*)(const QString & )>(_a, &StyleSet::nameChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleSet::*)()>(_a, &StyleSet::pathChanged, 2))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< aqt::stylesheets::StyleSetProps* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->name(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->pathString(); break;
        case 2: *reinterpret_cast<aqt::stylesheets::StyleSetProps**>(_v) = _t->props(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->styleInfo(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setName(*reinterpret_cast<QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *aqt::stylesheets::StyleSet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *aqt::stylesheets::StyleSet::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets8StyleSetE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int aqt::stylesheets::StyleSet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void aqt::stylesheets::StyleSet::propsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void aqt::stylesheets::StyleSet::nameChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void aqt::stylesheets::StyleSet::pathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
