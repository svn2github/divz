/****************************************************************************
** Meta object code from reading C++ file 'NativeViewerWin32PPT.h'
**
** Created: Sat Dec 19 18:15:34 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ppt/NativeViewerWin32PPT.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NativeViewerWin32PPT.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NativeViewerWin32PPT[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_NativeViewerWin32PPT[] = {
    "NativeViewerWin32PPT\0\0embedHwnd()\0"
};

const QMetaObject NativeViewerWin32PPT::staticMetaObject = {
    { &NativeViewerWin32::staticMetaObject, qt_meta_stringdata_NativeViewerWin32PPT,
      qt_meta_data_NativeViewerWin32PPT, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NativeViewerWin32PPT::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NativeViewerWin32PPT::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NativeViewerWin32PPT::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NativeViewerWin32PPT))
        return static_cast<void*>(const_cast< NativeViewerWin32PPT*>(this));
    return NativeViewerWin32::qt_metacast(_clname);
}

int NativeViewerWin32PPT::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NativeViewerWin32::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: embedHwnd(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
