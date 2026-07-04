/****************************************************************************
** Meta object code from reading C++ file 'StyleEngine.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/vendor/aqt-stylesheets/src/StyleEngine.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StyleEngine.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN3aqt11stylesheets11StyleEngineE_t {};
} // unnamed namespace

template <> constexpr inline auto aqt::stylesheets::StyleEngine::qt_create_metaobjectdata<qt_meta_tag_ZN3aqt11stylesheets11StyleEngineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "aqt::stylesheets::StyleEngine",
        "styleChanged",
        "",
        "exception",
        "type",
        "message",
        "propertiesPotentiallyMissing"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'styleChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'exception'
        QtMocHelpers::RevisionedSignalData<void(const QString &, const QString &)>(3, 2, QMC::AccessPublic, 0xff01, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'propertiesPotentiallyMissing'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<StyleEngine, qt_meta_tag_ZN3aqt11stylesheets11StyleEngineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject aqt::stylesheets::StyleEngine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets11StyleEngineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets11StyleEngineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3aqt11stylesheets11StyleEngineE_t>.metaTypes,
    nullptr
} };

void aqt::stylesheets::StyleEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<StyleEngine *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->styleChanged(); break;
        case 1: _t->exception((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->propertiesPotentiallyMissing(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (StyleEngine::*)()>(_a, &StyleEngine::styleChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngine::*)(const QString & , const QString & )>(_a, &StyleEngine::exception, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngine::*)()>(_a, &StyleEngine::propertiesPotentiallyMissing, 2))
            return;
    }
}

const QMetaObject *aqt::stylesheets::StyleEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *aqt::stylesheets::StyleEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets11StyleEngineE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int aqt::stylesheets::StyleEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void aqt::stylesheets::StyleEngine::styleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void aqt::stylesheets::StyleEngine::exception(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void aqt::stylesheets::StyleEngine::propertiesPotentiallyMissing()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
