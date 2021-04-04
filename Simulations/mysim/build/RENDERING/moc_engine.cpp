/****************************************************************************
** Meta object code from reading C++ file 'engine.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../RENDERING/engine.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'engine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Engine_t {
    QByteArrayData data[14];
    char stringdata0[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Engine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Engine_t qt_meta_stringdata_Engine = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Engine"
QT_MOC_LITERAL(1, 7, 11), // "initialized"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 8), // "finished"
QT_MOC_LITERAL(4, 29, 8), // "advanced"
QT_MOC_LITERAL(5, 38, 6), // "paused"
QT_MOC_LITERAL(6, 45, 7), // "stopped"
QT_MOC_LITERAL(7, 53, 10), // "initialize"
QT_MOC_LITERAL(8, 64, 7), // "advance"
QT_MOC_LITERAL(9, 72, 8), // "finalize"
QT_MOC_LITERAL(10, 81, 4), // "play"
QT_MOC_LITERAL(11, 86, 5), // "pause"
QT_MOC_LITERAL(12, 92, 4), // "stop"
QT_MOC_LITERAL(13, 97, 8) // "setDelay"

    },
    "Engine\0initialized\0\0finished\0advanced\0"
    "paused\0stopped\0initialize\0advance\0"
    "finalize\0play\0pause\0stop\0setDelay"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Engine[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,
       4,    0,   76,    2, 0x06 /* Public */,
       5,    0,   77,    2, 0x06 /* Public */,
       6,    0,   78,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   79,    2, 0x0a /* Public */,
       8,    0,   80,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    0,   82,    2, 0x0a /* Public */,
      11,    0,   83,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x0a /* Public */,
      13,    1,   85,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,    2,

       0        // eod
};

void Engine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Engine *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initialized(); break;
        case 1: _t->finished(); break;
        case 2: _t->advanced(); break;
        case 3: _t->paused(); break;
        case 4: _t->stopped(); break;
        case 5: _t->initialize(); break;
        case 6: _t->advance(); break;
        case 7: _t->finalize(); break;
        case 8: _t->play(); break;
        case 9: _t->pause(); break;
        case 10: _t->stop(); break;
        case 11: _t->setDelay((*reinterpret_cast< uint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Engine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Engine::initialized)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Engine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Engine::finished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Engine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Engine::advanced)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Engine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Engine::paused)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Engine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Engine::stopped)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Engine::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Engine.data,
    qt_meta_data_Engine,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Engine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Engine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Engine.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Engine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Engine::initialized()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Engine::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Engine::advanced()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Engine::paused()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Engine::stopped()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
