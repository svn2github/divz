/****************************************************************************
** Meta object code from reading C++ file 'VideoFileContent.h'
**
** Created: Sat Dec 19 21:08:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/VideoFileContent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VideoFileContent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VideoFileContent[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       1,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   17,   18,   17, 0x0a,
      42,   37,   17,   17, 0x0a,
      70,   63,   17,   17, 0x08,
      89,   17,   17,   17, 0x08,

 // properties: name, type, flags
     106,   18, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_VideoFileContent[] = {
    "VideoFileContent\0\0QString\0filename()\0"
    "text\0setFilename(QString)\0pixmap\0"
    "setPixmap(QPixmap)\0slotTogglePlay()\0"
    "filename\0"
};

const QMetaObject VideoFileContent::staticMetaObject = {
    { &AbstractContent::staticMetaObject, qt_meta_stringdata_VideoFileContent,
      qt_meta_data_VideoFileContent, 0 }
};

const QMetaObject *VideoFileContent::metaObject() const
{
    return &staticMetaObject;
}

void *VideoFileContent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VideoFileContent))
        return static_cast<void*>(const_cast< VideoFileContent*>(this));
    return AbstractContent::qt_metacast(_clname);
}

int VideoFileContent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractContent::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QString _r = filename();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 1: setFilename((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: setPixmap((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        case 3: slotTogglePlay(); break;
        default: ;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = filename(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setFilename(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
