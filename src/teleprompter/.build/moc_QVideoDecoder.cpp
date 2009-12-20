/****************************************************************************
** Meta object code from reading C++ file 'QVideoDecoder.h'
**
** Created: Sat Dec 19 21:08:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qvideo/QVideoDecoder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QVideoDecoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QVideoDecoder[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x05,
      49,   14,   14,   14, 0x05,
      56,   14,   14,   14, 0x05,
      67,   14,   14,   14, 0x05,
      86,   80,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      98,   14,   14,   14, 0x0a,
     105,   14,   14,   14, 0x0a,
     114,   14,   14,   14, 0x0a,
     126,   14,   14,   14, 0x0a,
     163,   14,  145,   14, 0x0a,
     178,   15,   14,   14, 0x0a,
     206,   15,   14,   14, 0x0a,
     241,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QVideoDecoder[] = {
    "QVideoDecoder\0\0frame\0newFrame(QFFMpegVideoFrame)\0"
    "done()\0decodeMe()\0reachedEnd()\0state\0"
    "ready(bool)\0read()\0decode()\0readFrame()\0"
    "decodeVideoFrame()\0QFFMpegVideoFrame\0"
    "getNextFrame()\0addFrame(QFFMpegVideoFrame)\0"
    "setCurrentFrame(QFFMpegVideoFrame)\0"
    "flushBuffers()\0"
};

const QMetaObject QVideoDecoder::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_QVideoDecoder,
      qt_meta_data_QVideoDecoder, 0 }
};

const QMetaObject *QVideoDecoder::metaObject() const
{
    return &staticMetaObject;
}

void *QVideoDecoder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QVideoDecoder))
        return static_cast<void*>(const_cast< QVideoDecoder*>(this));
    return QThread::qt_metacast(_clname);
}

int QVideoDecoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newFrame((*reinterpret_cast< QFFMpegVideoFrame(*)>(_a[1]))); break;
        case 1: done(); break;
        case 2: decodeMe(); break;
        case 3: reachedEnd(); break;
        case 4: ready((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: read(); break;
        case 6: decode(); break;
        case 7: readFrame(); break;
        case 8: decodeVideoFrame(); break;
        case 9: { QFFMpegVideoFrame _r = getNextFrame();
            if (_a[0]) *reinterpret_cast< QFFMpegVideoFrame*>(_a[0]) = _r; }  break;
        case 10: addFrame((*reinterpret_cast< QFFMpegVideoFrame(*)>(_a[1]))); break;
        case 11: setCurrentFrame((*reinterpret_cast< QFFMpegVideoFrame(*)>(_a[1]))); break;
        case 12: flushBuffers(); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void QVideoDecoder::newFrame(QFFMpegVideoFrame _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QVideoDecoder::done()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QVideoDecoder::decodeMe()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QVideoDecoder::reachedEnd()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void QVideoDecoder::ready(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
