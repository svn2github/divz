/****************************************************************************
** Meta object code from reading C++ file 'SlideItemListModel.h'
**
** Created: Sat Dec 19 18:15:35 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SlideItemListModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SlideItemListModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SlideItemListModel[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   19,   19,   19, 0x0a,
     101,   70,   19,   19, 0x08,
     153,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SlideItemListModel[] = {
    "SlideItemListModel\0\0"
    "itemsDropped(QList<AbstractItem*>)\0"
    "releaseSlide()\0item,operation,fieldName,value\0"
    "itemChanged(AbstractItem*,QString,QString,QVariant)\0"
    "modelDirtyTimeout()\0"
};

const QMetaObject SlideItemListModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_SlideItemListModel,
      qt_meta_data_SlideItemListModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SlideItemListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SlideItemListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SlideItemListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SlideItemListModel))
        return static_cast<void*>(const_cast< SlideItemListModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int SlideItemListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemsDropped((*reinterpret_cast< QList<AbstractItem*>(*)>(_a[1]))); break;
        case 1: releaseSlide(); break;
        case 2: itemChanged((*reinterpret_cast< AbstractItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4]))); break;
        case 3: modelDirtyTimeout(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SlideItemListModel::itemsDropped(QList<AbstractItem*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
