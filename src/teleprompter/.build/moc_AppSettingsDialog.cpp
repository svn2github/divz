/****************************************************************************
** Meta object code from reading C++ file 'AppSettingsDialog.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../AppSettingsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppSettingsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AppSettingsDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x09,
      46,   18,   18,   18, 0x09,
      66,   18,   18,   18, 0x09,
      88,   18,   18,   18, 0x09,
     103,   18,   18,   18, 0x09,
     120,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AppSettingsDialog[] = {
    "AppSettingsDialog\0\0slotUseOpenGLChanged(bool)\0"
    "slotConfigOutputs()\0slotDiskCacheBrowse()\0"
    "slotAccepted()\0portChanged(int)\0"
    "linkActivated(QString)\0"
};

const QMetaObject AppSettingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AppSettingsDialog,
      qt_meta_data_AppSettingsDialog, 0 }
};

const QMetaObject *AppSettingsDialog::metaObject() const
{
    return &staticMetaObject;
}

void *AppSettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppSettingsDialog))
        return static_cast<void*>(const_cast< AppSettingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int AppSettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotUseOpenGLChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: slotConfigOutputs(); break;
        case 2: slotDiskCacheBrowse(); break;
        case 3: slotAccepted(); break;
        case 4: portChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: linkActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
