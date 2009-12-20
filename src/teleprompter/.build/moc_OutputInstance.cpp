/****************************************************************************
** Meta object code from reading C++ file 'OutputInstance.h'
**
** Created: Sat Dec 19 21:08:46 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OutputInstance.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OutputInstance.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OutputInstance[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      42,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      28,   15,   15,   15, 0x05,
      45,   15,   15,   15, 0x05,
      58,   15,   15,   15, 0x05,
      76,   15,   15,   15, 0x05,
      99,   97,   15,   15, 0x05,
     137,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     157,   15,   15,   15, 0x0a,
     184,   15,   15,   15, 0x0a,
     214,   15,   15,   15, 0x0a,
     245,   15,   15,   15, 0x0a,
     279,   15,   15,   15, 0x0a,
     305,  298,   15,   15, 0x0a,
     339,   15,   15,   15, 0x2a,
     378,  366,   15,   15, 0x0a,
     409,   15,   15,   15, 0x0a,
     417,   15,   15,   15, 0x0a,
     439,   15,   15,   15, 0x0a,
     478,  463,  456,   15, 0x0a,
     500,   15,  456,   15, 0x2a,
     517,   15,  456,   15, 0x0a,
     531,   15,  456,   15, 0x0a,
     543,   15,  456,   15, 0x0a,
     555,   15,   15,   15, 0x0a,
     576,   15,   15,   15, 0x0a,
     640,  622,   15,   15, 0x0a,
     674,   15,   15,   15, 0x0a,
     698,   15,   15,   15, 0x0a,
     722,   15,   15,   15, 0x0a,
     753,   15,   15,   15, 0x0a,
     784,   15,   15,   15, 0x0a,
     802,   15,   15,   15, 0x0a,
     822,   15,   15,   15, 0x0a,
     856,   15,   15,   15, 0x0a,
     916,  904,   15,   15, 0x09,
     942,   15,   15,   15, 0x29,
     964,   15,   15,   15, 0x09,
     980,   15,   15,   15, 0x09,
    1007, 1001,   15,   15, 0x09,
    1075,   15,   15,   15, 0x09,
    1109, 1092,  456,   15, 0x09,
    1151,   15,   15,   15, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_OutputInstance[] = {
    "OutputInstance\0\0nextGroup()\0"
    "jumpToGroup(int)\0endOfGroup()\0"
    "slideChanged(int)\0slideChanged(Slide*)\0"
    ",\0slideGroupChanged(SlideGroup*,Slide*)\0"
    "imageReady(QImage*)\0addMirror(OutputInstance*)\0"
    "removeMirror(OutputInstance*)\0"
    "addFilter(AbstractItemFilter*)\0"
    "removeFilter(AbstractItemFilter*)\0"
    "removeAllFilters()\0,slide\0"
    "setSlideGroup(SlideGroup*,Slide*)\0"
    "setSlideGroup(SlideGroup*)\0,startSlide\0"
    "setSlideGroup(SlideGroup*,int)\0clear()\0"
    "setBackground(QColor)\0setCanZoom(bool)\0"
    "Slide*\0,takeOwnership\0setSlide(Slide*,bool)\0"
    "setSlide(Slide*)\0setSlide(int)\0"
    "nextSlide()\0prevSlide()\0fadeBlackFrame(bool)\0"
    "setViewerState(SlideGroupViewer::ViewerState)\0"
    ",waitForNextSlide\0setLiveBackground(QFileInfo,bool)\0"
    "setOverlaySlide(Slide*)\0setOverlayEnabled(bool)\0"
    "setTextOnlyFilterEnabled(bool)\0"
    "setAutoResizeTextEnabled(bool)\0"
    "setFadeSpeed(int)\0setFadeQuality(int)\0"
    "setEndActionOverrideEnabled(bool)\0"
    "setEndGroupAction(SlideGroup::EndOfGroupAction)\0"
    "startHidden\0applyOutputSettings(bool)\0"
    "applyOutputSettings()\0slotNextGroup()\0"
    "slotJumpToGroup(int)\0,,,,,\0"
    "slideChanged(Slide*,QString,AbstractItem*,QString,QString,QVariant)\0"
    "slotGrabPixmap()\0group,startSlide\0"
    "setSlideGroupInternal(SlideGroup*,Slide*)\0"
    "setSlideInternal(Slide*)\0"
};

const QMetaObject OutputInstance::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_OutputInstance,
      qt_meta_data_OutputInstance, 0 }
};

const QMetaObject *OutputInstance::metaObject() const
{
    return &staticMetaObject;
}

void *OutputInstance::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputInstance))
        return static_cast<void*>(const_cast< OutputInstance*>(this));
    return QWidget::qt_metacast(_clname);
}

int OutputInstance::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: nextGroup(); break;
        case 1: jumpToGroup((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: endOfGroup(); break;
        case 3: slideChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: slideChanged((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 5: slideGroupChanged((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 6: imageReady((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        case 7: addMirror((*reinterpret_cast< OutputInstance*(*)>(_a[1]))); break;
        case 8: removeMirror((*reinterpret_cast< OutputInstance*(*)>(_a[1]))); break;
        case 9: addFilter((*reinterpret_cast< AbstractItemFilter*(*)>(_a[1]))); break;
        case 10: removeFilter((*reinterpret_cast< AbstractItemFilter*(*)>(_a[1]))); break;
        case 11: removeAllFilters(); break;
        case 12: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 13: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1]))); break;
        case 14: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: clear(); break;
        case 16: setBackground((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 17: setCanZoom((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: { Slide* _r = setSlide((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 19: { Slide* _r = setSlide((*reinterpret_cast< Slide*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 20: { Slide* _r = setSlide((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 21: { Slide* _r = nextSlide();
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 22: { Slide* _r = prevSlide();
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 23: fadeBlackFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: setViewerState((*reinterpret_cast< SlideGroupViewer::ViewerState(*)>(_a[1]))); break;
        case 25: setLiveBackground((*reinterpret_cast< const QFileInfo(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 26: setOverlaySlide((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 27: setOverlayEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: setTextOnlyFilterEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 29: setAutoResizeTextEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 30: setFadeSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 31: setFadeQuality((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 32: setEndActionOverrideEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 33: setEndGroupAction((*reinterpret_cast< SlideGroup::EndOfGroupAction(*)>(_a[1]))); break;
        case 34: applyOutputSettings((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 35: applyOutputSettings(); break;
        case 36: slotNextGroup(); break;
        case 37: slotJumpToGroup((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 38: slideChanged((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< AbstractItem*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QVariant(*)>(_a[6]))); break;
        case 39: slotGrabPixmap(); break;
        case 40: { Slide* _r = setSlideGroupInternal((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 41: setSlideInternal((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 42;
    }
    return _id;
}

// SIGNAL 0
void OutputInstance::nextGroup()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void OutputInstance::jumpToGroup(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OutputInstance::endOfGroup()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void OutputInstance::slideChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void OutputInstance::slideChanged(Slide * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void OutputInstance::slideGroupChanged(SlideGroup * _t1, Slide * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void OutputInstance::imageReady(QImage * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
