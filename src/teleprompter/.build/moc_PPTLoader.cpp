/****************************************************************************
** Meta object code from reading C++ file 'PPTLoader.h'
**
** Created: Sat Dec 19 18:15:34 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ppt/PPTLoader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PPTLoader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PPTLoaderTest[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      23,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PPTLoaderTest[] = {
    "PPTLoaderTest\0\0test1()\0test2()\0"
};

const QMetaObject PPTLoaderTest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PPTLoaderTest,
      qt_meta_data_PPTLoaderTest, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PPTLoaderTest::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PPTLoaderTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PPTLoaderTest::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PPTLoaderTest))
        return static_cast<void*>(const_cast< PPTLoaderTest*>(this));
    return QObject::qt_metacast(_clname);
}

int PPTLoaderTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: test1(); break;
        case 1: test2(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_PPTLoader[] = {

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
      15,   11,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PPTLoader[] = {
    "PPTLoader\0\0,,,\0axException(int,QString,QString,QString)\0"
};

const QMetaObject PPTLoader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PPTLoader,
      qt_meta_data_PPTLoader, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PPTLoader::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PPTLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PPTLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PPTLoader))
        return static_cast<void*>(const_cast< PPTLoader*>(this));
    return QObject::qt_metacast(_clname);
}

int PPTLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: axException((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
