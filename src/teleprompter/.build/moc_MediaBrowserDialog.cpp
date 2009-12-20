/****************************************************************************
** Meta object code from reading C++ file 'MediaBrowserDialog.h'
**
** Created: Sat Dec 19 21:08:46 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MediaBrowserDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MediaBrowserDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MediaBrowserDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   19,   19,   19, 0x08,
      73,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MediaBrowserDialog[] = {
    "MediaBrowserDialog\0\0fileDoubleClicked(QFileInfo)\0"
    "fileSelected(QFileInfo)\0"
    "slotFileDoubleClicked(QFileInfo)\0"
};

const QMetaObject MediaBrowserDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MediaBrowserDialog,
      qt_meta_data_MediaBrowserDialog, 0 }
};

const QMetaObject *MediaBrowserDialog::metaObject() const
{
    return &staticMetaObject;
}

void *MediaBrowserDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MediaBrowserDialog))
        return static_cast<void*>(const_cast< MediaBrowserDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MediaBrowserDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileDoubleClicked((*reinterpret_cast< const QFileInfo(*)>(_a[1]))); break;
        case 1: fileSelected((*reinterpret_cast< const QFileInfo(*)>(_a[1]))); break;
        case 2: slotFileDoubleClicked((*reinterpret_cast< const QFileInfo(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void MediaBrowserDialog::fileDoubleClicked(const QFileInfo & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
