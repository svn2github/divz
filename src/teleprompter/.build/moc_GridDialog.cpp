/****************************************************************************
** Meta object code from reading C++ file 'GridDialog.h'
**
** Created: Sat Dec 19 18:15:35 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../GridDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GridDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GridDialog[] = {

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
      12,   11,   11,   11, 0x09,
      36,   11,   11,   11, 0x09,
      56,   11,   11,   11, 0x09,
      78,   11,   11,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GridDialog[] = {
    "GridDialog\0\0snapToGridEnabled(bool)\0"
    "setGridSize(double)\0setThirdEnabled(bool)\0"
    "linkActivated(QString)\0"
};

const QMetaObject GridDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GridDialog,
      qt_meta_data_GridDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GridDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GridDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GridDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GridDialog))
        return static_cast<void*>(const_cast< GridDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int GridDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: snapToGridEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: setGridSize((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setThirdEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: linkActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
