/****************************************************************************
** Meta object code from reading C++ file 'TextBoxConfig.h'
**
** Created: Sat Dec 19 21:08:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/TextBoxConfig.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextBoxConfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextBoxConfig[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      32,   15,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TextBoxConfig[] = {
    "TextBoxConfig\0\0fieldName,value,\0"
    "itemChanged(QString,QVariant,QVariant)\0"
};

const QMetaObject TextBoxConfig::staticMetaObject = {
    { &GenericItemConfig::staticMetaObject, qt_meta_stringdata_TextBoxConfig,
      qt_meta_data_TextBoxConfig, 0 }
};

const QMetaObject *TextBoxConfig::metaObject() const
{
    return &staticMetaObject;
}

void *TextBoxConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextBoxConfig))
        return static_cast<void*>(const_cast< TextBoxConfig*>(this));
    return GenericItemConfig::qt_metacast(_clname);
}

int TextBoxConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GenericItemConfig::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
