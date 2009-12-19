/****************************************************************************
** Meta object code from reading C++ file 'SongRecord.h'
**
** Created: Sat Dec 19 18:15:32 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../songdb/SongRecord.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SongRecord.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SongRecord[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       8,   19, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   12,   11,   11, 0x05,

 // properties: name, type, flags
      71,   67, 0x02095103,
      86,   78, 0x0a095103,
      92,   78, 0x0a095103,
      97,   67, 0x02095103,
     104,   78, 0x0a095103,
     109,   78, 0x0a095103,
     116,   78, 0x0a095103,
     126,   78, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_SongRecord[] = {
    "SongRecord\0\0,field,value\0"
    "songChanged(SongRecord*,QString,QVariant)\0"
    "int\0songId\0QString\0title\0tags\0number\0"
    "text\0author\0copyright\0lastUsed\0"
};

const QMetaObject SongRecord::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SongRecord,
      qt_meta_data_SongRecord, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SongRecord::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SongRecord::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SongRecord::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SongRecord))
        return static_cast<void*>(const_cast< SongRecord*>(this));
    return QObject::qt_metacast(_clname);
}

int SongRecord::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: songChanged((*reinterpret_cast< SongRecord*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = songId(); break;
        case 1: *reinterpret_cast< QString*>(_v) = title(); break;
        case 2: *reinterpret_cast< QString*>(_v) = tags(); break;
        case 3: *reinterpret_cast< int*>(_v) = number(); break;
        case 4: *reinterpret_cast< QString*>(_v) = text(); break;
        case 5: *reinterpret_cast< QString*>(_v) = author(); break;
        case 6: *reinterpret_cast< QString*>(_v) = copyright(); break;
        case 7: *reinterpret_cast< QString*>(_v) = lastUsed(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSongId(*reinterpret_cast< int*>(_v)); break;
        case 1: setTitle(*reinterpret_cast< QString*>(_v)); break;
        case 2: setTags(*reinterpret_cast< QString*>(_v)); break;
        case 3: setNumber(*reinterpret_cast< int*>(_v)); break;
        case 4: setText(*reinterpret_cast< QString*>(_v)); break;
        case 5: setAuthor(*reinterpret_cast< QString*>(_v)); break;
        case 6: setCopyright(*reinterpret_cast< QString*>(_v)); break;
        case 7: setLastUsed(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void SongRecord::songChanged(SongRecord * _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
