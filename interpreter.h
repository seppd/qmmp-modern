/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef intERPRETER_H
#define intERPRETER_H

#include <QObject>
#include <QDataStream>
#include <QUuid>
#include <QStack>

#include "debug.h"
#include "variable.h"

Q_DECLARE_LOGGING_CATEGORY(MODERNUI_SCRIPT_INTERPRETER)

class Script;
class Callback;
class Function;
class QSignalMapper;

extern const Variable M_NULLPTR;
//extern const QObject * const M_NULLPTR;
//Q_DECLARE_METATYPE(const QObject *)

inline quint8 load8(QDataStream &stream)
{
    quint8 u8;
    stream >> u8;
    return u8;
}

inline quint16 load16(QDataStream &stream)
{
    quint16 u16;
    stream >> u16;
    return u16;
}

inline quint32 load32(QDataStream &stream)
{
    quint32 u32;
    stream >> u32;
    return u32;
}

inline QString loadString(QDataStream &stream)
{
    quint16 len = load16(stream);
    QByteArray bytes(len, 0);
    stream.readRawData(bytes.data(), len);
    return QString(bytes);
}

class Interpreter : public QObject
{
    Q_OBJECT
public:
    explicit Interpreter(QObject *parent = 0);
    ~Interpreter() {}


    void exec(Callback *callback);
    const Script *runningScript() const;

    static QMetaMethod methodOfMakiFunction(const QUuid &guid, const QString &name);
    static int connectCallback(const Variable &variable, Callback *callback);

    static Interpreter *instance();
    //static const QMetaObject *metaObjectForGuid(const QString &guid);
    static const QMetaObject *metaObjectForGuid(const QUuid &guid);
    //static QMetaMethod methodOfMakiFunction(const QUuid &guid, const QString &function_name);

private:
    enum Instruction {
        INST_PUSH       = 0x01,
        INST_POP        = 0x02,
        INST_POPTO      = 0x03,
        INST_EQ         = 0x08,
        INST_NEQ        = 0x09,
        INST_GT         = 0x0A,
        INST_GTQ        = 0x0B,
        INST_LE         = 0x0C,
        INST_LEQ        = 0x0D,
        INST_JMPIF      = 0x10,
        INST_JMPIFNOT   = 0x11,
        INST_JUMP       = 0x12,
        INST_CALL       = 0x18,
        INST_CALL_LOC   = 0x19,
        INST_RET        = 0x21,
        INST_MOV        = 0x30,
        INST_ADD        = 0x40,
        INST_SUB        = 0x41,
        INST_MUL        = 0x42,
        INST_DIV        = 0x43,
        INST_MOD        = 0x44,
        INTS_AND        = 0x48,
        INTS_OR         = 0x49,
        INST_NOT        = 0x4A,
        INST_NEGATIVE   = 0x4C,
        INST_LOGAND     = 0x50,
        INST_LOGOR      = 0x51,
        INST_LSHIFT     = 0x5A,
        INST_RSHIFT     = 0x5B,
        INST_NEW        = 0x60,
        INST_DELETE     = 0x61,
        INST_SCALL      = 0x70,
    };

    void opEqual();
    void opNotEqual();
    void opLess();
    void opLessEqual();
    void opGreater();
    void opGreaterEqual();
    void opAdd();
    void opSubtract();
    void opMultiply();
    void opDivide();
    void opModulus();
    void opNot();
    void opLogAnd();
    void opLogOr();
    void opMov();
    void opNew(const QUuid &guid);
    void opDelete();
    void opCall(Function *func);

    void checkProtectionBlock(QDataStream &stream);

    static Interpreter *m_instance;
    const Script *m_curScript;

    QStack<Variable *> m_stack;
    QStack<quint32> m_returnOffsetStack;
    QVector<Variable> m_allocations;

    static const QMap<QUuid, const QMetaObject *> m_metaObjects;
    static const QMap<QString, QString> m_methodNames;
};

#endif // intERPRETER_H
