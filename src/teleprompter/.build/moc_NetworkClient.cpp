/****************************************************************************
** Meta object code from reading C++ file 'NetworkClient.h'
**
** Created: Sat Dec 19 18:17:03 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../viewer/NetworkClient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkClient[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      42,   14,   14,   14, 0x05,
      63,   14,   14,   14, 0x05,
     105,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     123,   14,   14,   14, 0x08,
     135,   14,   14,   14, 0x08,
     154,  150,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NetworkClient[] = {
    "NetworkClient\0\0aspectRatioChanged(double)\0"
    "socketDisconnected()\0"
    "socketError(QAbstractSocket::SocketError)\0"
    "socketConnected()\0dataReady()\0"
    "processBlock()\0,,,\0"
    "processCommand(OutputServer::Command,QVariant,QVariant,QVariant)\0"
};

const QMetaObject NetworkClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetworkClient,
      qt_meta_data_NetworkClient, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetworkClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetworkClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetworkClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkClient))
        return static_cast<void*>(const_cast< NetworkClient*>(this));
    return QObject::qt_metacast(_clname);
}

int NetworkClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: aspectRatioChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: socketDisconnected(); break;
        case 2: socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 3: socketConnected(); break;
        case 4: dataReady(); break;
        case 5: processBlock(); break;
        case 6: processCommand((*reinterpret_cast< OutputServer::Command(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void NetworkClient::aspectRatioChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkClient::socketDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void NetworkClient::socketError(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkClient::socketConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
