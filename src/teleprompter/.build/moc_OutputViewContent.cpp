/****************************************************************************
** Meta object code from reading C++ file 'OutputViewContent.h'
**
** Created: Sat Dec 19 18:15:31 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../items/OutputViewContent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OutputViewContent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OutputViewInst[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      31,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

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
     366,   15,   15,   15, 0x0a,
     374,   15,   15,   15, 0x0a,
     396,   15,   15,   15, 0x0a,
     435,  420,  413,   15, 0x0a,
     457,   15,  413,   15, 0x2a,
     474,   15,   15,   15, 0x0a,
     495,   15,   15,   15, 0x0a,
     559,  541,   15,   15, 0x0a,
     593,   15,   15,   15, 0x0a,
     617,   15,   15,   15, 0x0a,
     641,   15,   15,   15, 0x0a,
     672,   15,   15,   15, 0x0a,
     703,   15,   15,   15, 0x0a,
     721,   15,   15,   15, 0x0a,
     753,  741,   15,   15, 0x09,
     779,   15,   15,   15, 0x29,
     801,   15,   15,   15, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_OutputViewInst[] = {
    "OutputViewInst\0\0nextGroup()\0"
    "jumpToGroup(int)\0endOfGroup()\0"
    "slideChanged(int)\0slideChanged(Slide*)\0"
    ",\0slideGroupChanged(SlideGroup*,Slide*)\0"
    "imageReady(QImage*)\0addMirror(OutputInstance*)\0"
    "removeMirror(OutputInstance*)\0"
    "addFilter(AbstractItemFilter*)\0"
    "removeFilter(AbstractItemFilter*)\0"
    "removeAllFilters()\0,slide\0"
    "setSlideGroup(SlideGroup*,Slide*)\0"
    "setSlideGroup(SlideGroup*)\0clear()\0"
    "setBackground(QColor)\0setCanZoom(bool)\0"
    "Slide*\0,takeOwnership\0setSlide(Slide*,bool)\0"
    "setSlide(Slide*)\0fadeBlackFrame(bool)\0"
    "setViewerState(SlideGroupViewer::ViewerState)\0"
    ",waitForNextSlide\0setLiveBackground(QFileInfo,bool)\0"
    "setOverlaySlide(Slide*)\0setOverlayEnabled(bool)\0"
    "setTextOnlyFilterEnabled(bool)\0"
    "setAutoResizeTextEnabled(bool)\0"
    "setFadeSpeed(int)\0setFadeQuality(int)\0"
    "startHidden\0applyOutputSettings(bool)\0"
    "applyOutputSettings()\0slotGrabPixmap()\0"
};

const QMetaObject OutputViewInst::staticMetaObject = {
    { &OutputInstance::staticMetaObject, qt_meta_stringdata_OutputViewInst,
      qt_meta_data_OutputViewInst, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OutputViewInst::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OutputViewInst::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OutputViewInst::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputViewInst))
        return static_cast<void*>(const_cast< OutputViewInst*>(this));
    return OutputInstance::qt_metacast(_clname);
}

int OutputViewInst::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OutputInstance::qt_metacall(_c, _id, _a);
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
        case 14: clear(); break;
        case 15: setBackground((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 16: setCanZoom((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: { Slide* _r = setSlide((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 18: { Slide* _r = setSlide((*reinterpret_cast< Slide*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 19: fadeBlackFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: setViewerState((*reinterpret_cast< SlideGroupViewer::ViewerState(*)>(_a[1]))); break;
        case 21: setLiveBackground((*reinterpret_cast< const QFileInfo(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 22: setOverlaySlide((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 23: setOverlayEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: setTextOnlyFilterEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: setAutoResizeTextEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: setFadeSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: setFadeQuality((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: applyOutputSettings((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 29: applyOutputSettings(); break;
        case 30: slotGrabPixmap(); break;
        default: ;
        }
        _id -= 31;
    }
    return _id;
}

// SIGNAL 0
void OutputViewInst::nextGroup()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void OutputViewInst::jumpToGroup(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OutputViewInst::endOfGroup()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void OutputViewInst::slideChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void OutputViewInst::slideChanged(Slide * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void OutputViewInst::slideGroupChanged(SlideGroup * _t1, Slide * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void OutputViewInst::imageReady(QImage * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
static const uint qt_meta_data_OutputViewContent[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      31,   19,   18,   18, 0x0a,
      72,   65,   18,   18, 0x0a,
     122,   18,   18,   18, 0x2a,
     139,   18,   18,   18, 0x0a,
     152,   18,   18,   18, 0x0a,
     173,   18,   18,   18, 0x0a,
     191,   18,   18,   18, 0x0a,
     243,  211,   18,   18, 0x08,
     309,   18,   18,   18, 0x08,
     330,   18,   18,   18, 0x08,
     346,   18,   18,   18, 0x08,
     388,   18,   18,   18, 0x08,
     413,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_OutputViewContent[] = {
    "OutputViewContent\0\0,startSlide\0"
    "setSlideGroup(SlideGroup*,Slide*)\0"
    ",trans\0setSlide(Slide*,MyGraphicsScene::SlideTransition)\0"
    "setSlide(Slide*)\0clearSlide()\0"
    "fadeBlackFrame(bool)\0setFadeSpeed(int)\0"
    "setFadeQuality(int)\0item,operation,fieldName,value,\0"
    "slideItemChanged(AbstractItem*,QString,QString,QVariant,QVariant)\0"
    "slotTransitionStep()\0endTransition()\0"
    "socketError(QAbstractSocket::SocketError)\0"
    "networkClientConnected()\0"
    "connectNetworkClient()\0"
};

const QMetaObject OutputViewContent::staticMetaObject = {
    { &AbstractContent::staticMetaObject, qt_meta_stringdata_OutputViewContent,
      qt_meta_data_OutputViewContent, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OutputViewContent::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OutputViewContent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OutputViewContent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputViewContent))
        return static_cast<void*>(const_cast< OutputViewContent*>(this));
    return AbstractContent::qt_metacast(_clname);
}

int OutputViewContent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractContent::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 1: setSlide((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< MyGraphicsScene::SlideTransition(*)>(_a[2]))); break;
        case 2: setSlide((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 3: clearSlide(); break;
        case 4: fadeBlackFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: setFadeSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: setFadeQuality((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: slideItemChanged((*reinterpret_cast< AbstractItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4])),(*reinterpret_cast< QVariant(*)>(_a[5]))); break;
        case 8: slotTransitionStep(); break;
        case 9: endTransition(); break;
        case 10: socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 11: networkClientConnected(); break;
        case 12: connectNetworkClient(); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
