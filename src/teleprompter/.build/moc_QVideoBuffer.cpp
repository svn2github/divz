/****************************************************************************
** Meta object code from reading C++ file 'QVideoBuffer.h'
**
** Created: Sat Dec 19 18:15:33 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qvideo/QVideoBuffer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QVideoBuffer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QVideoBuffer[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      43,   13,   25,   13, 0x0a,
      64,   58,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QVideoBuffer[] = {
    "QVideoBuffer\0\0nowEmpty()\0QFFMpegVideoFrame\0"
    "getNextFrame()\0frame\0addFrame(QFFMpegVideoFrame)\0"
};

const QMetaObject QVideoBuffer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QVideoBuffer,
      qt_meta_data_QVideoBuffer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QVideoBuffer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QVideoBuffer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QVideoBuffer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QVideoBuffer))
        return static_cast<void*>(const_cast< QVideoBuffer*>(this));
    return QObject::qt_metacast(_clname);
}

int QVideoBuffer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: nowEmpty(); break;
        case 1: { QFFMpegVideoFrame _r = getNextFrame();
            if (_a[0]) *reinterpret_cast< QFFMpegVideoFrame*>(_a[0]) = _r; }  break;
        case 2: addFrame((*reinterpret_cast< QFFMpegVideoFrame(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QVideoBuffer::nowEmpty()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
