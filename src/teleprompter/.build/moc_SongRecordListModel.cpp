/****************************************************************************
** Meta object code from reading C++ file 'SongRecordListModel.h'
**
** Created: Sat Dec 19 18:15:32 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../songdb/SongRecordListModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SongRecordListModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SongRecordListModel[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x09,
      42,   20,   20,   20, 0x09,
      66,   20,   20,   20, 0x08,
      99,   86,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SongRecordListModel[] = {
    "SongRecordListModel\0\0addSong(SongRecord*)\0"
    "removeSong(SongRecord*)\0modelDirtyTimeout()\0"
    ",field,value\0songChanged(SongRecord*,QString,QVariant)\0"
};

const QMetaObject SongRecordListModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_SongRecordListModel,
      qt_meta_data_SongRecordListModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SongRecordListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SongRecordListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SongRecordListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SongRecordListModel))
        return static_cast<void*>(const_cast< SongRecordListModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int SongRecordListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addSong((*reinterpret_cast< SongRecord*(*)>(_a[1]))); break;
        case 1: removeSong((*reinterpret_cast< SongRecord*(*)>(_a[1]))); break;
        case 2: modelDirtyTimeout(); break;
        case 3: songChanged((*reinterpret_cast< SongRecord*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
