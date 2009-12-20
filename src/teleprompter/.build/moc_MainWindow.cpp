/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created: Sat Dec 19 22:04:24 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      42,   11,   11,   11, 0x05,
      69,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      90,   11,   11,   11, 0x09,
     101,   11,   11,   11, 0x09,
     112,   11,   11,   11, 0x09,
     125,   11,   11,   11, 0x09,
     143,   11,   11,   11, 0x09,
     154,   11,   11,   11, 0x09,
     171,   11,   11,   11, 0x09,
     194,   11,   11,   11, 0x09,
     219,   11,   11,   11, 0x09,
     236,   11,   11,   11, 0x09,
     258,   11,   11,   11, 0x09,
     268,   11,   11,   11, 0x09,
     283,   11,   11,   11, 0x09,
     301,   11,   11,   11, 0x09,
     323,   11,   11,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0docSettingsChanged(Document*)\0"
    "aspectRatioChanged(double)\0"
    "appSettingsChanged()\0slotOpen()\0"
    "slotSave()\0slotSaveAs()\0slotOutputSetup()\0"
    "slotExit()\0slotTogglePlay()\0"
    "slotPosBoxChanged(int)\0slotAccelBoxChanged(int)\0"
    "slotResetAccel()\0aspectChanged(double)\0"
    "animate()\0setPos(double)\0slotTextChanged()\0"
    "saveTextFile(QString)\0openTextFile(QString)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: docSettingsChanged((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 1: aspectRatioChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: appSettingsChanged(); break;
        case 3: slotOpen(); break;
        case 4: slotSave(); break;
        case 5: slotSaveAs(); break;
        case 6: slotOutputSetup(); break;
        case 7: slotExit(); break;
        case 8: slotTogglePlay(); break;
        case 9: slotPosBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: slotAccelBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: slotResetAccel(); break;
        case 12: aspectChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: animate(); break;
        case 14: setPos((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: slotTextChanged(); break;
        case 16: saveTextFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: openTextFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::docSettingsChanged(Document * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::aspectRatioChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainWindow::appSettingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
