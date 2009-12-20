/****************************************************************************
** Meta object code from reading C++ file 'SlideGroupViewer.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SlideGroupViewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SlideGroupViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NativeViewer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_NativeViewer[] = {
    "NativeViewer\0"
};

const QMetaObject NativeViewer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NativeViewer,
      qt_meta_data_NativeViewer, 0 }
};

const QMetaObject *NativeViewer::metaObject() const
{
    return &staticMetaObject;
}

void *NativeViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NativeViewer))
        return static_cast<void*>(const_cast< NativeViewer*>(this));
    return QObject::qt_metacast(_clname);
}

int NativeViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_NativeViewerWin32[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_NativeViewerWin32[] = {
    "NativeViewerWin32\0\0embedHwnd()\0"
};

const QMetaObject NativeViewerWin32::staticMetaObject = {
    { &NativeViewer::staticMetaObject, qt_meta_stringdata_NativeViewerWin32,
      qt_meta_data_NativeViewerWin32, 0 }
};

const QMetaObject *NativeViewerWin32::metaObject() const
{
    return &staticMetaObject;
}

void *NativeViewerWin32::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NativeViewerWin32))
        return static_cast<void*>(const_cast< NativeViewerWin32*>(this));
    return NativeViewer::qt_metacast(_clname);
}

int NativeViewerWin32::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NativeViewer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: embedHwnd(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_SlideGroupViewer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      30,   17,   17,   17, 0x05,
      47,   17,   17,   17, 0x05,
      60,   17,   17,   17, 0x05,
      92,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     132,  117,  110,   17, 0x0a,
     154,   17,  110,   17, 0x2a,
     171,   17,  110,   17, 0x0a,
     185,   17,  110,   17, 0x0a,
     197,   17,  110,   17, 0x0a,
     209,   17,   17,   17, 0x0a,
     230,   17,   17,   17, 0x0a,
     276,  258,   17,   17, 0x0a,
     310,   17,   17,   17, 0x0a,
     334,   17,   17,   17, 0x0a,
     358,   17,   17,   17, 0x0a,
     389,   17,   17,   17, 0x0a,
     420,   17,   17,   17, 0x0a,
     438,   17,   17,   17, 0x0a,
     458,   17,   17,   17, 0x0a,
     492,   17,   17,   17, 0x0a,
     540,   17,   17,   17, 0x0a,
     567,   17,  562,   17, 0x0a,
     582,   17,   17,   17, 0x08,
     603,   17,   17,   17, 0x08,
     630,   17,   17,   17, 0x08,
     653,  651,   17,   17, 0x08,
     686,   17,   17,   17, 0x08,
     761,  709,   17,   17, 0x08,
     829,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SlideGroupViewer[] = {
    "SlideGroupViewer\0\0nextGroup()\0"
    "jumpToGroup(int)\0endOfGroup()\0"
    "viewerStateChanged(ViewerState)\0"
    "slideChanged(int)\0Slide*\0,takeOwnership\0"
    "setSlide(Slide*,bool)\0setSlide(Slide*)\0"
    "setSlide(int)\0nextSlide()\0prevSlide()\0"
    "fadeBlackFrame(bool)\0setViewerState(ViewerState)\0"
    ",waitForNextSlide\0setLiveBackground(QFileInfo,bool)\0"
    "setOverlaySlide(Slide*)\0setOverlayEnabled(bool)\0"
    "setTextOnlyFilterEnabled(bool)\0"
    "setAutoResizeTextEnabled(bool)\0"
    "setFadeSpeed(int)\0setFadeQuality(int)\0"
    "setEndActionOverrideEnabled(bool)\0"
    "setEndGroupAction(SlideGroup::EndOfGroupAction)\0"
    "forceGLDisabled(bool)\0bool\0isGLDisabled()\0"
    "appSettingsChanged()\0aspectRatioChanged(double)\0"
    "videoStreamStarted()\0,\0"
    "crossFadeFinished(Slide*,Slide*)\0"
    "slideDiscarded(Slide*)\0"
    "slide,slideOperation,item,operation,fieldName,value\0"
    "slideChanged(Slide*,QString,AbstractItem*,QString,QString,QVariant)\0"
    "checkCurrentNativeSlide()\0"
};

const QMetaObject SlideGroupViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SlideGroupViewer,
      qt_meta_data_SlideGroupViewer, 0 }
};

const QMetaObject *SlideGroupViewer::metaObject() const
{
    return &staticMetaObject;
}

void *SlideGroupViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SlideGroupViewer))
        return static_cast<void*>(const_cast< SlideGroupViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int SlideGroupViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: nextGroup(); break;
        case 1: jumpToGroup((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: endOfGroup(); break;
        case 3: viewerStateChanged((*reinterpret_cast< ViewerState(*)>(_a[1]))); break;
        case 4: slideChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: { Slide* _r = setSlide((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 6: { Slide* _r = setSlide((*reinterpret_cast< Slide*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 7: { Slide* _r = setSlide((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 8: { Slide* _r = nextSlide();
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 9: { Slide* _r = prevSlide();
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 10: fadeBlackFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: setViewerState((*reinterpret_cast< ViewerState(*)>(_a[1]))); break;
        case 12: setLiveBackground((*reinterpret_cast< const QFileInfo(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 13: setOverlaySlide((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 14: setOverlayEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: setTextOnlyFilterEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: setAutoResizeTextEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: setFadeSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: setFadeQuality((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: setEndActionOverrideEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: setEndGroupAction((*reinterpret_cast< SlideGroup::EndOfGroupAction(*)>(_a[1]))); break;
        case 21: forceGLDisabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: { bool _r = isGLDisabled();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 23: appSettingsChanged(); break;
        case 24: aspectRatioChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 25: videoStreamStarted(); break;
        case 26: crossFadeFinished((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 27: slideDiscarded((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 28: slideChanged((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< AbstractItem*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QVariant(*)>(_a[6]))); break;
        case 29: checkCurrentNativeSlide(); break;
        default: ;
        }
        _id -= 30;
    }
    return _id;
}

// SIGNAL 0
void SlideGroupViewer::nextGroup()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SlideGroupViewer::jumpToGroup(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SlideGroupViewer::endOfGroup()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void SlideGroupViewer::viewerStateChanged(ViewerState _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SlideGroupViewer::slideChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
