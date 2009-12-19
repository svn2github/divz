/****************************************************************************
** Meta object code from reading C++ file 'ButtonItem.h'
**
** Created: Sat Dec 19 18:15:30 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/ButtonItem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ButtonItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ButtonItem[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      36,   12,   11,   11, 0x05,
      76,   11,   11,   11, 0x05,
      86,   11,   11,   11, 0x05,
      96,   11,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ButtonItem[] = {
    "ButtonItem\0\0sceneRelPoint,modifiers\0"
    "dragging(QPointF,Qt::KeyboardModifiers)\0"
    "pressed()\0clicked()\0doubleClicked()\0"
};

const QMetaObject ButtonItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ButtonItem,
      qt_meta_data_ButtonItem, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ButtonItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ButtonItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ButtonItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ButtonItem))
        return static_cast<void*>(const_cast< ButtonItem*>(this));
    if (!strcmp(_clname, "QGraphicsItem"))
        return static_cast< QGraphicsItem*>(const_cast< ButtonItem*>(this));
    return QObject::qt_metacast(_clname);
}

int ButtonItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dragging((*reinterpret_cast< const QPointF(*)>(_a[1])),(*reinterpret_cast< Qt::KeyboardModifiers(*)>(_a[2]))); break;
        case 1: pressed(); break;
        case 2: clicked(); break;
        case 3: doubleClicked(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ButtonItem::dragging(const QPointF & _t1, Qt::KeyboardModifiers _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ButtonItem::pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ButtonItem::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void ButtonItem::doubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
