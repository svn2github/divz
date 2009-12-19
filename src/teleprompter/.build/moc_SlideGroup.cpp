/****************************************************************************
** Meta object code from reading C++ file 'SlideGroup.h'
**
** Created: Sat Dec 19 18:15:32 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../model/SlideGroup.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SlideGroup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SlideGroup[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
      10,   24, // properties
       1,   54, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      64,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     167,  132,   11,   11, 0x08,

 // properties: name, type, flags
     237,  233, 0x02095103,
     245,  233, 0x02095103,
     257,  233, 0x02095103,
     275,  267, 0x0a095103,
     286,  267, 0x0a095103,
     312,  295, 0x0009510b,
     329,  233, 0x02095103,
     353,  346, 0x06095103,
     373,  346, 0x06095103,
     388,  346, 0x06095103,

 // enums: name, flags, count, data
     295, 0x0,    4,   58,

 // enum data: key, value
     405, uint(SlideGroup::LoopToStart),
     417, uint(SlideGroup::GotoNextGroup),
     431, uint(SlideGroup::GotoGroupIndex),
     446, uint(SlideGroup::Stop),

       0        // eod
};

static const char qt_meta_stringdata_SlideGroup[] = {
    "SlideGroup\0\0"
    "slide,slideOperation,item,operation,fieldName,value\0"
    "slideChanged(Slide*,QString,AbstractItem*,QString,QString,QVariant)\0"
    "item,operation,fieldName,value,old\0"
    "slideItemChanged(AbstractItem*,QString,QString,QVariant,QVariant)\0"
    "int\0groupId\0groupNumber\0groupType\0"
    "QString\0groupTitle\0iconFile\0"
    "EndOfGroupAction\0endOfGroupAction\0"
    "jumpToGroupIndex\0double\0inheritFadeSettings\0"
    "crossFadeSpeed\0crossFadeQuality\0"
    "LoopToStart\0GotoNextGroup\0GotoGroupIndex\0"
    "Stop\0"
};

const QMetaObject SlideGroup::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SlideGroup,
      qt_meta_data_SlideGroup, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SlideGroup::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SlideGroup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SlideGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SlideGroup))
        return static_cast<void*>(const_cast< SlideGroup*>(this));
    return QObject::qt_metacast(_clname);
}

int SlideGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slideChanged((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< AbstractItem*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QVariant(*)>(_a[6]))); break;
        case 1: slideItemChanged((*reinterpret_cast< AbstractItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4])),(*reinterpret_cast< QVariant(*)>(_a[5]))); break;
        default: ;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = groupId(); break;
        case 1: *reinterpret_cast< int*>(_v) = groupNumber(); break;
        case 2: *reinterpret_cast< int*>(_v) = groupType(); break;
        case 3: *reinterpret_cast< QString*>(_v) = groupTitle(); break;
        case 4: *reinterpret_cast< QString*>(_v) = iconFile(); break;
        case 5: *reinterpret_cast< EndOfGroupAction*>(_v) = endOfGroupAction(); break;
        case 6: *reinterpret_cast< int*>(_v) = jumpToGroupIndex(); break;
        case 7: *reinterpret_cast< double*>(_v) = inheritFadeSettings(); break;
        case 8: *reinterpret_cast< double*>(_v) = crossFadeSpeed(); break;
        case 9: *reinterpret_cast< double*>(_v) = crossFadeQuality(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setGroupId(*reinterpret_cast< int*>(_v)); break;
        case 1: setGroupNumber(*reinterpret_cast< int*>(_v)); break;
        case 2: setGroupType(*reinterpret_cast< int*>(_v)); break;
        case 3: setGroupTitle(*reinterpret_cast< QString*>(_v)); break;
        case 4: setIconFile(*reinterpret_cast< QString*>(_v)); break;
        case 5: setEndOfGroupAction(*reinterpret_cast< EndOfGroupAction*>(_v)); break;
        case 6: setJumpToGroupIndex(*reinterpret_cast< int*>(_v)); break;
        case 7: setInheritFadeSettings(*reinterpret_cast< double*>(_v)); break;
        case 8: setCrossFadeSpeed(*reinterpret_cast< double*>(_v)); break;
        case 9: setCrossFadeQuality(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void SlideGroup::slideChanged(Slide * _t1, QString _t2, AbstractItem * _t3, QString _t4, QString _t5, QVariant _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
