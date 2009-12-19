/****************************************************************************
** Meta object code from reading C++ file 'QVideoEncoder.h'
**
** Created: Sat Dec 19 18:15:33 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qvideo/QVideoEncoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QVideoEncoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QVideoEncoder[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   15,   14,   14, 0x05,
      54,   14,   14,   14, 0x05,
      74,   71,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QVideoEncoder[] = {
    "QVideoEncoder\0\0status,fatal\0"
    "errorStatus(QString,bool)\0encodeFinished()\0"
    "ms\0advanceSource(int)\0"
};

const QMetaObject QVideoEncoder::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_QVideoEncoder,
      qt_meta_data_QVideoEncoder, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QVideoEncoder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QVideoEncoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QVideoEncoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QVideoEncoder))
        return static_cast<void*>(const_cast< QVideoEncoder*>(this));
    return QThread::qt_metacast(_clname);
}

int QVideoEncoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: errorStatus((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: encodeFinished(); break;
        case 2: advanceSource((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QVideoEncoder::errorStatus(const QString & _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QVideoEncoder::encodeFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QVideoEncoder::advanceSource(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
