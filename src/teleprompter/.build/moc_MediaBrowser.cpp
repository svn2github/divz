/****************************************************************************
** Meta object code from reading C++ file 'MediaBrowser.h'
**
** Created: Sat Dec 19 21:08:46 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MediaBrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MediaBrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MediaBrowser[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      38,   13,   13,   13, 0x05,
      67,   13,   13,   13, 0x05,
     118,  100,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     152,   13,   13,   13, 0x0a,
     171,   13,   13,   13, 0x0a,
     199,  185,   13,   13, 0x0a,
     226,   13,   13,   13, 0x2a,
     248,   13,   13,   13, 0x0a,
     257,   13,   13,   13, 0x0a,
     269,   13,   13,   13, 0x0a,
     276,   13,   13,   13, 0x0a,
     311,   13,   13,   13, 0x09,
     343,   13,   13,   13, 0x09,
     375,   13,   13,   13, 0x09,
     398,   13,   13,   13, 0x09,
     420,   13,   13,   13, 0x09,
     442,   13,   13,   13, 0x09,
     463,   13,   13,   13, 0x09,
     483,   13,   13,   13, 0x09,
     504,   13,   13,   13, 0x09,
     523,   13,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_MediaBrowser[] = {
    "MediaBrowser\0\0fileSelected(QFileInfo)\0"
    "fileDoubleClicked(QFileInfo)\0"
    "setSelectedBackground(QFileInfo)\0"
    ",waitForNextSlide\0setLiveBackground(QFileInfo,bool)\0"
    "setFilter(QString)\0clearFilter()\0"
    ",addToHistory\0setDirectory(QString,bool)\0"
    "setDirectory(QString)\0goBack()\0"
    "goForward()\0goUp()\0"
    "setFileTypeFilterList(QStringList)\0"
    "indexDoubleClicked(QModelIndex)\0"
    "indexSingleClicked(QModelIndex)\0"
    "filterChanged(QString)\0filterReturnPressed()\0"
    "dirBoxReturnPressed()\0fileTypeChanged(int)\0"
    "slotAddToSchedule()\0slotSetAsBgCurrent()\0"
    "slotSetAsBgLater()\0slotSetAsBgLive()\0"
};

const QMetaObject MediaBrowser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MediaBrowser,
      qt_meta_data_MediaBrowser, 0 }
};

const QMetaObject *MediaBrowser::metaObject() const
{
    return &staticMetaObject;
}

void *MediaBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MediaBrowser))
        return static_cast<void*>(const_cast< MediaBrowser*>(this));
    return QWidget::qt_metacast(_clname);
}

int MediaBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileSelected((*reinterpret_cast< const QFileInfo(*)>(_a[1]))); break;
        case 1: fileDoubleClicked((*reinterpret_cast< const QFileInfo(*)>(_a[1]))); break;
        case 2: setSelectedBackground((*reinterpret_cast< const QFileInfo(*)>(_a[1]))); break;
        case 3: setLiveBackground((*reinterpret_cast< const QFileInfo(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: setFilter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: clearFilter(); break;
        case 6: setDirectory((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: setDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: goBack(); break;
        case 9: goForward(); break;
        case 10: goUp(); break;
        case 11: setFileTypeFilterList((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 12: indexDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 13: indexSingleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 14: filterChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: filterReturnPressed(); break;
        case 16: dirBoxReturnPressed(); break;
        case 17: fileTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: slotAddToSchedule(); break;
        case 19: slotSetAsBgCurrent(); break;
        case 20: slotSetAsBgLater(); break;
        case 21: slotSetAsBgLive(); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void MediaBrowser::fileSelected(const QFileInfo & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MediaBrowser::fileDoubleClicked(const QFileInfo & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MediaBrowser::setSelectedBackground(const QFileInfo & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MediaBrowser::setLiveBackground(const QFileInfo & _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
