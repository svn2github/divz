/****************************************************************************
** Meta object code from reading C++ file 'analyzerbase.h'
**
** Created: Sat Dec 19 18:15:33 2009
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../3rdparty/analyzers/analyzerbase.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'analyzerbase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Analyzer__Base[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x09,
      27,   15,   15,   15, 0x09,
      38,   15,   15,   15, 0x09,
      49,   15,   15,   15, 0x09,
      60,   15,   15,   15, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_Analyzer__Base[] = {
    "Analyzer::Base\0\0set50fps()\0set33fps()\0"
    "set25fps()\0set20fps()\0set10fps()\0"
};

const QMetaObject Analyzer::Base::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Analyzer__Base,
      qt_meta_data_Analyzer__Base, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Analyzer::Base::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Analyzer::Base::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Analyzer::Base::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Analyzer__Base))
        return static_cast<void*>(const_cast< Base*>(this));
    return QWidget::qt_metacast(_clname);
}

int Analyzer::Base::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: set50fps(); break;
        case 1: set33fps(); break;
        case 2: set25fps(); break;
        case 3: set20fps(); break;
        case 4: set10fps(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
