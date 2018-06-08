/****************************************************************************
** Meta object code from reading C++ file 'qwt_counter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_counter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_counter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QwtCounter_t {
    QByteArrayData data[18];
    char stringdata0[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QwtCounter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QwtCounter_t qt_meta_stringdata_QwtCounter = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QwtCounter"
QT_MOC_LITERAL(1, 11, 14), // "buttonReleased"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 5), // "value"
QT_MOC_LITERAL(4, 33, 12), // "valueChanged"
QT_MOC_LITERAL(5, 46, 8), // "setValue"
QT_MOC_LITERAL(6, 55, 11), // "btnReleased"
QT_MOC_LITERAL(7, 67, 10), // "btnClicked"
QT_MOC_LITERAL(8, 78, 11), // "textChanged"
QT_MOC_LITERAL(9, 90, 7), // "minimum"
QT_MOC_LITERAL(10, 98, 7), // "maximum"
QT_MOC_LITERAL(11, 106, 10), // "singleStep"
QT_MOC_LITERAL(12, 117, 10), // "numButtons"
QT_MOC_LITERAL(13, 128, 11), // "stepButton1"
QT_MOC_LITERAL(14, 140, 11), // "stepButton2"
QT_MOC_LITERAL(15, 152, 11), // "stepButton3"
QT_MOC_LITERAL(16, 164, 8), // "readOnly"
QT_MOC_LITERAL(17, 173, 8) // "wrapping"

    },
    "QwtCounter\0buttonReleased\0\0value\0"
    "valueChanged\0setValue\0btnReleased\0"
    "btnClicked\0textChanged\0minimum\0maximum\0"
    "singleStep\0numButtons\0stepButton1\0"
    "stepButton2\0stepButton3\0readOnly\0"
    "wrapping"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QwtCounter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
      10,   56, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    1,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   50,    2, 0x0a /* Public */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::Double, 0x00095103,
       9, QMetaType::Double, 0x00095103,
      10, QMetaType::Double, 0x00095103,
      11, QMetaType::Double, 0x00095103,
      12, QMetaType::Int, 0x00095103,
      13, QMetaType::Int, 0x00095103,
      14, QMetaType::Int, 0x00095103,
      15, QMetaType::Int, 0x00095103,
      16, QMetaType::Bool, 0x00095103,
      17, QMetaType::Bool, 0x00095103,

       0        // eod
};

void QwtCounter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QwtCounter *_t = static_cast<QwtCounter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->buttonReleased((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->setValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->btnReleased(); break;
        case 4: _t->btnClicked(); break;
        case 5: _t->textChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QwtCounter::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QwtCounter::buttonReleased)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QwtCounter::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QwtCounter::valueChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QwtCounter *_t = static_cast<QwtCounter *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->value(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->minimum(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->maximum(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->singleStep(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->numButtons(); break;
        case 5: *reinterpret_cast< int*>(_v) = _t->stepButton1(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->stepButton2(); break;
        case 7: *reinterpret_cast< int*>(_v) = _t->stepButton3(); break;
        case 8: *reinterpret_cast< bool*>(_v) = _t->isReadOnly(); break;
        case 9: *reinterpret_cast< bool*>(_v) = _t->wrapping(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QwtCounter *_t = static_cast<QwtCounter *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setValue(*reinterpret_cast< double*>(_v)); break;
        case 1: _t->setMinimum(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setMaximum(*reinterpret_cast< double*>(_v)); break;
        case 3: _t->setSingleStep(*reinterpret_cast< double*>(_v)); break;
        case 4: _t->setNumButtons(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setStepButton1(*reinterpret_cast< int*>(_v)); break;
        case 6: _t->setStepButton2(*reinterpret_cast< int*>(_v)); break;
        case 7: _t->setStepButton3(*reinterpret_cast< int*>(_v)); break;
        case 8: _t->setReadOnly(*reinterpret_cast< bool*>(_v)); break;
        case 9: _t->setWrapping(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject QwtCounter::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QwtCounter.data,
      qt_meta_data_QwtCounter,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QwtCounter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtCounter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QwtCounter.stringdata0))
        return static_cast<void*>(const_cast< QwtCounter*>(this));
    return QWidget::qt_metacast(_clname);
}

int QwtCounter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
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

// SIGNAL 0
void QwtCounter::buttonReleased(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QwtCounter::valueChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
