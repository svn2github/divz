/****************************************************************************
** Meta object code from reading C++ file 'OutputViewItem.h'
**
** Created: Sat Dec 19 18:15:32 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../model/OutputViewItem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OutputViewItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OutputViewItem[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      19,   15, 0x02095103,
      28,   15, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_OutputViewItem[] = {
    "OutputViewItem\0int\0outputId\0outputPort\0"
};

const QMetaObject OutputViewItem::staticMetaObject = {
    { &AbstractVisualItem::staticMetaObject, qt_meta_stringdata_OutputViewItem,
      qt_meta_data_OutputViewItem, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OutputViewItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OutputViewItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OutputViewItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputViewItem))
        return static_cast<void*>(const_cast< OutputViewItem*>(this));
    return AbstractVisualItem::qt_metacast(_clname);
}

int OutputViewItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractVisualItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = outputId(); break;
        case 1: *reinterpret_cast< int*>(_v) = outputPort(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOutputId(*reinterpret_cast< int*>(_v)); break;
        case 1: setOutputPort(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
