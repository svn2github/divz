/****************************************************************************
** Meta object code from reading C++ file 'SingleOutputSetupDialog.h'
**
** Created: Sat Dec 19 18:15:34 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SingleOutputSetupDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SingleOutputSetupDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SingleOutputSetupDialog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x09,
      34,   24,   24,   24, 0x09,
      45,   43,   24,   24, 0x09,
      82,   24,   24,   24, 0x09,
     117,   24,   24,   24, 0x09,
     137,   24,   24,   24, 0x09,
     168,   24,   24,   24, 0x09,
     183,   24,   24,   24, 0x09,
     198,   24,   24,   24, 0x09,
     213,   24,   24,   24, 0x09,
     228,   24,   24,   24, 0x09,
     253,   24,   24,   24, 0x09,
     278,   24,   24,   24, 0x09,
     299,   24,   24,   24, 0x09,
     325,   24,   24,   24, 0x09,
     345,   24,   24,   24, 0x09,
     368,   24,   24,   24, 0x09,
     387,   24,   24,   24, 0x09,
     405,   24,   24,   24, 0x09,
     427,   24,   24,   24, 0x09,
     439,   24,   24,   24, 0x09,
     451,   24,   24,   24, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SingleOutputSetupDialog[] = {
    "SingleOutputSetupDialog\0\0accept()\0"
    "reject()\0,\0slotScreenListCellActivated(int,int)\0"
    "slotOutputEnabledStateChanged(int)\0"
    "slotTabChanged(int)\0slotOutputNameChanged(QString)\0"
    "slotCustX(int)\0slotCustY(int)\0"
    "slotCustW(int)\0slotCustH(int)\0"
    "slotNetRoleChanged(bool)\0"
    "slotHostChanged(QString)\0slotPortChanged(int)\0"
    "slotAllowMultChanged(int)\0slotStayOnTop(bool)\0"
    "slotMjpegEnabled(bool)\0slotMjpegPort(int)\0"
    "slotMjpegFps(int)\0slotDiskCacheBrowse()\0"
    "addResBtn()\0delResBtn()\0adjustTableSize()\0"
};

const QMetaObject SingleOutputSetupDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SingleOutputSetupDialog,
      qt_meta_data_SingleOutputSetupDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SingleOutputSetupDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SingleOutputSetupDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SingleOutputSetupDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SingleOutputSetupDialog))
        return static_cast<void*>(const_cast< SingleOutputSetupDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int SingleOutputSetupDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: reject(); break;
        case 2: slotScreenListCellActivated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: slotOutputEnabledStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: slotTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: slotOutputNameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: slotCustX((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: slotCustY((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: slotCustW((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: slotCustH((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: slotNetRoleChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: slotHostChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: slotPortChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: slotAllowMultChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: slotStayOnTop((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: slotMjpegEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: slotMjpegPort((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: slotMjpegFps((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: slotDiskCacheBrowse(); break;
        case 19: addResBtn(); break;
        case 20: delResBtn(); break;
        case 21: adjustTableSize(); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
