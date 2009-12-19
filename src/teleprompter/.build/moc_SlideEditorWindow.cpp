/****************************************************************************
** Meta object code from reading C++ file 'SlideEditorWindow.h'
**
** Created: Sat Dec 19 18:15:34 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SlideEditorWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SlideEditorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SlideEditorWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      50,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      39,   28,   18,   18, 0x0a,
      75,   73,   18,   18, 0x2a,
     108,  102,   18,   18, 0x0a,
     139,   18,  132,   18, 0x0a,
     151,   18,  132,   18, 0x0a,
     163,   18,   18,   18, 0x0a,
     187,   18,   18,   18, 0x09,
     219,  214,   18,   18, 0x09,
     259,   18,   18,   18, 0x09,
     285,  214,   18,   18, 0x09,
     329,   18,   18,   18, 0x09,
     365,   18,   18,   18, 0x08,
     379,   18,   18,   18, 0x08,
     392,   18,   18,   18, 0x08,
     407,   18,   18,   18, 0x08,
     422,   18,   18,   18, 0x08,
     438,   18,   18,   18, 0x08,
     449,   18,   18,   18, 0x08,
     460,   18,   18,   18, 0x08,
     478,   18,   18,   18, 0x08,
     489,   18,   18,   18, 0x08,
     507,   18,   18,   18, 0x08,
     528,   18,   18,   18, 0x08,
     539,   18,   18,   18, 0x08,
     551,   18,   18,   18, 0x08,
     567,   18,   18,   18, 0x08,
     584,   18,   18,   18, 0x08,
     601,   18,   18,   18, 0x08,
     619,   18,   18,   18, 0x08,
     645,   18,   18,   18, 0x08,
     661,   18,   18,   18, 0x08,
     677,   18,   18,   18, 0x08,
     694,   18,   18,   18, 0x08,
     717,   18,   18,   18, 0x08,
     741,   18,   18,   18, 0x08,
     760,   18,   18,   18, 0x08,
     780,   18,   18,   18, 0x08,
     801,   18,   18,   18, 0x08,
     825,   18,   18,   18, 0x08,
     854,   18,   18,   18, 0x08,
     889,   18,   18,   18, 0x08,
     910,   18,   18,   18, 0x08,
     989,  937,   18,   18, 0x08,
    1092, 1057,   18,   18, 0x08,
    1158,   18,   18,   18, 0x08,
    1178,   18,   18,   18, 0x08,
    1197,   18,   18,   18, 0x08,
    1219,   18,   18,   18, 0x08,
    1239,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SlideEditorWindow[] = {
    "SlideEditorWindow\0\0closed()\0g,curSlide\0"
    "setSlideGroup(SlideGroup*,Slide*)\0g\0"
    "setSlideGroup(SlideGroup*)\0slide\0"
    "setCurrentSlide(Slide*)\0Slide*\0"
    "nextSlide()\0prevSlide()\0ignoreUndoChanged(bool)\0"
    "slideSelected(QModelIndex)\0idx,\0"
    "currentChanged(QModelIndex,QModelIndex)\0"
    "itemSelected(QModelIndex)\0"
    "currentItemChanged(QModelIndex,QModelIndex)\0"
    "itemDoubleClicked(AbstractContent*)\0"
    "newTextItem()\0newBoxItem()\0newVideoItem()\0"
    "newImageItem()\0newOutputView()\0"
    "newSlide()\0delSlide()\0slideProperties()\0"
    "dupSlide()\0groupProperties()\0"
    "textSizeBoxChanged()\0textPlus()\0"
    "textMinus()\0textFitToRect()\0"
    "textNaturalBox()\0textFitToSlide()\0"
    "editMasterSlide()\0masterSlideEditorClosed()\0"
    "centerSelVert()\0centerSelHorz()\0"
    "slotConfigGrid()\0slotConfigBackground()\0"
    "setSlideTimeout(double)\0zeroSlideTimeout()\0"
    "guessSlideTimeout()\0setInheritFade(bool)\0"
    "setFadeSpeedPreset(int)\0"
    "slidesDropped(QList<Slide*>)\0"
    "itemsDropped(QList<AbstractItem*>)\0"
    "appSettingsChanged()\0aspectRatioChanged(double)\0"
    "slide,slideOperation,item,operation,fieldName,value\0"
    "slideChanged(Slide*,QString,AbstractItem*,QString,QString,QVariant)\0"
    "item,operation,fieldName,value,old\0"
    "slideItemChanged(AbstractItem*,QString,QString,QVariant,QVariant)\0"
    "releaseSlideGroup()\0selectionChanged()\0"
    "setCurrentSlideLive()\0setAutosaveOn(bool)\0"
    "repaintSlideList()\0"
};

const QMetaObject SlideEditorWindow::staticMetaObject = {
    { &AbstractSlideGroupEditor::staticMetaObject, qt_meta_stringdata_SlideEditorWindow,
      qt_meta_data_SlideEditorWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SlideEditorWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SlideEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SlideEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SlideEditorWindow))
        return static_cast<void*>(const_cast< SlideEditorWindow*>(this));
    return AbstractSlideGroupEditor::qt_metacast(_clname);
}

int SlideEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractSlideGroupEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: closed(); break;
        case 1: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 2: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1]))); break;
        case 3: setCurrentSlide((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 4: { Slide* _r = nextSlide();
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 5: { Slide* _r = prevSlide();
            if (_a[0]) *reinterpret_cast< Slide**>(_a[0]) = _r; }  break;
        case 6: ignoreUndoChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: slideSelected((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 9: itemSelected((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 10: currentItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 11: itemDoubleClicked((*reinterpret_cast< AbstractContent*(*)>(_a[1]))); break;
        case 12: newTextItem(); break;
        case 13: newBoxItem(); break;
        case 14: newVideoItem(); break;
        case 15: newImageItem(); break;
        case 16: newOutputView(); break;
        case 17: newSlide(); break;
        case 18: delSlide(); break;
        case 19: slideProperties(); break;
        case 20: dupSlide(); break;
        case 21: groupProperties(); break;
        case 22: textSizeBoxChanged(); break;
        case 23: textPlus(); break;
        case 24: textMinus(); break;
        case 25: textFitToRect(); break;
        case 26: textNaturalBox(); break;
        case 27: textFitToSlide(); break;
        case 28: editMasterSlide(); break;
        case 29: masterSlideEditorClosed(); break;
        case 30: centerSelVert(); break;
        case 31: centerSelHorz(); break;
        case 32: slotConfigGrid(); break;
        case 33: slotConfigBackground(); break;
        case 34: setSlideTimeout((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 35: zeroSlideTimeout(); break;
        case 36: guessSlideTimeout(); break;
        case 37: setInheritFade((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 38: setFadeSpeedPreset((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 39: slidesDropped((*reinterpret_cast< QList<Slide*>(*)>(_a[1]))); break;
        case 40: itemsDropped((*reinterpret_cast< QList<AbstractItem*>(*)>(_a[1]))); break;
        case 41: appSettingsChanged(); break;
        case 42: aspectRatioChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 43: slideChanged((*reinterpret_cast< Slide*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< AbstractItem*(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QVariant(*)>(_a[6]))); break;
        case 44: slideItemChanged((*reinterpret_cast< AbstractItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QVariant(*)>(_a[4])),(*reinterpret_cast< QVariant(*)>(_a[5]))); break;
        case 45: releaseSlideGroup(); break;
        case 46: selectionChanged(); break;
        case 47: setCurrentSlideLive(); break;
        case 48: setAutosaveOn((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 49: repaintSlideList(); break;
        default: ;
        }
        _id -= 50;
    }
    return _id;
}

// SIGNAL 0
void SlideEditorWindow::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
