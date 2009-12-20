/****************************************************************************
** Meta object code from reading C++ file 'QVideoProvider.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qvideo/QVideoProvider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QVideoProvider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QVideoIconGenerator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QVideoIconGenerator[] = {
    "QVideoIconGenerator\0\0newPixmap(QPixmap)\0"
};

const QMetaObject QVideoIconGenerator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QVideoIconGenerator,
      qt_meta_data_QVideoIconGenerator, 0 }
};

const QMetaObject *QVideoIconGenerator::metaObject() const
{
    return &staticMetaObject;
}

void *QVideoIconGenerator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QVideoIconGenerator))
        return static_cast<void*>(const_cast< QVideoIconGenerator*>(this));
    return QObject::qt_metacast(_clname);
}

int QVideoIconGenerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newPixmap((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_QVideoProvider[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      32,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      48,   15,   15,   15, 0x0a,
      55,   15,   15,   15, 0x0a,
      62,   15,   15,   15, 0x0a,
      73,   70,   15,   15, 0x0a,
      92,   85,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QVideoProvider[] = {
    "QVideoProvider\0\0streamStarted()\0"
    "streamStopped()\0stop()\0play()\0pause()\0"
    "ms\0seekTo(int)\0pixmap\0newPixmap(QPixmap)\0"
};

const QMetaObject QVideoProvider::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QVideoProvider,
      qt_meta_data_QVideoProvider, 0 }
};

const QMetaObject *QVideoProvider::metaObject() const
{
    return &staticMetaObject;
}

void *QVideoProvider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QVideoProvider))
        return static_cast<void*>(const_cast< QVideoProvider*>(this));
    return QObject::qt_metacast(_clname);
}

int QVideoProvider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: streamStarted(); break;
        case 1: streamStopped(); break;
        case 2: stop(); break;
        case 3: play(); break;
        case 4: pause(); break;
        case 5: seekTo((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: newPixmap((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QVideoProvider::streamStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QVideoProvider::streamStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
