/****************************************************************************
** Meta object code from reading C++ file 'SongBrowser.h'
**
** Created: Sat Dec 19 18:15:32 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../songdb/SongBrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SongBrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SongBrowser[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       3,  104, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   39,   12,   12, 0x0a,
      65,   12,   12,   12, 0x2a,
      77,   12,   12,   12, 0x0a,
      91,   12,   12,   12, 0x0a,
     115,   12,   12,   12, 0x0a,
     141,   12,   12,   12, 0x0a,
     165,   12,   12,   12, 0x09,
     196,   12,   12,   12, 0x09,
     227,   12,   12,   12, 0x09,
     254,   12,   12,   12, 0x09,
     272,   12,   12,   12, 0x09,
     298,   12,   12,   12, 0x09,
     311,   12,   12,   12, 0x09,
     336,   12,   12,   12, 0x09,
     355,   12,   12,   12, 0x09,
     370,   12,   12,   12, 0x09,
     390,   12,   12,   12, 0x09,

 // properties: name, type, flags
     410,  405, 0x01095103,
     425,  405, 0x01095103,
     442,  405, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_SongBrowser[] = {
    "SongBrowser\0\0songSelected(SongRecord*)\0"
    "filter\0setFilter(QString)\0setFilter()\0"
    "clearFilter()\0setEditingEnabled(bool)\0"
    "setFilteringEnabled(bool)\0"
    "setPreviewEnabled(bool)\0"
    "songDoubleClicked(QModelIndex)\0"
    "songSingleClicked(QModelIndex)\0"
    "songFilterChanged(QString)\0songFilterReset()\0"
    "songSearchReturnPressed()\0addNewSong()\0"
    "songCreated(SongRecord*)\0editSongAccepted()\0"
    "editSongInDB()\0deleteCurrentSong()\0"
    "searchOnline()\0bool\0editingEnabled\0"
    "filteringEnabled\0previewEnabled\0"
};

const QMetaObject SongBrowser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SongBrowser,
      qt_meta_data_SongBrowser, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SongBrowser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SongBrowser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SongBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SongBrowser))
        return static_cast<void*>(const_cast< SongBrowser*>(this));
    return QWidget::qt_metacast(_clname);
}

int SongBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: songSelected((*reinterpret_cast< SongRecord*(*)>(_a[1]))); break;
        case 1: setFilter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: setFilter(); break;
        case 3: clearFilter(); break;
        case 4: setEditingEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: setFilteringEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: setPreviewEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: songDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: songSingleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 9: songFilterChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: songFilterReset(); break;
        case 11: songSearchReturnPressed(); break;
        case 12: addNewSong(); break;
        case 13: songCreated((*reinterpret_cast< SongRecord*(*)>(_a[1]))); break;
        case 14: editSongAccepted(); break;
        case 15: editSongInDB(); break;
        case 16: deleteCurrentSong(); break;
        case 17: searchOnline(); break;
        default: ;
        }
        _id -= 18;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = editingEnabled(); break;
        case 1: *reinterpret_cast< bool*>(_v) = filteringEnabled(); break;
        case 2: *reinterpret_cast< bool*>(_v) = previewEnabled(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setEditingEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 1: setFilteringEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 2: setPreviewEnabled(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void SongBrowser::songSelected(SongRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
