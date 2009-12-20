/****************************************************************************
** Meta object code from reading C++ file 'SlideGroupListModel.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SlideGroupListModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SlideGroupListModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SlideGroupListModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,
      50,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      64,   20,   20,   20, 0x0a,
     136,   84,   20,   20, 0x08,
     204,   20,   20,   20, 0x08,
     224,   20,   20,   20, 0x08,
     245,   20,   20,   20, 0x08,
     272,   20,   20,   20, 0x08,
     286,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SlideGroupListModel[] = {
    "SlideGroupListModel\0\0slidesDropped(QList<Slide*>)\0"
    "repaintList()\0releaseSlideGroup()\0"
    "slide,slideOperation,item,operation,fieldName,value\0"
    "slideChanged(Slide*,QString,AbstractItem*,QString,QString,QVariant)\0"
    "modelDirtyTimeout()\0modelDirtyTimeout2()\0"
    "aspectRatioChanged(double)\0makePixmaps()\0"
    "turnOffQueuedIconGeneration()\0"
};

const QMetaObject SlideGroupListModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_SlideGroupListModel,
      qt_meta_data_SlideGroupListModel, 0 }
};

const QMetaObject *SlideGroupListModel::metaObject() const
{
    return &staticMetaObject;
}

void *SlideGroupListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SlideGroupListModel))
        return static_cast<void*>(const_cast< SlideGroupListModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int SlideGroupListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slidesDropped((*reinterpret_cast< QList<Slide*>(*)>(_a[1]))); break;
        case 1: repaintList(); break;
        case 2: releaseSlideGroup(); break;
        case 3: slideChanged((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< AbstractItem*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QVariant(*)>(_a[6]))); break;
        case 4: modelDirtyTimeout(); break;
        case 5: modelDirtyTimeout2(); break;
        case 6: aspectRatioChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: makePixmaps(); break;
        case 8: turnOffQueuedIconGeneration(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SlideGroupListModel::slidesDropped(QList<Slide*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SlideGroupListModel::repaintList()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
