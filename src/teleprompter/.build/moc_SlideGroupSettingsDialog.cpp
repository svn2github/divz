/****************************************************************************
** Meta object code from reading C++ file 'SlideGroupSettingsDialog.h'
**
** Created: Sat Dec 19 18:15:34 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SlideGroupSettingsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SlideGroupSettingsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SlideGroupSettingsDialog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x09,
      48,   25,   25,   25, 0x09,
      70,   25,   25,   25, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SlideGroupSettingsDialog[] = {
    "SlideGroupSettingsDialog\0\0"
    "titleChanged(QString)\0autoChangeGroup(bool)\0"
    "slotAccepted()\0"
};

const QMetaObject SlideGroupSettingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SlideGroupSettingsDialog,
      qt_meta_data_SlideGroupSettingsDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SlideGroupSettingsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SlideGroupSettingsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SlideGroupSettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SlideGroupSettingsDialog))
        return static_cast<void*>(const_cast< SlideGroupSettingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int SlideGroupSettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: titleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: autoChangeGroup((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: slotAccepted(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
