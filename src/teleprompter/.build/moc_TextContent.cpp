/****************************************************************************
** Meta object code from reading C++ file 'TextContent.h'
**
** Created: Sat Dec 19 21:08:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/TextContent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextContent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextContent[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   12, // methods
       4,   62, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      34,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      67,   12,   59,   12, 0x0a,
      85,   76,   12,   12, 0x0a,
     107,   12,  102,   12, 0x0a,
     118,   12,   12,   12, 0x0a,
     131,   12,  102,   12, 0x0a,
     156,  148,   12,   12, 0x0a,
     191,   12,  178,   12, 0x0a,
     208,  203,   12,   12, 0x0a,

 // properties: name, type, flags
     235,   59, 0x0a095103,
     240,  102, 0x01495001,
     249,  102, 0x01495103,
     262,  178, 0x0009510b,

 // properties: notify_signal_id
       0,
       0,
       1,
       0,

       0        // eod
};

static const char qt_meta_stringdata_TextContent[] = {
    "TextContent\0\0notifyHasShape(bool)\0"
    "notifyShapeEditing(bool)\0QString\0"
    "toHtml()\0htmlCode\0setHtml(QString)\0"
    "bool\0hasShape()\0clearShape()\0"
    "isShapeEditing()\0enabled\0setShapeEditing(bool)\0"
    "QPainterPath\0shapePath()\0path\0"
    "setShapePath(QPainterPath)\0html\0"
    "hasShape\0shapeEditing\0shapePath\0"
};

const QMetaObject TextContent::staticMetaObject = {
    { &AbstractContent::staticMetaObject, qt_meta_stringdata_TextContent,
      qt_meta_data_TextContent, 0 }
};

const QMetaObject *TextContent::metaObject() const
{
    return &staticMetaObject;
}

void *TextContent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextContent))
        return static_cast<void*>(const_cast< TextContent*>(this));
    return AbstractContent::qt_metacast(_clname);
}

int TextContent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 4: { bool _r = hasShape();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: clearShape(); break;
        case 6: { bool _r = isShapeEditing();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: setShapeEditing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: { QPainterPath _r = shapePath();
            if (_a[0]) *reinterpret_cast< QPainterPath*>(_a[0]) = _r; }  break;
        case 9: setShapePath((*reinterpret_cast< const QPainterPath(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = toHtml(); break;
        case 1: *reinterpret_cast< bool*>(_v) = hasShape(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isShapeEditing(); break;
        case 3: *reinterpret_cast< QPainterPath*>(_v) = shapePath(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setHtml(*reinterpret_cast< QString*>(_v)); break;
        case 2: setShapeEditing(*reinterpret_cast< bool*>(_v)); break;
        case 3: setShapePath(*reinterpret_cast< QPainterPath*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void TextContent::notifyHasShape(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TextContent::notifyShapeEditing(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
