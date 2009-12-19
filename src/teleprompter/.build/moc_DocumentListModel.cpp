/****************************************************************************
** Meta object code from reading C++ file 'DocumentListModel.h'
**
** Created: Sat Dec 19 18:15:34 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DocumentListModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DocumentListModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DocumentListModel[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     122,   53,   18,   18, 0x08,
     215,   18,   18,   18, 0x08,
     235,   18,   18,   18, 0x08,
     262,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DocumentListModel[] = {
    "DocumentListModel\0\0groupsDropped(QList<SlideGroup*>)\0"
    "g,groupOperation,slide,slideOperation,item,operation,fieldName,value\0"
    "slideGroupChanged(SlideGroup*,QString,Slide*,QString,AbstractItem*,QSt"
    "ring,QString,QVariant)\0"
    "modelDirtyTimeout()\0aspectRatioChanged(double)\0"
    "makePixmaps()\0"
};

const QMetaObject DocumentListModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_DocumentListModel,
      qt_meta_data_DocumentListModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DocumentListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DocumentListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DocumentListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DocumentListModel))
        return static_cast<void*>(const_cast< DocumentListModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int DocumentListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: groupsDropped((*reinterpret_cast< QList<SlideGroup*>(*)>(_a[1]))); break;
        case 1: slideGroupChanged((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< Slide*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< AbstractItem*(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6])),(*reinterpret_cast< QString(*)>(_a[7])),(*reinterpret_cast< QVariant(*)>(_a[8]))); break;
        case 2: modelDirtyTimeout(); break;
        case 3: aspectRatioChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: makePixmaps(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DocumentListModel::groupsDropped(QList<SlideGroup*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
