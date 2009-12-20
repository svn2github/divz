/****************************************************************************
** Meta object code from reading C++ file 'MyGraphicsScene.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MyGraphicsScene.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyGraphicsScene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyGraphicsScene[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      24,   17,   16,   16, 0x05,
      55,   16,   16,   16, 0x05,
      98,   80,   16,   16, 0x05,
     130,   80,   16,   16, 0x05,
     163,   16,   16,   16, 0x05,
     191,  186,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     243,  227,   16,   16, 0x0a,
     270,   16,   16,   16, 0x2a,
     293,   16,   16,   16, 0x0a,
     311,   16,   16,   16, 0x0a,
     323,   16,   16,   16, 0x09,
     363,   16,   16,   16, 0x08,
     381,   16,   16,   16, 0x08,
     402,   16,   16,   16, 0x08,
     418,   16,   16,   16, 0x08,
     452,  441,   16,   16, 0x08,
     481,   16,   16,   16, 0x08,
     503,   16,   16,   16, 0x08,
     530,  523,   16,   16, 0x08,
     598,  567,   16,   16, 0x08,
     666,  631,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MyGraphicsScene[] = {
    "MyGraphicsScene\0\0widget\0"
    "showPropertiesWidget(QWidget*)\0"
    "sceneRectChanged(QRectF)\0oldSlide,newSlide\0"
    "crossFadeStarted(Slide*,Slide*)\0"
    "crossFadeFinished(Slide*,Slide*)\0"
    "slideDiscarded(Slide*)\0item\0"
    "itemDoubleClicked(AbstractContent*)\0"
    "removeSelection\0copyCurrentSelection(bool)\0"
    "copyCurrentSelection()\0pasteCopyBuffer()\0"
    "selectAll()\0slotItemDoubleClicked(AbstractContent*)\0"
    "startTransition()\0slotTransitionStep()\0"
    "endTransition()\0slotSelectionChanged()\0"
    "scenePoint\0slotConfigureContent(QPoint)\0"
    "slotStackContent(int)\0slotDeleteContent()\0"
    "config\0slotDeleteConfig(GenericItemConfig*)\0"
    "frameClass,mirrored,allContent\0"
    "slotApplyLook(quint32,bool,bool)\0"
    "item,operation,fieldName,value,old\0"
    "slideItemChanged(AbstractItem*,QString,QString,QVariant,QVariant)\0"
};

const QMetaObject MyGraphicsScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_MyGraphicsScene,
      qt_meta_data_MyGraphicsScene, 0 }
};

const QMetaObject *MyGraphicsScene::metaObject() const
{
    return &staticMetaObject;
}

void *MyGraphicsScene::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyGraphicsScene))
        return static_cast<void*>(const_cast< MyGraphicsScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int MyGraphicsScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showPropertiesWidget((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: sceneRectChanged((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 2: crossFadeStarted((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 3: crossFadeFinished((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 4: slideDiscarded((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 5: itemDoubleClicked((*reinterpret_cast< AbstractContent*(*)>(_a[1]))); break;
        case 6: copyCurrentSelection((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: copyCurrentSelection(); break;
        case 8: pasteCopyBuffer(); break;
        case 9: selectAll(); break;
        case 10: slotItemDoubleClicked((*reinterpret_cast< AbstractContent*(*)>(_a[1]))); break;
        case 11: startTransition(); break;
        case 12: slotTransitionStep(); break;
        case 13: endTransition(); break;
        case 14: slotSelectionChanged(); break;
        case 15: slotConfigureContent((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 16: slotStackContent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: slotDeleteContent(); break;
        case 18: slotDeleteConfig((*reinterpret_cast< GenericItemConfig*(*)>(_a[1]))); break;
        case 19: slotApplyLook((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 20: slideItemChanged((*reinterpret_cast< AbstractItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4])),(*reinterpret_cast< QVariant(*)>(_a[5]))); break;
        default: ;
        }
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void MyGraphicsScene::showPropertiesWidget(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MyGraphicsScene::sceneRectChanged(const QRectF & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MyGraphicsScene::crossFadeStarted(Slide * _t1, Slide * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MyGraphicsScene::crossFadeFinished(Slide * _t1, Slide * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MyGraphicsScene::slideDiscarded(Slide * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MyGraphicsScene::itemDoubleClicked(AbstractContent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
