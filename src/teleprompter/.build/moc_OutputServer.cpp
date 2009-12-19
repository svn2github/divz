/****************************************************************************
** Meta object code from reading C++ file 'OutputServer.h'
**
** Created: Sat Dec 19 18:15:35 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OutputServer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OutputServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OutputServer[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      88,   81,   13,   13, 0x0a,
     141,  136,   13,   13, 0x2a,
     183,  180,   13,   13, 0x2a,
     213,   13,   13,   13, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_OutputServer[] = {
    "OutputServer\0\0,,,\0"
    "commandReady(OutputServer::Command,QVariant,QVariant,QVariant)\0"
    ",a,b,c\0sendCommand(Command,QVariant,QVariant,QVariant)\0"
    ",a,b\0sendCommand(Command,QVariant,QVariant)\0"
    ",a\0sendCommand(Command,QVariant)\0"
    "sendCommand(Command)\0"
};

const QMetaObject OutputServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_OutputServer,
      qt_meta_data_OutputServer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OutputServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OutputServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OutputServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputServer))
        return static_cast<void*>(const_cast< OutputServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int OutputServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: commandReady((*reinterpret_cast< OutputServer::Command(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4]))); break;
        case 1: sendCommand((*reinterpret_cast< Command(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4]))); break;
        case 2: sendCommand((*reinterpret_cast< Command(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 3: sendCommand((*reinterpret_cast< Command(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 4: sendCommand((*reinterpret_cast< Command(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void OutputServer::commandReady(OutputServer::Command _t1, QVariant _t2, QVariant _t3, QVariant _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_OutputServerThread[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      32,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      67,   63,   19,   19, 0x0a,
     130,   19,   19,   19, 0x08,
     153,   63,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_OutputServerThread[] = {
    "OutputServerThread\0\0socketError\0"
    "error(QTcpSocket::SocketError)\0,,,\0"
    "queueCommand(OutputServer::Command,QVariant,QVariant,QVariant)\0"
    "processCommandBuffer()\0"
    "sendCommand(OutputServer::Command,QVariant,QVariant,QVariant)\0"
};

const QMetaObject OutputServerThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_OutputServerThread,
      qt_meta_data_OutputServerThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OutputServerThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OutputServerThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OutputServerThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputServerThread))
        return static_cast<void*>(const_cast< OutputServerThread*>(this));
    return QObject::qt_metacast(_clname);
}

int OutputServerThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: error((*reinterpret_cast< QTcpSocket::SocketError(*)>(_a[1]))); break;
        case 1: queueCommand((*reinterpret_cast< OutputServer::Command(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4]))); break;
        case 2: processCommandBuffer(); break;
        case 3: sendCommand((*reinterpret_cast< OutputServer::Command(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void OutputServerThread::error(QTcpSocket::SocketError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
