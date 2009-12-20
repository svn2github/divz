/****************************************************************************
** Meta object code from reading C++ file 'PPTSlideGroup.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ppt/PPTSlideGroup.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PPTSlideGroup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PPTSlideGroup[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       1,   17, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x09,

 // properties: name, type, flags
      52,   44, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_PPTSlideGroup[] = {
    "PPTSlideGroup\0\0x\0aspectRatioChanged(double)\0"
    "QString\0file\0"
};

const QMetaObject PPTSlideGroup::staticMetaObject = {
    { &SlideGroup::staticMetaObject, qt_meta_stringdata_PPTSlideGroup,
      qt_meta_data_PPTSlideGroup, 0 }
};

const QMetaObject *PPTSlideGroup::metaObject() const
{
    return &staticMetaObject;
}

void *PPTSlideGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PPTSlideGroup))
        return static_cast<void*>(const_cast< PPTSlideGroup*>(this));
    return SlideGroup::qt_metacast(_clname);
}

int PPTSlideGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SlideGroup::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: aspectRatioChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = file(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setFile(*reinterpret_cast< QString*>(_v)); break;
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
