/****************************************************************************
** Meta object code from reading C++ file 'AbstractVisualItem.h'
**
** Created: Sat Dec 19 21:08:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../model/AbstractVisualItem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AbstractVisualItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AbstractVisualItem[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      29,   12, // properties
       4,   99, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      27,   19, 0x1a095103,
      38,   31, 0x14095103,
      51,   19, 0x1a095103,
      66,   19, 0x1a095103,
      86,   81, 0x01095103,
     103,   96, 0x06095103,
     110,   96, 0x06095103,
     118,   96, 0x06095103,
     128,   96, 0x06095103,
     138,   96, 0x06095103,
     156,  148, 0x03095103,
     176,  167, 0x0009510b,
     192,  185, 0x42095103,
     210,  202, 0x0a095103,
     224,  202, 0x0a095103,
     253,  238, 0x0009510b,
     268,   81, 0x01095103,
     288,  283, 0x4d095103,
     299,   81, 0x01095103,
     313,   96, 0x06095103,
     326,   81, 0x01095103,
     340,   96, 0x06095103,
     357,   96, 0x06095103,
     371,   96, 0x06095103,
     385,  185, 0x42095103,
     397,   81, 0x01095103,
     419,  415, 0x02095103,
     445,  429, 0x0009510b,
     481,  461, 0x0009510b,

 // enums: name, flags, count, data
     167, 0x0,    5,  115,
     238, 0x0,    2,  125,
     461, 0x0,    3,  129,
     429, 0x0,   10,  135,

 // enum data: key, value
     495, uint(AbstractVisualItem::None),
     500, uint(AbstractVisualItem::Solid),
     506, uint(AbstractVisualItem::Gradient),
     515, uint(AbstractVisualItem::Image),
     521, uint(AbstractVisualItem::Video),
     527, uint(AbstractVisualItem::VideoLoop),
     537, uint(AbstractVisualItem::VideoStop),
     547, uint(AbstractVisualItem::ZoomIn),
     554, uint(AbstractVisualItem::ZoomOut),
     562, uint(AbstractVisualItem::ZoomRandom),
     573, uint(AbstractVisualItem::ZoomAnchorRandom),
     590, uint(AbstractVisualItem::ZoomCenter),
     601, uint(AbstractVisualItem::ZoomTopLeft),
     613, uint(AbstractVisualItem::ZoomTopMid),
     624, uint(AbstractVisualItem::ZoomTopRight),
     637, uint(AbstractVisualItem::ZoomRightMid),
     650, uint(AbstractVisualItem::ZoomBottomRight),
     666, uint(AbstractVisualItem::ZoomBottomMid),
     680, uint(AbstractVisualItem::ZoomBottomLeft),
     695, uint(AbstractVisualItem::ZoomLeftMid),

       0        // eod
};

static const char qt_meta_stringdata_AbstractVisualItem[] = {
    "AbstractVisualItem\0QPointF\0pos\0QRectF\0"
    "contentsRect\0sourceOffsetTL\0sourceOffsetBR\0"
    "bool\0isVisible\0double\0zValue\0opacity\0"
    "xRotation\0yRotation\0zRotation\0quint32\0"
    "frameClass\0FillType\0fillType\0QBrush\0"
    "fillBrush\0QString\0fillImageFile\0"
    "fillVideoFile\0VideoEndAction\0"
    "videoEndAction\0outlineEnabled\0QPen\0"
    "outlinePen\0mirrorEnabled\0mirrorOffset\0"
    "shadowEnabled\0shadowBlurRadius\0"
    "shadowOffsetX\0shadowOffsetY\0shadowBrush\0"
    "zoomEffectEnabled\0int\0zoomSpeed\0"
    "ZoomAnchorPoint\0zoomAnchorPoint\0"
    "ZoomEffectDirection\0zoomDirection\0"
    "None\0Solid\0Gradient\0Image\0Video\0"
    "VideoLoop\0VideoStop\0ZoomIn\0ZoomOut\0"
    "ZoomRandom\0ZoomAnchorRandom\0ZoomCenter\0"
    "ZoomTopLeft\0ZoomTopMid\0ZoomTopRight\0"
    "ZoomRightMid\0ZoomBottomRight\0ZoomBottomMid\0"
    "ZoomBottomLeft\0ZoomLeftMid\0"
};

const QMetaObject AbstractVisualItem::staticMetaObject = {
    { &AbstractItem::staticMetaObject, qt_meta_stringdata_AbstractVisualItem,
      qt_meta_data_AbstractVisualItem, 0 }
};

const QMetaObject *AbstractVisualItem::metaObject() const
{
    return &staticMetaObject;
}

void *AbstractVisualItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AbstractVisualItem))
        return static_cast<void*>(const_cast< AbstractVisualItem*>(this));
    return AbstractItem::qt_metacast(_clname);
}

int AbstractVisualItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QPointF*>(_v) = pos(); break;
        case 1: *reinterpret_cast< QRectF*>(_v) = contentsRect(); break;
        case 2: *reinterpret_cast< QPointF*>(_v) = sourceOffsetTL(); break;
        case 3: *reinterpret_cast< QPointF*>(_v) = sourceOffsetBR(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isVisible(); break;
        case 5: *reinterpret_cast< double*>(_v) = zValue(); break;
        case 6: *reinterpret_cast< double*>(_v) = opacity(); break;
        case 7: *reinterpret_cast< double*>(_v) = xRotation(); break;
        case 8: *reinterpret_cast< double*>(_v) = yRotation(); break;
        case 9: *reinterpret_cast< double*>(_v) = zRotation(); break;
        case 10: *reinterpret_cast< quint32*>(_v) = frameClass(); break;
        case 11: *reinterpret_cast< FillType*>(_v) = fillType(); break;
        case 12: *reinterpret_cast< QBrush*>(_v) = fillBrush(); break;
        case 13: *reinterpret_cast< QString*>(_v) = fillImageFile(); break;
        case 14: *reinterpret_cast< QString*>(_v) = fillVideoFile(); break;
        case 15: *reinterpret_cast< VideoEndAction*>(_v) = videoEndAction(); break;
        case 16: *reinterpret_cast< bool*>(_v) = outlineEnabled(); break;
        case 17: *reinterpret_cast< QPen*>(_v) = outlinePen(); break;
        case 18: *reinterpret_cast< bool*>(_v) = mirrorEnabled(); break;
        case 19: *reinterpret_cast< double*>(_v) = mirrorOffset(); break;
        case 20: *reinterpret_cast< bool*>(_v) = shadowEnabled(); break;
        case 21: *reinterpret_cast< double*>(_v) = shadowBlurRadius(); break;
        case 22: *reinterpret_cast< double*>(_v) = shadowOffsetX(); break;
        case 23: *reinterpret_cast< double*>(_v) = shadowOffsetY(); break;
        case 24: *reinterpret_cast< QBrush*>(_v) = shadowBrush(); break;
        case 25: *reinterpret_cast< bool*>(_v) = zoomEffectEnabled(); break;
        case 26: *reinterpret_cast< int*>(_v) = zoomSpeed(); break;
        case 27: *reinterpret_cast< ZoomAnchorPoint*>(_v) = zoomAnchorPoint(); break;
        case 28: *reinterpret_cast< ZoomEffectDirection*>(_v) = zoomDirection(); break;
        }
        _id -= 29;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPos(*reinterpret_cast< QPointF*>(_v)); break;
        case 1: setContentsRect(*reinterpret_cast< QRectF*>(_v)); break;
        case 2: setSourceOffsetTL(*reinterpret_cast< QPointF*>(_v)); break;
        case 3: setSourceOffsetBR(*reinterpret_cast< QPointF*>(_v)); break;
        case 4: setIsVisible(*reinterpret_cast< bool*>(_v)); break;
        case 5: setZValue(*reinterpret_cast< double*>(_v)); break;
        case 6: setOpacity(*reinterpret_cast< double*>(_v)); break;
        case 7: setXRotation(*reinterpret_cast< double*>(_v)); break;
        case 8: setYRotation(*reinterpret_cast< double*>(_v)); break;
        case 9: setZRotation(*reinterpret_cast< double*>(_v)); break;
        case 10: setFrameClass(*reinterpret_cast< quint32*>(_v)); break;
        case 11: setFillType(*reinterpret_cast< FillType*>(_v)); break;
        case 12: setFillBrush(*reinterpret_cast< QBrush*>(_v)); break;
        case 13: setFillImageFile(*reinterpret_cast< QString*>(_v)); break;
        case 14: setFillVideoFile(*reinterpret_cast< QString*>(_v)); break;
        case 15: setVideoEndAction(*reinterpret_cast< VideoEndAction*>(_v)); break;
        case 16: setOutlineEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 17: setOutlinePen(*reinterpret_cast< QPen*>(_v)); break;
        case 18: setMirrorEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 19: setMirrorOffset(*reinterpret_cast< double*>(_v)); break;
        case 20: setShadowEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 21: setShadowBlurRadius(*reinterpret_cast< double*>(_v)); break;
        case 22: setShadowOffsetX(*reinterpret_cast< double*>(_v)); break;
        case 23: setShadowOffsetY(*reinterpret_cast< double*>(_v)); break;
        case 24: setShadowBrush(*reinterpret_cast< QBrush*>(_v)); break;
        case 25: setZoomEffectEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 26: setZoomSpeed(*reinterpret_cast< int*>(_v)); break;
        case 27: setZoomAnchorPoint(*reinterpret_cast< ZoomAnchorPoint*>(_v)); break;
        case 28: setZoomDirection(*reinterpret_cast< ZoomEffectDirection*>(_v)); break;
        }
        _id -= 29;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 29;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
