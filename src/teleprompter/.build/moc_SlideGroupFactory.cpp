/****************************************************************************
** Meta object code from reading C++ file 'SlideGroupFactory.h'
**
** Created: Sat Dec 19 21:08:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../model/SlideGroupFactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SlideGroupFactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SlideGroupViewControl[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,
      50,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
     103,   72,   22,   22, 0x0a,
     153,  142,   22,   22, 0x2a,
     189,  187,   22,   22, 0x2a,
     216,   22,   22,   22, 0x0a,
     236,   22,   22,   22, 0x0a,
     248,   22,   22,   22, 0x0a,
     260,   22,   22,   22, 0x0a,
     281,   22,   22,   22, 0x0a,
     322,  305,   22,   22, 0x0a,
     362,  356,   22,   22, 0x2a,
     391,   22,   22,   22, 0x2a,
     410,   22,   22,   22, 0x0a,
     436,   22,   22,   22, 0x0a,
     453,   22,   22,   22, 0x0a,
     480,   22,   22,   22, 0x0a,
     501,  496,   22,   22, 0x0a,
     522,   22,   22,   22, 0x2a,
     544,  539,   22,   22, 0x0a,
     571,   22,   22,   22, 0x2a,
     591,   22,   22,   22, 0x09,
     620,  618,   22,   22, 0x09,
     660,   22,   22,   22, 0x09,
     692,   22,   22,   22, 0x09,
     715,  710,   22,   22, 0x09,
     739,   22,   22,   22, 0x29,
     757,   22,   22,   22, 0x08,
     771,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SlideGroupViewControl[] = {
    "SlideGroupViewControl\0\0"
    "slideDoubleClicked(Slide*)\0"
    "slideSelected(Slide*)\0"
    "g,curSlide,allowProgressDialog\0"
    "setSlideGroup(SlideGroup*,Slide*,bool)\0"
    "g,curSlide\0setSlideGroup(SlideGroup*,Slide*)\0"
    "g\0setSlideGroup(SlideGroup*)\0"
    "releaseSlideGroup()\0nextSlide()\0"
    "prevSlide()\0setCurrentSlide(int)\0"
    "setCurrentSlide(Slide*)\0state,resetTimer\0"
    "toggleTimerState(TimerState,bool)\0"
    "state\0toggleTimerState(TimerState)\0"
    "toggleTimerState()\0setIsPreviewControl(bool)\0"
    "setEnabled(bool)\0setQuickSlideEnabled(bool)\0"
    "addQuickSlide()\0flag\0showQuickSlide(bool)\0"
    "showQuickSlide()\0text\0setQuickSlideText(QString)\0"
    "setQuickSlideText()\0slideSelected(QModelIndex)\0"
    ",\0currentChanged(QModelIndex,QModelIndex)\0"
    "slideDoubleClicked(QModelIndex)\0"
    "updateTimeLabel()\0time\0enableAnimation(double)\0"
    "enableAnimation()\0repaintList()\0"
    "slideChanged(Slide*)\0"
};

const QMetaObject SlideGroupViewControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SlideGroupViewControl,
      qt_meta_data_SlideGroupViewControl, 0 }
};

const QMetaObject *SlideGroupViewControl::metaObject() const
{
    return &staticMetaObject;
}

void *SlideGroupViewControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SlideGroupViewControl))
        return static_cast<void*>(const_cast< SlideGroupViewControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int SlideGroupViewControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slideDoubleClicked((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 1: slideSelected((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 2: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 4: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1]))); break;
        case 5: releaseSlideGroup(); break;
        case 6: nextSlide(); break;
        case 7: prevSlide(); break;
        case 8: setCurrentSlide((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: setCurrentSlide((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 10: toggleTimerState((*reinterpret_cast< TimerState(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 11: toggleTimerState((*reinterpret_cast< TimerState(*)>(_a[1]))); break;
        case 12: toggleTimerState(); break;
        case 13: setIsPreviewControl((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: setQuickSlideEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: addQuickSlide(); break;
        case 17: showQuickSlide((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: showQuickSlide(); break;
        case 19: setQuickSlideText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: setQuickSlideText(); break;
        case 21: slideSelected((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 22: currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 23: slideDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 24: updateTimeLabel(); break;
        case 25: enableAnimation((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 26: enableAnimation(); break;
        case 27: repaintList(); break;
        case 28: slideChanged((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void SlideGroupViewControl::slideDoubleClicked(Slide * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SlideGroupViewControl::slideSelected(Slide * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_AbstractSlideGroupEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_AbstractSlideGroupEditor[] = {
    "AbstractSlideGroupEditor\0\0closed()\0"
};

const QMetaObject AbstractSlideGroupEditor::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_AbstractSlideGroupEditor,
      qt_meta_data_AbstractSlideGroupEditor, 0 }
};

const QMetaObject *AbstractSlideGroupEditor::metaObject() const
{
    return &staticMetaObject;
}

void *AbstractSlideGroupEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AbstractSlideGroupEditor))
        return static_cast<void*>(const_cast< AbstractSlideGroupEditor*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int AbstractSlideGroupEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: closed(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void AbstractSlideGroupEditor::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
