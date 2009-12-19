/****************************************************************************
** Meta object code from reading C++ file 'JpegServer.h'
**
** Created: Sat Dec 19 18:15:35 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../JpegServer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'JpegServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_JpegServer[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_JpegServer[] = {
    "JpegServer\0"
};

const QMetaObject JpegServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_JpegServer,
      qt_meta_data_JpegServer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &JpegServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *JpegServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *JpegServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_JpegServer))
        return static_cast<void*>(const_cast< JpegServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int JpegServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_JpegServerThread[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   18,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      61,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_JpegServerThread[] = {
    "JpegServerThread\0\0socketError\0"
    "error(QTcpSocket::SocketError)\0"
    "imageReady(QImage*)\0"
};

const QMetaObject JpegServerThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_JpegServerThread,
      qt_meta_data_JpegServerThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &JpegServerThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *JpegServerThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *JpegServerThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_JpegServerThread))
        return static_cast<void*>(const_cast< JpegServerThread*>(this));
    return QThread::qt_metacast(_clname);
}

int JpegServerThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: error((*reinterpret_cast< QTcpSocket::SocketError(*)>(_a[1]))); break;
        case 1: imageReady((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void JpegServerThread::error(QTcpSocket::SocketError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
