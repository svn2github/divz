/****************************************************************************
** Meta object code from reading C++ file 'OutputControl.h'
**
** Created: Sat Dec 19 21:08:46 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OutputControl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OutputControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OutputControl[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      43,   14,   14,   14, 0x0a,
      78,   14,   14,   14, 0x0a,
     117,   14,   14,   14, 0x0a,
     164,  158,   14,   14, 0x0a,
     187,   14,   14,   14, 0x0a,
     211,   14,   14,   14, 0x0a,
     242,   14,   14,   14, 0x0a,
     272,   14,   14,   14, 0x0a,
     302,   14,   14,   14, 0x0a,
     326,   14,   14,   14, 0x0a,
     357,   14,   14,   14, 0x0a,
     384,   14,   14,   14, 0x0a,
     405,   14,   14,   14, 0x0a,
     430,  426,   14,   14, 0x09,
     453,   14,   14,   14, 0x09,
     485,   14,   14,   14, 0x09,
     508,   14,   14,   14, 0x09,
     531,   14,   14,   14, 0x09,
     550,   14,   14,   14, 0x09,
     573,  568,   14,   14, 0x09,
     613,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_OutputControl[] = {
    "OutputControl\0\0outputIsSyncedChanged(bool)\0"
    "setOutputInstance(OutputInstance*)\0"
    "setViewControl(SlideGroupViewControl*)\0"
    "setCustomFilters(AbstractItemFilterList)\0"
    "value\0setCrossFadeSpeed(int)\0"
    "setIsOutputSynced(bool)\0"
    "setSyncSource(OutputInstance*)\0"
    "setTextOnlyBackground(QColor)\0"
    "setOverlayDocument(Document*)\0"
    "setOverlayEnabled(bool)\0"
    "setTextOnlyFilterEnabled(bool)\0"
    "setTextResizeEnabled(bool)\0"
    "fadeBlackFrame(bool)\0fadeClearFrame(bool)\0"
    "idx\0syncOutputChanged(int)\0"
    "overlayDocumentChanged(QString)\0"
    "overlayBrowseClicked()\0setIsOutputSynced(int)\0"
    "setupSyncWithBox()\0slideChanged(int)\0"
    "item\0filterListItemChanged(QListWidgetItem*)\0"
    "setAdvancedWidgetVisible(bool)\0"
};

const QMetaObject OutputControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_OutputControl,
      qt_meta_data_OutputControl, 0 }
};

const QMetaObject *OutputControl::metaObject() const
{
    return &staticMetaObject;
}

void *OutputControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputControl))
        return static_cast<void*>(const_cast< OutputControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int OutputControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: outputIsSyncedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: setOutputInstance((*reinterpret_cast< OutputInstance*(*)>(_a[1]))); break;
        case 2: setViewControl((*reinterpret_cast< SlideGroupViewControl*(*)>(_a[1]))); break;
        case 3: setCustomFilters((*reinterpret_cast< AbstractItemFilterList(*)>(_a[1]))); break;
        case 4: setCrossFadeSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: setIsOutputSynced((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: setSyncSource((*reinterpret_cast< OutputInstance*(*)>(_a[1]))); break;
        case 7: setTextOnlyBackground((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 8: setOverlayDocument((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 9: setOverlayEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: setTextOnlyFilterEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: setTextResizeEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: fadeBlackFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: fadeClearFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: syncOutputChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: overlayDocumentChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: overlayBrowseClicked(); break;
        case 17: setIsOutputSynced((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: setupSyncWithBox(); break;
        case 19: slideChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: filterListItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 21: setAdvancedWidgetVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void OutputControl::outputIsSyncedChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
