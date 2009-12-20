/****************************************************************************
** Meta object code from reading C++ file 'SongEditorWindow.h'
**
** Created: Sat Dec 19 21:08:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../songdb/SongEditorWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SongEditorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SongEditorHighlighter[] = {

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

static const char qt_meta_stringdata_SongEditorHighlighter[] = {
    "SongEditorHighlighter\0"
};

const QMetaObject SongEditorHighlighter::staticMetaObject = {
    { &QSyntaxHighlighter::staticMetaObject, qt_meta_stringdata_SongEditorHighlighter,
      qt_meta_data_SongEditorHighlighter, 0 }
};

const QMetaObject *SongEditorHighlighter::metaObject() const
{
    return &staticMetaObject;
}

void *SongEditorHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SongEditorHighlighter))
        return static_cast<void*>(const_cast< SongEditorHighlighter*>(this));
    return QSyntaxHighlighter::qt_metacast(_clname);
}

int SongEditorHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSyntaxHighlighter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SongEditorWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      43,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      66,   55,   17,   17, 0x0a,
     102,  100,   17,   17, 0x2a,
     146,  129,   17,   17, 0x0a,
     178,   17,   17,   17, 0x0a,
     189,   17,   17,   17, 0x0a,
     208,   17,   17,   17, 0x0a,
     232,   17,   17,   17, 0x0a,
     253,   17,   17,   17, 0x0a,
     275,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SongEditorWindow[] = {
    "SongEditorWindow\0\0songCreated(SongRecord*)\0"
    "songSaved()\0g,curSlide\0"
    "setSlideGroup(SlideGroup*,Slide*)\0g\0"
    "setSlideGroup(SlideGroup*)\0g,syncToDatabase\0"
    "setSlideGroup(SlideGroup*,bool)\0"
    "accepted()\0editSongTemplate()\0"
    "setSyncToDatabase(bool)\0showSyncOption(bool)\0"
    "isSyncOptionVisible()\0editorWindowClosed()\0"
};

const QMetaObject SongEditorWindow::staticMetaObject = {
    { &AbstractSlideGroupEditor::staticMetaObject, qt_meta_stringdata_SongEditorWindow,
      qt_meta_data_SongEditorWindow, 0 }
};

const QMetaObject *SongEditorWindow::metaObject() const
{
    return &staticMetaObject;
}

void *SongEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SongEditorWindow))
        return static_cast<void*>(const_cast< SongEditorWindow*>(this));
    return AbstractSlideGroupEditor::qt_metacast(_clname);
}

int SongEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractSlideGroupEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: songCreated((*reinterpret_cast< SongRecord*(*)>(_a[1]))); break;
        case 1: songSaved(); break;
        case 2: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< Slide*(*)>(_a[2]))); break;
        case 3: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1]))); break;
        case 4: setSlideGroup((*reinterpret_cast< SlideGroup*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: accepted(); break;
        case 6: editSongTemplate(); break;
        case 7: setSyncToDatabase((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: showSyncOption((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: isSyncOptionVisible(); break;
        case 10: editorWindowClosed(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void SongEditorWindow::songCreated(SongRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SongEditorWindow::songSaved()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
