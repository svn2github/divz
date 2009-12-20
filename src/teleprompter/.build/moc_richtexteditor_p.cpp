/****************************************************************************
** Meta object code from reading C++ file 'richtexteditor_p.h'
**
** Created: Sat Dec 19 22:04:23 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../3rdparty/richtextedit/richtexteditor_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'richtexteditor_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RichTextEditorDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   40,   21,   21, 0x08,
      70,   21,   21,   21, 0x08,
      88,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_RichTextEditorDialog[] = {
    "RichTextEditorDialog\0\0contentsChanged()\0"
    "newIndex\0tabIndexChanged(int)\0"
    "richTextChanged()\0sourceChanged()\0"
};

const QMetaObject RichTextEditorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_RichTextEditorDialog,
      qt_meta_data_RichTextEditorDialog, 0 }
};

const QMetaObject *RichTextEditorDialog::metaObject() const
{
    return &staticMetaObject;
}

void *RichTextEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RichTextEditorDialog))
        return static_cast<void*>(const_cast< RichTextEditorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int RichTextEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contentsChanged(); break;
        case 1: tabIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: richTextChanged(); break;
        case 3: sourceChanged(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void RichTextEditorDialog::contentsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_RichTextEditorWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   40,   21,   21, 0x08,
      70,   21,   21,   21, 0x08,
      88,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_RichTextEditorWidget[] = {
    "RichTextEditorWidget\0\0contentsChanged()\0"
    "newIndex\0tabIndexChanged(int)\0"
    "richTextChanged()\0sourceChanged()\0"
};

const QMetaObject RichTextEditorWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RichTextEditorWidget,
      qt_meta_data_RichTextEditorWidget, 0 }
};

const QMetaObject *RichTextEditorWidget::metaObject() const
{
    return &staticMetaObject;
}

void *RichTextEditorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RichTextEditorWidget))
        return static_cast<void*>(const_cast< RichTextEditorWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int RichTextEditorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contentsChanged(); break;
        case 1: tabIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: richTextChanged(); break;
        case 3: sourceChanged(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void RichTextEditorWidget::contentsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
