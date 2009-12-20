/****************************************************************************
** Meta object code from reading C++ file 'Output.h'
**
** Created: Sat Dec 19 21:08:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../model/Output.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Output.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Output[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      15,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      12,    7, 0x01095103,
      21,    7, 0x01095103,
      39,   31, 0x0a095103,
      55,   44, 0x0009510b,
      70,   66, 0x02095103,
      86,   80, 0x13095103,
     109,   97, 0x0009510b,
     121,   31, 0x0a095103,
     126,   66, 0x02095103,
     131,    7, 0x01095103,
     145,   31, 0x0a095103,
     150,    7, 0x01095103,
     160,    7, 0x01095103,
     179,   66, 0x02095103,
     195,   66, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_Output[] = {
    "Output\0bool\0isSystem\0isEnabled\0QString\0"
    "name\0OutputType\0outputType\0int\0screenNum\0"
    "QRect\0customRect\0NetworkRole\0networkRole\0"
    "host\0port\0allowMultiple\0tags\0stayOnTop\0"
    "mjpegServerEnabled\0mjpegServerPort\0"
    "mjpegServerFPS\0"
};

const QMetaObject Output::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Output,
      qt_meta_data_Output, 0 }
};

const QMetaObject *Output::metaObject() const
{
    return &staticMetaObject;
}

void *Output::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Output))
        return static_cast<void*>(const_cast< Output*>(this));
    return QObject::qt_metacast(_clname);
}

int Output::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isSystem(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isEnabled(); break;
        case 2: *reinterpret_cast< QString*>(_v) = name(); break;
        case 3: *reinterpret_cast< OutputType*>(_v) = outputType(); break;
        case 4: *reinterpret_cast< int*>(_v) = screenNum(); break;
        case 5: *reinterpret_cast< QRect*>(_v) = customRect(); break;
        case 6: *reinterpret_cast< NetworkRole*>(_v) = networkRole(); break;
        case 7: *reinterpret_cast< QString*>(_v) = host(); break;
        case 8: *reinterpret_cast< int*>(_v) = port(); break;
        case 9: *reinterpret_cast< bool*>(_v) = allowMultiple(); break;
        case 10: *reinterpret_cast< QString*>(_v) = tags(); break;
        case 11: *reinterpret_cast< bool*>(_v) = stayOnTop(); break;
        case 12: *reinterpret_cast< bool*>(_v) = mjpegServerEnabled(); break;
        case 13: *reinterpret_cast< int*>(_v) = mjpegServerPort(); break;
        case 14: *reinterpret_cast< int*>(_v) = mjpegServerFPS(); break;
        }
        _id -= 15;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setIsSystem(*reinterpret_cast< bool*>(_v)); break;
        case 1: setIsEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 2: setName(*reinterpret_cast< QString*>(_v)); break;
        case 3: setOutputType(*reinterpret_cast< OutputType*>(_v)); break;
        case 4: setScreenNum(*reinterpret_cast< int*>(_v)); break;
        case 5: setCustomRect(*reinterpret_cast< QRect*>(_v)); break;
        case 6: setNetworkRole(*reinterpret_cast< NetworkRole*>(_v)); break;
        case 7: setHost(*reinterpret_cast< QString*>(_v)); break;
        case 8: setPort(*reinterpret_cast< int*>(_v)); break;
        case 9: setAllowMultiple(*reinterpret_cast< bool*>(_v)); break;
        case 10: setTags(*reinterpret_cast< QString*>(_v)); break;
        case 11: setStayOnTop(*reinterpret_cast< bool*>(_v)); break;
        case 12: setMjpegServerEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 13: setMjpegServerPort(*reinterpret_cast< int*>(_v)); break;
        case 14: setMjpegServerFPS(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 15;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 15;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
