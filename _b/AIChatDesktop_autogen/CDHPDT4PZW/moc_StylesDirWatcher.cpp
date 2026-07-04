/****************************************************************************
** Meta object code from reading C++ file 'StylesDirWatcher.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/vendor/aqt-stylesheets/src/StylesDirWatcher.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StylesDirWatcher.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN3aqt11stylesheets16StylesDirWatcherE_t {};
} // unnamed namespace

template <> constexpr inline auto aqt::stylesheets::StylesDirWatcher::qt_create_metaobjectdata<qt_meta_tag_ZN3aqt11stylesheets16StylesDirWatcherE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "aqt::stylesheets::StylesDirWatcher",
        "availableStylesChanged",
        "",
        "fileExtensionsChanged",
        "stylePathChanged",
        "QUrl",
        "url",
        "onDirectoryChanged",
        "path",
        "availableStyleSheetNames",
        "QVariantList",
        "stylePath",
        "fileExtensions",
        "availableStyles"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'availableStylesChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'fileExtensionsChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stylePathChanged'
        QtMocHelpers::SignalData<void(const QUrl &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Slot 'onDirectoryChanged'
        QtMocHelpers::SlotData<void(const QString &)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Method 'availableStyleSheetNames'
        QtMocHelpers::MethodData<QVariantList() const>(9, 2, QMC::AccessPublic, 0x80000000 | 10),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'stylePath'
        QtMocHelpers::PropertyData<QUrl>(11, 0x80000000 | 5, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 2),
        // property 'fileExtensions'
        QtMocHelpers::PropertyData<QVariantList>(12, 0x80000000 | 10, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 1),
        // property 'availableStyles'
        QtMocHelpers::PropertyData<QVariantList>(13, 0x80000000 | 10, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<StylesDirWatcher, qt_meta_tag_ZN3aqt11stylesheets16StylesDirWatcherE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject aqt::stylesheets::StylesDirWatcher::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets16StylesDirWatcherE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets16StylesDirWatcherE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3aqt11stylesheets16StylesDirWatcherE_t>.metaTypes,
    nullptr
} };

void aqt::stylesheets::StylesDirWatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<StylesDirWatcher *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->availableStylesChanged(); break;
        case 1: _t->fileExtensionsChanged(); break;
        case 2: _t->stylePathChanged((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 3: _t->onDirectoryChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: { QVariantList _r = _t->availableStyleSheetNames();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (StylesDirWatcher::*)()>(_a, &StylesDirWatcher::availableStylesChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (StylesDirWatcher::*)()>(_a, &StylesDirWatcher::fileExtensionsChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (StylesDirWatcher::*)(const QUrl & )>(_a, &StylesDirWatcher::stylePathChanged, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QUrl*>(_v) = _t->stylePath(); break;
        case 1: *reinterpret_cast<QVariantList*>(_v) = _t->fileExtensions(); break;
        case 2: *reinterpret_cast<QVariantList*>(_v) = _t->availableStyles(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setStylePath(*reinterpret_cast<QUrl*>(_v)); break;
        case 1: _t->setFileExtensions(*reinterpret_cast<QVariantList*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *aqt::stylesheets::StylesDirWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *aqt::stylesheets::StylesDirWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets16StylesDirWatcherE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int aqt::stylesheets::StylesDirWatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void aqt::stylesheets::StylesDirWatcher::availableStylesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void aqt::stylesheets::StylesDirWatcher::fileExtensionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void aqt::stylesheets::StylesDirWatcher::stylePathChanged(const QUrl & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
