/****************************************************************************
** Meta object code from reading C++ file 'BackgroundContent.h'
**
** Created: Sat Dec 19 21:08:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/BackgroundContent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BackgroundContent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BackgroundImageWarmingThread[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_BackgroundImageWarmingThread[] = {
    "BackgroundImageWarmingThread\0\0"
    "renderDone(QImage*)\0"
};

const QMetaObject BackgroundImageWarmingThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_BackgroundImageWarmingThread,
      qt_meta_data_BackgroundImageWarmingThread, 0 }
};

const QMetaObject *BackgroundImageWarmingThread::metaObject() const
{
    return &staticMetaObject;
}

void *BackgroundImageWarmingThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BackgroundImageWarmingThread))
        return static_cast<void*>(const_cast< BackgroundImageWarmingThread*>(this));
    return QThread::qt_metacast(_clname);
}

int BackgroundImageWarmingThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: renderDone((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void BackgroundImageWarmingThread::renderDone(QImage * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_BackgroundImageWarmingThreadManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      37,   36,   36,   36, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BackgroundImageWarmingThreadManager[] = {
    "BackgroundImageWarmingThreadManager\0"
    "\0renderDone(QImage*)\0"
};

const QMetaObject BackgroundImageWarmingThreadManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BackgroundImageWarmingThreadManager,
      qt_meta_data_BackgroundImageWarmingThreadManager, 0 }
};

const QMetaObject *BackgroundImageWarmingThreadManager::metaObject() const
{
    return &staticMetaObject;
}

void *BackgroundImageWarmingThreadManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BackgroundImageWarmingThreadManager))
        return static_cast<void*>(const_cast< BackgroundImageWarmingThreadManager*>(this));
    return QObject::qt_metacast(_clname);
}

int BackgroundImageWarmingThreadManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: renderDone((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_BackgroundContent[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   19,   18,   18, 0x08,
      45,   18,   18,   18, 0x08,
      70,   18,   18,   18, 0x08,
      82,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BackgroundContent[] = {
    "BackgroundContent\0\0pixmap\0setPixmap(QPixmap)\0"
    "sceneRectChanged(QRectF)\0renderSvg()\0"
    "animateZoom()\0"
};

const QMetaObject BackgroundContent::staticMetaObject = {
    { &AbstractContent::staticMetaObject, qt_meta_stringdata_BackgroundContent,
      qt_meta_data_BackgroundContent, 0 }
};

const QMetaObject *BackgroundContent::metaObject() const
{
    return &staticMetaObject;
}

void *BackgroundContent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BackgroundContent))
        return static_cast<void*>(const_cast< BackgroundContent*>(this));
    return AbstractContent::qt_metacast(_clname);
}

int BackgroundContent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractContent::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setPixmap((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        case 1: sceneRectChanged((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 2: renderSvg(); break;
        case 3: animateZoom(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
