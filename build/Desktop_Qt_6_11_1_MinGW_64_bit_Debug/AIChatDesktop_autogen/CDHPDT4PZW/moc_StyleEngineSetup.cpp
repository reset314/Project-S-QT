/****************************************************************************
** Meta object code from reading C++ file 'StyleEngineSetup.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/core/vendor/aqt-stylesheets/src/StyleEngineSetup.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StyleEngineSetup.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN3aqt11stylesheets16StyleEngineSetupE_t {};
} // unnamed namespace

template <> constexpr inline auto aqt::stylesheets::StyleEngineSetup::qt_create_metaobjectdata<qt_meta_tag_ZN3aqt11stylesheets16StyleEngineSetupE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "aqt::stylesheets::StyleEngineSetup",
        "styleChanged",
        "",
        "styleNameChanged",
        "defaultStyleNameChanged",
        "fileExtensionsChanged",
        "availableStylesChanged",
        "styleSheetSourceChanged",
        "QUrl",
        "url",
        "defaultStyleSheetSourceChanged",
        "exception",
        "type",
        "message",
        "onFileChanged",
        "path",
        "stylePath",
        "styleName",
        "defaultStyleName",
        "fileExtensions",
        "QVariantList",
        "availableStyles",
        "styleSheetSource",
        "defaultStyleSheetSource"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'styleChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'styleNameChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'defaultStyleNameChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'fileExtensionsChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'availableStylesChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'styleSheetSourceChanged'
        QtMocHelpers::RevisionedSignalData<void(const QUrl &)>(7, 2, QMC::AccessPublic, 0xff01, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'defaultStyleSheetSourceChanged'
        QtMocHelpers::RevisionedSignalData<void(const QUrl &)>(10, 2, QMC::AccessPublic, 0xff01, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'exception'
        QtMocHelpers::RevisionedSignalData<void(const QString &, const QString &)>(11, 2, QMC::AccessPublic, 0xff01, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 13 },
        }}),
        // Slot 'onFileChanged'
        QtMocHelpers::SlotData<void(const QString &)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'stylePath'
        QtMocHelpers::PropertyData<QUrl>(16, 0x80000000 | 8, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'styleName'
        QtMocHelpers::PropertyData<QString>(17, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'defaultStyleName'
        QtMocHelpers::PropertyData<QString>(18, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'fileExtensions'
        QtMocHelpers::PropertyData<QVariantList>(19, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 3),
        // property 'availableStyles'
        QtMocHelpers::PropertyData<QVariantList>(21, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 4),
        // property 'styleSheetSource'
        QtMocHelpers::PropertyData<QUrl>(22, 0x80000000 | 8, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 5, 0xff01),
        // property 'defaultStyleSheetSource'
        QtMocHelpers::PropertyData<QUrl>(23, 0x80000000 | 8, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 6, 0xff01),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<StyleEngineSetup, qt_meta_tag_ZN3aqt11stylesheets16StyleEngineSetupE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject aqt::stylesheets::StyleEngineSetup::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets16StyleEngineSetupE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets16StyleEngineSetupE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3aqt11stylesheets16StyleEngineSetupE_t>.metaTypes,
    nullptr
} };

void aqt::stylesheets::StyleEngineSetup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<StyleEngineSetup *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->styleChanged(); break;
        case 1: _t->styleNameChanged(); break;
        case 2: _t->defaultStyleNameChanged(); break;
        case 3: _t->fileExtensionsChanged(); break;
        case 4: _t->availableStylesChanged(); break;
        case 5: _t->styleSheetSourceChanged((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 6: _t->defaultStyleSheetSourceChanged((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 7: _t->exception((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->onFileChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (StyleEngineSetup::*)()>(_a, &StyleEngineSetup::styleChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngineSetup::*)()>(_a, &StyleEngineSetup::styleNameChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngineSetup::*)()>(_a, &StyleEngineSetup::defaultStyleNameChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngineSetup::*)()>(_a, &StyleEngineSetup::fileExtensionsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngineSetup::*)()>(_a, &StyleEngineSetup::availableStylesChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngineSetup::*)(const QUrl & )>(_a, &StyleEngineSetup::styleSheetSourceChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngineSetup::*)(const QUrl & )>(_a, &StyleEngineSetup::defaultStyleSheetSourceChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (StyleEngineSetup::*)(const QString & , const QString & )>(_a, &StyleEngineSetup::exception, 7))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QUrl*>(_v) = _t->stylePath(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->styleName(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->defaultStyleName(); break;
        case 3: *reinterpret_cast<QVariantList*>(_v) = _t->fileExtensions(); break;
        case 4: *reinterpret_cast<QVariantList*>(_v) = _t->availableStyles(); break;
        case 5: *reinterpret_cast<QUrl*>(_v) = _t->styleSheetSource(); break;
        case 6: *reinterpret_cast<QUrl*>(_v) = _t->defaultStyleSheetSource(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setStylePath(*reinterpret_cast<QUrl*>(_v)); break;
        case 1: _t->setStyleName(*reinterpret_cast<QString*>(_v)); break;
        case 2: _t->setDefaultStyleName(*reinterpret_cast<QString*>(_v)); break;
        case 3: _t->setFileExtensions(*reinterpret_cast<QVariantList*>(_v)); break;
        case 5: _t->setStyleSheetSource(*reinterpret_cast<QUrl*>(_v)); break;
        case 6: _t->setDefaultStyleSheetSource(*reinterpret_cast<QUrl*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *aqt::stylesheets::StyleEngineSetup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *aqt::stylesheets::StyleEngineSetup::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3aqt11stylesheets16StyleEngineSetupE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QQmlParserStatus"))
        return static_cast< QQmlParserStatus*>(this);
    if (!strcmp(_clname, "org.qt-project.Qt.QQmlParserStatus"))
        return static_cast< QQmlParserStatus*>(this);
    return QObject::qt_metacast(_clname);
}

int aqt::stylesheets::StyleEngineSetup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void aqt::stylesheets::StyleEngineSetup::styleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void aqt::stylesheets::StyleEngineSetup::styleNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void aqt::stylesheets::StyleEngineSetup::defaultStyleNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void aqt::stylesheets::StyleEngineSetup::fileExtensionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void aqt::stylesheets::StyleEngineSetup::availableStylesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void aqt::stylesheets::StyleEngineSetup::styleSheetSourceChanged(const QUrl & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void aqt::stylesheets::StyleEngineSetup::defaultStyleSheetSourceChanged(const QUrl & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void aqt::stylesheets::StyleEngineSetup::exception(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}
QT_WARNING_POP
