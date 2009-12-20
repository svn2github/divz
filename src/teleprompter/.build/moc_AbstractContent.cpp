/****************************************************************************
** Meta object code from reading C++ file 'AbstractContent.h'
**
** Created: Sat Dec 19 21:08:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/AbstractContent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AbstractContent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AbstractContent[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      28,   17,   16,   16, 0x05,
      55,   48,   16,   16, 0x05,
      72,   16,   16,   16, 0x05,
      87,   16,   16,   16, 0x05,
     100,   16,   16,   16, 0x05,
     117,   16,   16,   16, 0x05,
     127,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     159,   16,   16,   16, 0x09,
     175,   16,   16,   16, 0x09,
     192,   16,   16,   16, 0x09,
     209,   16,   16,   16, 0x09,
     226,   16,   16,   16, 0x09,
     242,   16,   16,   16, 0x09,
     280,  255,   16,   16, 0x09,
     348,  324,   16,   16, 0x08,
     395,   16,   16,   16, 0x08,
     418,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AbstractContent[] = {
    "AbstractContent\0\0scenePoint\0"
    "configureMe(QPoint)\0opcode\0changeStack(int)\0"
    "backgroundMe()\0deleteItem()\0"
    "contentChanged()\0resized()\0"
    "doubleClicked(AbstractContent*)\0"
    "slotConfigure()\0slotStackFront()\0"
    "slotStackRaise()\0slotStackLower()\0"
    "slotStackBack()\0slotSaveAs()\0"
    "fieldName,value,oldValue\0"
    "modelItemChanged(QString,QVariant,QVariant)\0"
    "sceneRelPoint,modifiers\0"
    "slotPerspective(QPointF,Qt::KeyboardModifiers)\0"
    "slotClearPerspective()\0slotDirtyEnded()\0"
};

const QMetaObject AbstractContent::staticMetaObject = {
    { &AbstractDisposeable::staticMetaObject, qt_meta_stringdata_AbstractContent,
      qt_meta_data_AbstractContent, 0 }
};

const QMetaObject *AbstractContent::metaObject() const
{
    return &staticMetaObject;
}

void *AbstractContent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AbstractContent))
        return static_cast<void*>(const_cast< AbstractContent*>(this));
    return AbstractDisposeable::qt_metacast(_clname);
}

int AbstractContent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractDisposeable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: configureMe((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 1: changeStack((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: backgroundMe(); break;
        case 3: deleteItem(); break;
        case 4: contentChanged(); break;
        case 5: resized(); break;
        case 6: doubleClicked((*reinterpret_cast< AbstractContent*(*)>(_a[1]))); break;
        case 7: slotConfigure(); break;
        case 8: slotStackFront(); break;
        case 9: slotStackRaise(); break;
        case 10: slotStackLower(); break;
        case 11: slotStackBack(); break;
        case 12: slotSaveAs(); break;
        case 13: modelItemChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 14: slotPerspective((*reinterpret_cast< const QPointF(*)>(_a[1])),(*reinterpret_cast< Qt::KeyboardModifiers(*)>(_a[2]))); break;
        case 15: slotClearPerspective(); break;
        case 16: slotDirtyEnded(); break;
        default: ;
        }
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void AbstractContent::configureMe(const QPoint & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AbstractContent::changeStack(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AbstractContent::backgroundMe()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void AbstractContent::deleteItem()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void AbstractContent::contentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void AbstractContent::resized()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void AbstractContent::doubleClicked(AbstractContent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
