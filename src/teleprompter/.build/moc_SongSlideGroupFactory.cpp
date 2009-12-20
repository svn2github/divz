/****************************************************************************
** Meta object code from reading C++ file 'SongSlideGroupFactory.h'
**
** Created: Sat Dec 19 21:08:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../songdb/SongSlideGroupFactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SongSlideGroupFactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SongFoldbackTextFilter[] = {

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

static const char qt_meta_stringdata_SongFoldbackTextFilter[] = {
    "SongFoldbackTextFilter\0"
};

const QMetaObject SongFoldbackTextFilter::staticMetaObject = {
    { &SlideTextOnlyFilter::staticMetaObject, qt_meta_stringdata_SongFoldbackTextFilter,
      qt_meta_data_SongFoldbackTextFilter, 0 }
};

const QMetaObject *SongFoldbackTextFilter::metaObject() const
{
    return &staticMetaObject;
}

void *SongFoldbackTextFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SongFoldbackTextFilter))
        return static_cast<void*>(const_cast< SongFoldbackTextFilter*>(this));
    return SlideTextOnlyFilter::qt_metacast(_clname);
}

int SongFoldbackTextFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SlideTextOnlyFilter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SongSlideGroupViewControl[] = {

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

static const char qt_meta_stringdata_SongSlideGroupViewControl[] = {
    "SongSlideGroupViewControl\0"
};

const QMetaObject SongSlideGroupViewControl::staticMetaObject = {
    { &SlideGroupViewControl::staticMetaObject, qt_meta_stringdata_SongSlideGroupViewControl,
      qt_meta_data_SongSlideGroupViewControl, 0 }
};

const QMetaObject *SongSlideGroupViewControl::metaObject() const
{
    return &staticMetaObject;
}

void *SongSlideGroupViewControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SongSlideGroupViewControl))
        return static_cast<void*>(const_cast< SongSlideGroupViewControl*>(this));
    return SlideGroupViewControl::qt_metacast(_clname);
}

int SongSlideGroupViewControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SlideGroupViewControl::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
