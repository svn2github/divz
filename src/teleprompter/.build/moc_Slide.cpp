/****************************************************************************
** Meta object code from reading C++ file 'Slide.h'
**
** Created: Sat Dec 19 18:15:31 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../model/Slide.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Slide.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Slide[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       6,   24, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      47,    7,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
     130,  113,    6,    6, 0x08,

 // properties: name, type, flags
     173,  169, 0x02095103,
     181,  169, 0x02095103,
     200,  193, 0x06095103,
     215,  193, 0x06095103,
     235,  193, 0x06095103,
     250,  193, 0x06095103,

       0        // eod
};

static const char qt_meta_stringdata_Slide[] = {
    "Slide\0\0item,operation,fieldName,value,oldValue\0"
    "slideItemChanged(AbstractItem*,QString,QString,QVariant,QVariant)\0"
    "fieldName,value,\0"
    "itemChanged(QString,QVariant,QVariant)\0"
    "int\0slideId\0slideNumber\0double\0"
    "autoChangeTime\0inheritFadeSettings\0"
    "crossFadeSpeed\0crossFadeQuality\0"
};

const QMetaObject Slide::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Slide,
      qt_meta_data_Slide, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Slide::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Slide::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Slide::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Slide))
        return static_cast<void*>(const_cast< Slide*>(this));
    return QObject::qt_metacast(_clname);
}

int Slide::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slideItemChanged((*reinterpret_cast< AbstractItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4])),(*reinterpret_cast< QVariant(*)>(_a[5]))); break;
        case 1: itemChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = slideId(); break;
        case 1: *reinterpret_cast< int*>(_v) = slideNumber(); break;
        case 2: *reinterpret_cast< double*>(_v) = autoChangeTime(); break;
        case 3: *reinterpret_cast< double*>(_v) = inheritFadeSettings(); break;
        case 4: *reinterpret_cast< double*>(_v) = crossFadeSpeed(); break;
        case 5: *reinterpret_cast< double*>(_v) = crossFadeQuality(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSlideId(*reinterpret_cast< int*>(_v)); break;
        case 1: setSlideNumber(*reinterpret_cast< int*>(_v)); break;
        case 2: setAutoChangeTime(*reinterpret_cast< double*>(_v)); break;
        case 3: setInheritFadeSettings(*reinterpret_cast< double*>(_v)); break;
        case 4: setCrossFadeSpeed(*reinterpret_cast< double*>(_v)); break;
        case 5: setCrossFadeQuality(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Slide::slideItemChanged(AbstractItem * _t1, QString _t2, QString _t3, QVariant _t4, QVariant _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
