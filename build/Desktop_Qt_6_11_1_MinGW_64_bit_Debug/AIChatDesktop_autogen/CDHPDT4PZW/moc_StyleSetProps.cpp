/****************************************************************************
** Meta object code from reading C++ file 'StyleSetProps.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/core/vendor/aqt-stylesheets/src/StyleSetProps.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StyleSetProps.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN3aqt11stylesheets13StyleSetPropsE_t {};
} // unnamed namespace

template <> constexpr inline auto aqt::stylesheets::StyleSetProps::qt_create_metaobjectdata<qt_meta_tag_ZN3aqt11stylesheets13StyleSetPropsE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "aqt::stylesheets::StyleSetProps",
        "propsChanged",
        "",
        "isValid",
        "isSet",
        "key",
        "get",
        "QVariant",
        "values",
        "color",
        "QColor",
        "boolean",
        "number",
        "font",
        "QFont",
        "string",
        "url",
        "QUrl"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'propsChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'isValid'
        QtMocHelpers::MethodData<bool() const>(3, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'isSet'
        QtMocHelpers::MethodData<bool(const QString &) const>(4, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'get'
        QtMocHelpers::MethodData<QVariant(const QString &) const>(6, 2, QMC::AccessPublic, 0x80000000 | 7, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'values'
        QtMocHelpers::RevisionedMethodData<QVariant(const QString &) const>(8, 2, QMC::AccessPublic, 0xff02, 0x80000000 | 7, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'color'
        QtMocHelpers::MethodData<QColor(const QString &) const>(9, 2, QMC::AccessPublic, 0x80000000 | 10, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'boolean'
        QtMocHelpers::MethodData<bool(const QString &) const>(11, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'number'
        QtMocHelpers::MethodData<double(const QString &) const>(12, 2, QMC::AccessPublic, QMetaType::Double, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'font'
        QtMocHelpers::MethodData<QFont(const QString &) const>(13, 2, QMC::AccessPublic, 0x80000000 | 14, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'string'
        QtMocHelpers::RevisionedMethodData<QString(const QString &) const>(15, 2, QMC::AccessPublic, 0xff02, QMetaType::QString, {{
            { QMetaType::QString, 5 },
        }}),
        // Method 'url'
        QtMocHelpers::RevisionedMethodData<QUrl(const QString &) const>(16, 2, QMC::AccessPublic, 0xff02, 0x80000000 | 17, {{
            { QMetaType::QString, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<StyleSetProps, qt_meta_tag_ZN3aqt11stylesheets13StyleSetPropsE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject aqt::stylesheets::StyleSetProps::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets13StyleSetPropsE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets13StyleSetPropsE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3aqt11stylesheets13StyleSetPropsE_t>.metaTypes,
    nullptr
} };

void aqt::stylesheets::StyleSetProps::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<StyleSetProps *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->propsChanged(); break;
        case 1: { bool _r = _t->isValid();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->isSet((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { QVariant _r = _t->get((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariant*>(_a[0]) = std::move(_r); }  break;
        case 4: { QVariant _r = _t->values((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariant*>(_a[0]) = std::move(_r); }  break;
        case 5: { QColor _r = _t->color((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QColor*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->boolean((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 7: { double _r = _t->number((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<double*>(_a[0]) = std::move(_r); }  break;
        case 8: { QFont _r = _t->font((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QFont*>(_a[0]) = std::move(_r); }  break;
        case 9: { QString _r = _t->string((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 10: { QUrl _r = _t->url((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QUrl*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (StyleSetProps::*)()>(_a, &StyleSetProps::propsChanged, 0))
            return;
    }
}

const QMetaObject *aqt::stylesheets::StyleSetProps::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *aqt::stylesheets::StyleSetProps::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets13StyleSetPropsE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int aqt::stylesheets::StyleSetProps::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void aqt::stylesheets::StyleSetProps::propsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
