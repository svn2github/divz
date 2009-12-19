/****************************************************************************
** Meta object code from reading C++ file 'GroupPlayerSlideGroupFactory.h'
**
** Created: Sat Dec 19 18:15:34 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../groupplayer/GroupPlayerSlideGroupFactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GroupPlayerSlideGroupFactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GroupPlayerSlideGroupViewControl[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      65,   34,   33,   33, 0x0a,
     115,  104,   33,   33, 0x2a,
     151,  149,   33,   33, 0x2a,
     178,   33,   33,   33, 0x0a,
     198,   33,   33,   33, 0x0a,
     210,   33,   33,   33, 0x0a,
     222,   33,   33,   33, 0x0a,
     243,   33,   33,   33, 0x0a,
     284,  267,   33,   33, 0x0a,
     324,  318,   33,   33, 0x2a,
     353,   33,   33,   33, 0x2a,
     372,   33,   33,   33, 0x0a,
     393,   33,   33,   33, 0x0a,
     414,   33,   33,   33, 0x0a,
     440,   33,   33,   33, 0x0a,
     457,   33,   33,   33, 0x0a,
     484,   33,   33,   33, 0x0a,
     505,  500,   33,   33, 0x0a,
     526,   33,   33,   33, 0x2a,
     548,  543,   33,   33, 0x0a,
     575,   33,   33,   33, 0x2a,
     595,   33,   33,   33, 0x0a,
     648,   33,   33,   33, 0x09,
     661,   33,   33,   33, 0x09,
     696,  692,   33,   33, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GroupPlayerSlideGroupViewControl[] = {
    "GroupPlayerSlideGroupViewControl\0\0"
    "g,curSlide,allowProgressDialog\0"
    "setSlideGroup(SlideGroup*,Slide*,bool)\0"
    "g,curSlide\0setSlideGroup(SlideGroup*,Slide*)\0"
    "g\0setSlideGroup(SlideGroup*)\0"
    "releaseSlideGroup()\0nextSlide()\0"
    "prevSlide()\0setCurrentSlide(int)\0"
    "setCurrentSlide(Slide*)\0state,resetTimer\0"
    "toggleTimerState(TimerState,bool)\0"
    "state\0toggleTimerState(TimerState)\0"
    "toggleTimerState()\0fadeBlackFrame(bool)\0"
    "fadeClearFrame(bool)\0setIsPreviewControl(bool)\0"
    "setEnabled(bool)\0setQuickSlideEnabled(bool)\0"
    "addQuickSlide()\0flag\0showQuickSlide(bool)\0"
    "showQuickSlide()\0text\0setQuickSlideText(QString)\0"
    "setQuickSlideText()\0"
    "setCurrentMember(GroupPlayerSlideGroup::GroupMember)\0"
    "endOfGroup()\0itemSelected(QListWidgetItem*)\0"
    "mem\0addMemberToList(GroupPlayerSlideGroup::GroupMember)\0"
};

const QMetaObject GroupPlayerSlideGroupViewControl::staticMetaObject = {
    { &SlideGroupViewControl::staticMetaObject, qt_meta_stringdata_GroupPlayerSlideGroupViewControl,
      qt_meta_data_GroupPlayerSlideGroupViewControl, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GroupPlayerSlideGroupViewControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GroupPlayerSlideGroupViewControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GroupPlayerSlideGroupViewControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GroupPlayerSlideGroupViewControl))
        return static_cast<void*>(const_cast< GroupPlayerSlideGroupViewControl*>(this));
    return SlideGroupViewControl::qt_metacast(_clname);
}

int GroupPlayerSlideGroupViewControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SlideGroupViewControl::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 2: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1]))); break;
        case 3: releaseSlideGroup(); break;
        case 4: nextSlide(); break;
        case 5: prevSlide(); break;
        case 6: setCurrentSlide((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: setCurrentSlide((*reinterpret_cast< Slide*(*)>(_a[1]))); break;
        case 8: toggleTimerState((*reinterpret_cast< TimerState(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: toggleTimerState((*reinterpret_cast< TimerState(*)>(_a[1]))); break;
        case 10: toggleTimerState(); break;
        case 11: fadeBlackFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: fadeClearFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: setIsPreviewControl((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: setQuickSlideEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: addQuickSlide(); break;
        case 17: showQuickSlide((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: showQuickSlide(); break;
        case 19: setQuickSlideText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: setQuickSlideText(); break;
        case 21: setCurrentMember((*reinterpret_cast< GroupPlayerSlideGroup::GroupMember(*)>(_a[1]))); break;
        case 22: endOfGroup(); break;
        case 23: itemSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 24: addMemberToList((*reinterpret_cast< GroupPlayerSlideGroup::GroupMember(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 25;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
