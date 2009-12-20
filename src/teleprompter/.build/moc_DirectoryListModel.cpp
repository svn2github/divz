/****************************************************************************
** Meta object code from reading C++ file 'DirectoryListModel.h'
**
** Created: Sat Dec 19 21:08:46 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DirectoryListModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DirectoryListModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DirectoryListModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DirectoryListModel[] = {
    "DirectoryListModel\0\0makePixmaps()\0"
};

const QMetaObject DirectoryListModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_DirectoryListModel,
      qt_meta_data_DirectoryListModel, 0 }
};

const QMetaObject *DirectoryListModel::metaObject() const
{
    return &staticMetaObject;
}

void *DirectoryListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DirectoryListModel))
        return static_cast<void*>(const_cast< DirectoryListModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int DirectoryListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: makePixmaps(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
