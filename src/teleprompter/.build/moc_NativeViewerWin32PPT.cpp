/****************************************************************************
** Meta object code from reading C++ file 'NativeViewerWin32PPT.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ppt/NativeViewerWin32PPT.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NativeViewerWin32PPT.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NativeViewerWin32PPT[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

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

const QMetaObject *NativeViewerWin32PPT::metaObject() const
{
    return &staticMetaObject;
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
