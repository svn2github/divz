/****************************************************************************
** Meta object code from reading C++ file 'SlideSettingsDialog.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SlideSettingsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SlideSettingsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SlideSettingsDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x09,
      47,   20,   20,   20, 0x09,
      59,   20,   20,   20, 0x09,
      71,   20,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SlideSettingsDialog[] = {
    "SlideSettingsDialog\0\0setAutoChangeTime(double)\0"
    "slotGuess()\0slotNever()\0slotAccepted()\0"
};

const QMetaObject SlideSettingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SlideSettingsDialog,
      qt_meta_data_SlideSettingsDialog, 0 }
};

const QMetaObject *SlideSettingsDialog::metaObject() const
{
    return &staticMetaObject;
}

void *SlideSettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SlideSettingsDialog))
        return static_cast<void*>(const_cast< SlideSettingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int SlideSettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setAutoChangeTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: slotGuess(); break;
        case 2: slotNever(); break;
        case 3: slotAccepted(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
