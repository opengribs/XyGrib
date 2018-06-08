/****************************************************************************
** Meta object code from reading C++ file 'qwt_thermo.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_thermo.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_thermo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QwtThermo_t {
    QByteArrayData data[23];
    char stringdata0[245];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QwtThermo_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QwtThermo_t qt_meta_stringdata_QwtThermo = {
    {
QT_MOC_LITERAL(0, 0, 9), // "QwtThermo"
QT_MOC_LITERAL(1, 10, 8), // "setValue"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 3), // "val"
QT_MOC_LITERAL(4, 24, 11), // "orientation"
QT_MOC_LITERAL(5, 36, 15), // "Qt::Orientation"
QT_MOC_LITERAL(6, 52, 13), // "scalePosition"
QT_MOC_LITERAL(7, 66, 13), // "ScalePosition"
QT_MOC_LITERAL(8, 80, 10), // "originMode"
QT_MOC_LITERAL(9, 91, 10), // "OriginMode"
QT_MOC_LITERAL(10, 102, 12), // "alarmEnabled"
QT_MOC_LITERAL(11, 115, 10), // "alarmLevel"
QT_MOC_LITERAL(12, 126, 6), // "origin"
QT_MOC_LITERAL(13, 133, 7), // "spacing"
QT_MOC_LITERAL(14, 141, 11), // "borderWidth"
QT_MOC_LITERAL(15, 153, 9), // "pipeWidth"
QT_MOC_LITERAL(16, 163, 5), // "value"
QT_MOC_LITERAL(17, 169, 7), // "NoScale"
QT_MOC_LITERAL(18, 177, 12), // "LeadingScale"
QT_MOC_LITERAL(19, 190, 13), // "TrailingScale"
QT_MOC_LITERAL(20, 204, 13), // "OriginMinimum"
QT_MOC_LITERAL(21, 218, 13), // "OriginMaximum"
QT_MOC_LITERAL(22, 232, 12) // "OriginCustom"

    },
    "QwtThermo\0setValue\0\0val\0orientation\0"
    "Qt::Orientation\0scalePosition\0"
    "ScalePosition\0originMode\0OriginMode\0"
    "alarmEnabled\0alarmLevel\0origin\0spacing\0"
    "borderWidth\0pipeWidth\0value\0NoScale\0"
    "LeadingScale\0TrailingScale\0OriginMinimum\0"
    "OriginMaximum\0OriginCustom"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QwtThermo[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      10,   22, // properties
       2,   52, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,

 // properties: name, type, flags
       4, 0x80000000 | 5, 0x0009510b,
       6, 0x80000000 | 7, 0x0009510b,
       8, 0x80000000 | 9, 0x0009510b,
      10, QMetaType::Bool, 0x00095103,
      11, QMetaType::Double, 0x00095103,
      12, QMetaType::Double, 0x00095103,
      13, QMetaType::Int, 0x00095103,
      14, QMetaType::Int, 0x00095103,
      15, QMetaType::Int, 0x00095103,
      16, QMetaType::Double, 0x00095103,

 // enums: name, flags, count, data
       7, 0x0,    3,   60,
       9, 0x0,    3,   66,

 // enum data: key, value
      17, uint(QwtThermo::NoScale),
      18, uint(QwtThermo::LeadingScale),
      19, uint(QwtThermo::TrailingScale),
      20, uint(QwtThermo::OriginMinimum),
      21, uint(QwtThermo::OriginMaximum),
      22, uint(QwtThermo::OriginCustom),

       0        // eod
};

void QwtThermo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QwtThermo *_t = static_cast<QwtThermo *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QwtThermo *_t = static_cast<QwtThermo *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::Orientation*>(_v) = _t->orientation(); break;
        case 1: *reinterpret_cast< ScalePosition*>(_v) = _t->scalePosition(); break;
        case 2: *reinterpret_cast< OriginMode*>(_v) = _t->originMode(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->alarmEnabled(); break;
        case 4: *reinterpret_cast< double*>(_v) = _t->alarmLevel(); break;
        case 5: *reinterpret_cast< double*>(_v) = _t->origin(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->spacing(); break;
        case 7: *reinterpret_cast< int*>(_v) = _t->borderWidth(); break;
        case 8: *reinterpret_cast< int*>(_v) = _t->pipeWidth(); break;
        case 9: *reinterpret_cast< double*>(_v) = _t->value(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QwtThermo *_t = static_cast<QwtThermo *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 1: _t->setScalePosition(*reinterpret_cast< ScalePosition*>(_v)); break;
        case 2: _t->setOriginMode(*reinterpret_cast< OriginMode*>(_v)); break;
        case 3: _t->setAlarmEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 4: _t->setAlarmLevel(*reinterpret_cast< double*>(_v)); break;
        case 5: _t->setOrigin(*reinterpret_cast< double*>(_v)); break;
        case 6: _t->setSpacing(*reinterpret_cast< int*>(_v)); break;
        case 7: _t->setBorderWidth(*reinterpret_cast< int*>(_v)); break;
        case 8: _t->setPipeWidth(*reinterpret_cast< int*>(_v)); break;
        case 9: _t->setValue(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject QwtThermo::staticMetaObject = {
    { &QwtAbstractScale::staticMetaObject, qt_meta_stringdata_QwtThermo.data,
      qt_meta_data_QwtThermo,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QwtThermo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtThermo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QwtThermo.stringdata0))
        return static_cast<void*>(const_cast< QwtThermo*>(this));
    return QwtAbstractScale::qt_metacast(_clname);
}

int QwtThermo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtAbstractScale::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
