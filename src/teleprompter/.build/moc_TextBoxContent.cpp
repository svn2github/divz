/****************************************************************************
** Meta object code from reading C++ file 'TextBoxContent.h'
**
** Created: Sat Dec 19 21:08:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/TextBoxContent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextBoxContent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextBoxWarmingThread[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_TextBoxWarmingThread[] = {
    "TextBoxWarmingThread\0\0renderDone(QImage*)\0"
};

const QMetaObject TextBoxWarmingThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_TextBoxWarmingThread,
      qt_meta_data_TextBoxWarmingThread, 0 }
};

const QMetaObject *TextBoxWarmingThread::metaObject() const
{
    return &staticMetaObject;
}

void *TextBoxWarmingThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextBoxWarmingThread))
        return static_cast<void*>(const_cast< TextBoxWarmingThread*>(this));
    return QThread::qt_metacast(_clname);
}

int TextBoxWarmingThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: renderDone((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void TextBoxWarmingThread::renderDone(QImage * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_TextBoxWarmingThreadManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      29,   28,   28,   28, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TextBoxWarmingThreadManager[] = {
    "TextBoxWarmingThreadManager\0\0"
    "renderDone(QImage*)\0"
};

const QMetaObject TextBoxWarmingThreadManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TextBoxWarmingThreadManager,
      qt_meta_data_TextBoxWarmingThreadManager, 0 }
};

const QMetaObject *TextBoxWarmingThreadManager::metaObject() const
{
    return &staticMetaObject;
}

void *TextBoxWarmingThreadManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextBoxWarmingThreadManager))
        return static_cast<void*>(const_cast< TextBoxWarmingThreadManager*>(this));
    return QObject::qt_metacast(_clname);
}

int TextBoxWarmingThreadManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: renderDone((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_TextBoxContent[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       1,   52, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      37,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      70,   15,   62,   15, 0x0a,
      88,   79,   15,   15, 0x0a,
     105,   15,   15,   15, 0x0a,
     142,   15,   15,   15, 0x08,
     160,   15,   15,   15, 0x08,
     183,   15,   15,   15, 0x08,

 // properties: name, type, flags
     197,   62, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_TextBoxContent[] = {
    "TextBoxContent\0\0notifyHasShape(bool)\0"
    "notifyShapeEditing(bool)\0QString\0"
    "toHtml()\0htmlCode\0setHtml(QString)\0"
    "warmVisualCache(AbstractVisualItem*)\0"
    "contentsResized()\0delayContentsResized()\0"
    "animateZoom()\0html\0"
};

const QMetaObject TextBoxContent::staticMetaObject = {
    { &AbstractContent::staticMetaObject, qt_meta_stringdata_TextBoxContent,
      qt_meta_data_TextBoxContent, 0 }
};

const QMetaObject *TextBoxContent::metaObject() const
{
    return &staticMetaObject;
}

void *TextBoxContent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextBoxContent))
        return static_cast<void*>(const_cast< TextBoxContent*>(this));
    return AbstractContent::qt_metacast(_clname);
}

int TextBoxContent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractContent::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: notifyHasShape((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: notifyShapeEditing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: { QString _r = toHtml();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 3: setHtml((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: warmVisualCache((*reinterpret_cast< AbstractVisualItem*(*)>(_a[1]))); break;
        case 5: contentsResized(); break;
        case 6: delayContentsResized(); break;
        case 7: animateZoom(); break;
        default: ;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = toHtml(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setHtml(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void TextBoxContent::notifyHasShape(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TextBoxContent::notifyShapeEditing(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
