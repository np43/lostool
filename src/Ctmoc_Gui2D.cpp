/****************************************************************************
** Meta object code from reading C++ file 'Gui2D.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/Gui2D.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Gui2D.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Gui2D_t {
    QByteArrayData data[8];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Gui2D_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Gui2D_t qt_meta_stringdata_Gui2D = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Gui2D"
QT_MOC_LITERAL(1, 6, 10), // "wheelEvent"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 12), // "QWheelEvent*"
QT_MOC_LITERAL(4, 31, 5), // "event"
QT_MOC_LITERAL(5, 37, 11), // "scalingTime"
QT_MOC_LITERAL(6, 49, 1), // "x"
QT_MOC_LITERAL(7, 51, 12) // "animFinished"

    },
    "Gui2D\0wheelEvent\0\0QWheelEvent*\0event\0"
    "scalingTime\0x\0animFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Gui2D[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       5,    1,   32,    2, 0x0a /* Public */,
       7,    0,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QReal,    6,
    QMetaType::Void,

       0        // eod
};

void Gui2D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Gui2D *_t = static_cast<Gui2D *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->wheelEvent((*reinterpret_cast< QWheelEvent*(*)>(_a[1]))); break;
        case 1: _t->scalingTime((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 2: _t->animFinished(); break;
        default: ;
        }
    }
}

const QMetaObject Gui2D::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_Gui2D.data,
      qt_meta_data_Gui2D,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Gui2D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Gui2D::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Gui2D.stringdata0))
        return static_cast<void*>(const_cast< Gui2D*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int Gui2D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
