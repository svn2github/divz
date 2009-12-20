/****************************************************************************
** Meta object code from reading C++ file 'BoxContent.h'
**
** Created: Sat Dec 19 21:08:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/BoxContent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BoxContent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BoxContent[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      19,   11, 0x1a095103,

       0        // eod
};

static const char qt_meta_stringdata_BoxContent[] = {
    "BoxContent\0QPointF\0cornerRounding\0"
};

const QMetaObject BoxContent::staticMetaObject = {
    { &AbstractContent::staticMetaObject, qt_meta_stringdata_BoxContent,
      qt_meta_data_BoxContent, 0 }
};

const QMetaObject *BoxContent::metaObject() const
{
    return &staticMetaObject;
}

void *BoxContent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BoxContent))
        return static_cast<void*>(const_cast< BoxContent*>(this));
    return AbstractContent::qt_metacast(_clname);
}

int BoxContent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractContent::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QPointF*>(_v) = cornerRounding(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCornerRounding(*reinterpret_cast< QPointF*>(_v)); break;
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
QT_END_MOC_NAMESPACE
