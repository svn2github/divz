/****************************************************************************
** Meta object code from reading C++ file 'OutputSetupDialog.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OutputSetupDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OutputSetupDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OutputSetupDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x09,
      28,   18,   18,   18, 0x09,
      39,   37,   18,   18, 0x09,
      76,   37,   18,   18, 0x09,
     113,   18,   18,   18, 0x09,
     148,   18,   18,   18, 0x09,
     168,   18,   18,   18, 0x09,
     199,   18,   18,   18, 0x09,
     214,   18,   18,   18, 0x09,
     229,   18,   18,   18, 0x09,
     244,   18,   18,   18, 0x09,
     259,   18,   18,   18, 0x09,
     284,   18,   18,   18, 0x09,
     309,   18,   18,   18, 0x09,
     330,   18,   18,   18, 0x09,
     356,   18,   18,   18, 0x09,
     366,   18,   18,   18, 0x09,
     376,   18,   18,   18, 0x09,
     396,   18,   18,   18, 0x09,
     419,   18,   18,   18, 0x09,
     438,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_OutputSetupDialog[] = {
    "OutputSetupDialog\0\0accept()\0reject()\0"
    ",\0slotOutputListCellActivated(int,int)\0"
    "slotScreenListCellActivated(int,int)\0"
    "slotOutputEnabledStateChanged(int)\0"
    "slotTabChanged(int)\0slotOutputNameChanged(QString)\0"
    "slotCustX(int)\0slotCustY(int)\0"
    "slotCustW(int)\0slotCustH(int)\0"
    "slotNetRoleChanged(bool)\0"
    "slotHostChanged(QString)\0slotPortChanged(int)\0"
    "slotAllowMultChanged(int)\0slotNew()\0"
    "slotDel()\0slotStayOnTop(bool)\0"
    "slotMjpegEnabled(bool)\0slotMjpegPort(int)\0"
    "slotMjpegFps(int)\0"
};

const QMetaObject OutputSetupDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_OutputSetupDialog,
      qt_meta_data_OutputSetupDialog, 0 }
};

const QMetaObject *OutputSetupDialog::metaObject() const
{
    return &staticMetaObject;
}

void *OutputSetupDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputSetupDialog))
        return static_cast<void*>(const_cast< OutputSetupDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int OutputSetupDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: reject(); break;
        case 2: slotOutputListCellActivated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: slotScreenListCellActivated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: slotOutputEnabledStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: slotTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: slotOutputNameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: slotCustX((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: slotCustY((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: slotCustW((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: slotCustH((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: slotNetRoleChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: slotHostChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: slotPortChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: slotAllowMultChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: slotNew(); break;
        case 16: slotDel(); break;
        case 17: slotStayOnTop((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: slotMjpegEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: slotMjpegPort((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: slotMjpegFps((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 21;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
