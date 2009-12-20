/****************************************************************************
** Meta object code from reading C++ file 'StyledButtonItem.h'
**
** Created: Sat Dec 19 21:08:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/StyledButtonItem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StyledButtonItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_StyledButtonItem[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_StyledButtonItem[] = {
    "StyledButtonItem\0\0clicked()\0"
};

const QMetaObject StyledButtonItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_StyledButtonItem,
      qt_meta_data_StyledButtonItem, 0 }
};

const QMetaObject *StyledButtonItem::metaObject() const
{
    return &staticMetaObject;
}

void *StyledButtonItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_StyledButtonItem))
        return static_cast<void*>(const_cast< StyledButtonItem*>(this));
    if (!strcmp(_clname, "QGraphicsItem"))
        return static_cast< QGraphicsItem*>(const_cast< StyledButtonItem*>(this));
    return QObject::qt_metacast(_clname);
}

int StyledButtonItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clicked(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void StyledButtonItem::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
