/****************************************************************************
** Meta object code from reading C++ file 'MirrorItem.h'
**
** Created: Sat Dec 19 21:08:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/MirrorItem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MirrorItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MirrorItem[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      26,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MirrorItem[] = {
    "MirrorItem\0\0sourceMoved()\0sourceChanged()\0"
};

const QMetaObject MirrorItem::staticMetaObject = {
    { &AbstractDisposeable::staticMetaObject, qt_meta_stringdata_MirrorItem,
      qt_meta_data_MirrorItem, 0 }
};

const QMetaObject *MirrorItem::metaObject() const
{
    return &staticMetaObject;
}

void *MirrorItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MirrorItem))
        return static_cast<void*>(const_cast< MirrorItem*>(this));
    return AbstractDisposeable::qt_metacast(_clname);
}

int MirrorItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractDisposeable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sourceMoved(); break;
        case 1: sourceChanged(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
