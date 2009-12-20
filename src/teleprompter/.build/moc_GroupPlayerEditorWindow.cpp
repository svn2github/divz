/****************************************************************************
** Meta object code from reading C++ file 'GroupPlayerEditorWindow.h'
**
** Created: Sat Dec 19 21:08:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../groupplayer/GroupPlayerEditorWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GroupPlayerEditorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GroupPlayerEditorWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      35,   25,   24,   24, 0x0a,
      69,   24,   24,   24, 0x2a,
      96,   24,   24,   24, 0x0a,
     107,   24,   24,   24, 0x0a,
     119,   24,   24,   24, 0x0a,
     141,   24,   24,   24, 0x0a,
     156,   24,   24,   24, 0x0a,
     170,   24,   24,   24, 0x0a,
     189,   24,   24,   24, 0x0a,
     201,   24,   24,   24, 0x0a,
     215,   24,   24,   24, 0x0a,
     227,   24,   24,   24, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GroupPlayerEditorWindow[] = {
    "GroupPlayerEditorWindow\0\0,curSlide\0"
    "setSlideGroup(SlideGroup*,Slide*)\0"
    "setSlideGroup(SlideGroup*)\0accepted()\0"
    "browseDoc()\0loadOtherDoc(QString)\0"
    "showOtherDoc()\0showThisDoc()\0"
    "docReturnPressed()\0btnMoveUp()\0"
    "btnMoveDown()\0btnRemove()\0btnAdd()\0"
};

const QMetaObject GroupPlayerEditorWindow::staticMetaObject = {
    { &AbstractSlideGroupEditor::staticMetaObject, qt_meta_stringdata_GroupPlayerEditorWindow,
      qt_meta_data_GroupPlayerEditorWindow, 0 }
};

const QMetaObject *GroupPlayerEditorWindow::metaObject() const
{
    return &staticMetaObject;
}

void *GroupPlayerEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GroupPlayerEditorWindow))
        return static_cast<void*>(const_cast< GroupPlayerEditorWindow*>(this));
    return AbstractSlideGroupEditor::qt_metacast(_clname);
}

int GroupPlayerEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractSlideGroupEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 1: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1]))); break;
        case 2: accepted(); break;
        case 3: browseDoc(); break;
        case 4: loadOtherDoc((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: showOtherDoc(); break;
        case 6: showThisDoc(); break;
        case 7: docReturnPressed(); break;
        case 8: btnMoveUp(); break;
        case 9: btnMoveDown(); break;
        case 10: btnRemove(); break;
        case 11: btnAdd(); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
