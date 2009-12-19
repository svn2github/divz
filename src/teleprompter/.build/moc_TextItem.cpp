/****************************************************************************
** Meta object code from reading C++ file 'TextItem.h'
**
** Created: Sat Dec 19 18:15:32 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../model/TextItem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextItem[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      10,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      17,    9, 0x0a095103,
      22,    9, 0x0a095103,
      40,   33, 0x06095103,
      54,   49, 0x01095103,
      75,   67, 0x1a095103,
      87,   67, 0x1a095103,
      99,   67, 0x1a095103,
     111,   67, 0x1a095103,
     137,  123, 0x0009510b,
     148,  123, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_TextItem[] = {
    "TextItem\0QString\0text\0fontFamily\0"
    "double\0fontSize\0bool\0shapeEnabled\0"
    "QPointF\0shapePoint1\0shapePoint2\0"
    "shapePoint3\0shapePoint4\0Qt::Alignment\0"
    "xTextAlign\0yTextAlign\0"
};

const QMetaObject TextItem::staticMetaObject = {
    { &AbstractVisualItem::staticMetaObject, qt_meta_stringdata_TextItem,
      qt_meta_data_TextItem, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextItem))
        return static_cast<void*>(const_cast< TextItem*>(this));
    return AbstractVisualItem::qt_metacast(_clname);
}

int TextItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractVisualItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        case 1: *reinterpret_cast< QString*>(_v) = fontFamily(); break;
        case 2: *reinterpret_cast< double*>(_v) = fontSize(); break;
        case 3: *reinterpret_cast< bool*>(_v) = shapeEnabled(); break;
        case 4: *reinterpret_cast< QPointF*>(_v) = shapePoint1(); break;
        case 5: *reinterpret_cast< QPointF*>(_v) = shapePoint2(); break;
        case 6: *reinterpret_cast< QPointF*>(_v) = shapePoint3(); break;
        case 7: *reinterpret_cast< QPointF*>(_v) = shapePoint4(); break;
        case 8: *reinterpret_cast< Qt::Alignment*>(_v) = xTextAlign(); break;
        case 9: *reinterpret_cast< Qt::Alignment*>(_v) = yTextAlign(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setText(*reinterpret_cast< QString*>(_v)); break;
        case 1: setFontFamily(*reinterpret_cast< QString*>(_v)); break;
        case 2: setFontSize(*reinterpret_cast< double*>(_v)); break;
        case 3: setShapeEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 4: setShapePoint1(*reinterpret_cast< QPointF*>(_v)); break;
        case 5: setShapePoint2(*reinterpret_cast< QPointF*>(_v)); break;
        case 6: setShapePoint3(*reinterpret_cast< QPointF*>(_v)); break;
        case 7: setShapePoint4(*reinterpret_cast< QPointF*>(_v)); break;
        case 8: setXTextAlign(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 9: setYTextAlign(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
