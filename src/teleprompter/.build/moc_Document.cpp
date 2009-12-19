/****************************************************************************
** Meta object code from reading C++ file 'Document.h'
**
** Created: Sat Dec 19 18:15:32 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../model/Document.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Document.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Document[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       3,   29, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      79,   10,    9,    9, 0x05,
     172,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     251,  199,    9,    9, 0x08,

 // properties: name, type, flags
     327,  319, 0x0a095103,
     336,  319, 0x0a095103,
     352,  345, 0x06095103,

       0        // eod
};

static const char qt_meta_stringdata_Document[] = {
    "Document\0\0"
    "g,groupOperation,slide,slideOperation,item,operation,fieldName,value\0"
    "slideGroupChanged(SlideGroup*,QString,Slide*,QString,AbstractItem*,QSt"
    "ring,QString,QVariant)\0"
    "aspectRatioChanged(double)\0"
    "slide,slideOperation,item,operation,fieldName,value\0"
    "slideChanged(Slide*,QString,AbstractItem*,QString,QString,QVariant)\0"
    "QString\0docTitle\0filename\0double\0"
    "aspectRatio\0"
};

const QMetaObject Document::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Document,
      qt_meta_data_Document, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Document::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Document::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Document::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Document))
        return static_cast<void*>(const_cast< Document*>(this));
    return QObject::qt_metacast(_clname);
}

int Document::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slideGroupChanged((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< Slide*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< AbstractItem*(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6])),(*reinterpret_cast< QString(*)>(_a[7])),(*reinterpret_cast< QVariant(*)>(_a[8]))); break;
        case 1: aspectRatioChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: slideChanged((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< AbstractItem*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QVariant(*)>(_a[6]))); break;
        default: ;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = docTitle(); break;
        case 1: *reinterpret_cast< QString*>(_v) = filename(); break;
        case 2: *reinterpret_cast< double*>(_v) = aspectRatio(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setDocTitle(*reinterpret_cast< QString*>(_v)); break;
        case 1: setFilename(*reinterpret_cast< QString*>(_v)); break;
        case 2: setAspectRatio(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Document::slideGroupChanged(SlideGroup * _t1, QString _t2, Slide * _t3, QString _t4, AbstractItem * _t5, QString _t6, QString _t7, QVariant _t8)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Document::aspectRatioChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_DocumentSaveThread[] = {

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

static const char qt_meta_stringdata_DocumentSaveThread[] = {
    "DocumentSaveThread\0"
};

const QMetaObject DocumentSaveThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_DocumentSaveThread,
      qt_meta_data_DocumentSaveThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DocumentSaveThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DocumentSaveThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DocumentSaveThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DocumentSaveThread))
        return static_cast<void*>(const_cast< DocumentSaveThread*>(this));
    return QThread::qt_metacast(_clname);
}

int DocumentSaveThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
