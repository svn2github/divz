/****************************************************************************
** Meta object code from reading C++ file 'QVideo.h'
**
** Created: Sat Dec 19 18:15:33 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qvideo/QVideo.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QVideo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QVideo[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      22,    7,    7,    7, 0x05,
      47,   41,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      85,    7,    7,    7, 0x0a,
      95,   92,    7,    7, 0x0a,
     105,    7,    7,    7, 0x0a,
     113,    7,    7,    7, 0x0a,
     127,   92,  120,    7, 0x0a,
     140,    7,    7,    7, 0x0a,
     156,  150,    7,    7, 0x0a,
     171,    7,    7,    7, 0x0a,
     197,    7,    7,    7, 0x0a,
     212,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QVideo[] = {
    "QVideo\0\0startDecode()\0newPixmap(QPixmap)\0"
    "state\0movieStateChanged(QMovie::MovieState)\0"
    "play()\0ms\0seek(int)\0pause()\0stop()\0"
    "QImage\0advance(int)\0restart()\0ready\0"
    "setReady(bool)\0determineVideoEndAction()\0"
    "displayFrame()\0makeMovie()\0"
};

const QMetaObject QVideo::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QVideo,
      qt_meta_data_QVideo, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QVideo::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QVideo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QVideo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QVideo))
        return static_cast<void*>(const_cast< QVideo*>(this));
    return QObject::qt_metacast(_clname);
}

int QVideo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startDecode(); break;
        case 1: newPixmap((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        case 2: movieStateChanged((*reinterpret_cast< QMovie::MovieState(*)>(_a[1]))); break;
        case 3: play(); break;
        case 4: seek((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: pause(); break;
        case 6: stop(); break;
        case 7: { QImage _r = advance((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QImage*>(_a[0]) = _r; }  break;
        case 8: restart(); break;
        case 9: setReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: determineVideoEndAction(); break;
        case 11: displayFrame(); break;
        case 12: makeMovie(); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void QVideo::startDecode()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QVideo::newPixmap(const QPixmap & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QVideo::movieStateChanged(QMovie::MovieState _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
