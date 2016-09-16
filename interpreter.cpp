/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QMetaMethod>
#include <QMessageBox>

#include "interpreter.h"
#include "function.h"
#include "system.h"
#include "script.h"
#include "callback.h"
#include "container.h"
#include "layout.h"
#include "group.h"
#include "layer.h"
#include "button.h"
#include "togglebutton.h"
#include "slider.h"
#include "text.h"
#include "map.h"
#include "list.h"
#include "popupmenu.h"
#include "region.h"
#include "timer.h"
#include "windowholder.h"
#include "componentbucket.h"
#include "edit.h"
#include "vis.h"
#include "browser.h"
#include "eqvis.h"
#include "status.h"
#include "title.h"
#include "animatedlayer.h"
#include "grouplist.h"
#include "cfggroup.h"
#include "mouseredir.h"
#include "dropdownlist.h"
#include "layoutstatus.h"
#include "tabsheet.h"

Q_LOGGING_CATEGORY(MODERNUI_SCRIPT_INTERPRETER, "modernui.script.interpreter", QtWarningMsg)

const Variable MAKI_NULLPTR = Variable::fromValue(static_cast<QObject *>(Q_NULLPTR));

Interpreter *Interpreter::m_instance = Q_NULLPTR;

Interpreter *Interpreter::instance()
{
    if (m_instance == Q_NULLPTR)
        m_instance = new Interpreter();
    return m_instance;
}

Interpreter::Interpreter(QObject *parent) : QObject(parent)
{
    m_instance = this;
    m_allocPool.reserve(64);
}

const QMetaObject *Interpreter::metaObjectForGuid(const QUuid &guid)
{
    if (!m_metaObjects.contains(guid))
        return Q_NULLPTR;

    return m_metaObjects.value(guid);
}

QMetaMethod Interpreter::methodOfMakiFunction(const QUuid &guid, const QString &name)
{
    if (!m_metaObjects.contains(guid) || !m_methodNames.contains(name))
        return QMetaMethod();

    const QMetaObject *meta_object = m_metaObjects.value(guid);
    QString method_name = m_methodNames.value(name);

    // Reverse order should be faster in most cases
    for (int i = meta_object->methodCount() - 1; i >= 0; i--) {
        if (method_name == meta_object->method(i).name())
            return meta_object->method(i);
    }

    return QMetaMethod();
}

int Interpreter::connectCallback(const Variable &variable, Callback *callback)
{
    static const QMetaMethod run = Callback::staticMetaObject.method(Callback::staticMetaObject.indexOfMethod("run()"));
    static const QMetaMethod runargs = Callback::staticMetaObject.method(Callback::staticMetaObject.indexOfMethod("run(VariableList)"));

    QObject *obj = variable.value<QObject *>();
    if (obj == Q_NULLPTR) {
        mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "uninitialized object";
        return -1;
    }

    Function *func = callback->function();
    const QMetaMethod &method = func->method();
    if (!method.isValid()) {
        mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "no meta method found for function ", func->name();
        return -1;
    }

    if (method.parameterCount() == 0)
        connect(obj, method, callback, run);
    else
        connect(obj, method, callback, runargs);

    return 0;
}

const Script *Interpreter::runningScript() const
{
    return m_curScript;
}

void Interpreter::opEqual()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tEQ: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(*arg2 == *arg1);
    m_allocPool.append(ret);
    //m_retVal.setValue(*arg2 == *arg1);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t     ret=" << ret;

    m_stack.push(&m_allocPool.last());
    //m_stack.push(&m_retVal);
}

void Interpreter::opNotEqual()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tNEQ: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(*arg2 != *arg1);
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t     ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opLess()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tLE: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(*arg2 < *arg1);
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t    ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opLessEqual()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tLEQ: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(*arg2 <= *arg1);
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t     ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opGreater()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tGT: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(*arg2 > *arg1);
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t    ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opGreaterEqual()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tGTQ: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(*arg2 >= *arg1);
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t     ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opAdd()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tADD: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    if (static_cast<QMetaType::Type>(arg2->type()) == QMetaType::QString)
        ret.setValue(arg2->toString() + arg1->toString());
    else
        ret.setValue(arg2->toDouble() + arg1->toDouble());
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t     ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opSubtract()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tSUB: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(arg2->toDouble() - arg1->toDouble());
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t     ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opMultiply()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tMUL: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(arg2->toDouble() * arg1->toDouble());
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t      ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opDivide()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tDIV: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    ret.setValue(arg2->toDouble() / arg1->toDouble());
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t      ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opModulus()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tMOD: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    if (static_cast<QMetaType::Type>(arg2->type()) != QMetaType::Int ||
            static_cast<QMetaType::Type>(arg2->type()) != QMetaType::Int) {
        mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "wrong operand types: " << arg2->typeName() << " " << arg1->typeName();
        return;
    }

    ret.setValue(arg2->toInt() % arg1->toInt());
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t      ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opNot()
{
    Variable ret;
    Variable *arg = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tNOT: " << "arg=" << *arg;

    bool res = arg->toBool();
    ret.setValue(!res);
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t      ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opLogAnd()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tLOGAND: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    bool res = arg2->toBool() && arg1->toBool();
    ret.setValue(res);
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t         ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opLogOr()
{
    Variable ret;
    Variable *arg1 = m_stack.pop();
    Variable *arg2 = m_stack.pop();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tLOGOR: " << "arg2=" << *arg2 << " arg1=" << *arg1;

    bool res = arg2->toBool() || arg1->toBool();
    ret.setValue(res);
    m_allocPool.append(ret);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t        ret=" << ret;

    m_stack.push(&m_allocPool.last());
}

void Interpreter::opMov()
{
    // Convert back to the original variable type after the assignment
    int type = m_stack.at(m_stack.size() - 2)->type();
    *m_stack.top() = *m_stack.pop();
    m_stack.top()->convert(type);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tMOV: " << *m_stack.top();
}

void Interpreter::opNew(const QUuid &guid)
{
    const QMetaObject *meta_object = metaObjectForGuid(guid);
    if (meta_object == Q_NULLPTR) {
        mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "no metaobject found for guid ", guid.toString();
        return;
    }

    QObject *obj = meta_object->newInstance(Q_ARG(QObject *, this));
    if (obj == Q_NULLPTR) {
        mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "failed to create object of type " << meta_object->className();
        return;
    }

    Variable *var = new Variable();
    var->setValue(obj);
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tNEW: " << *var;
    m_stack.push(var);
}

void Interpreter::opDelete()
{
    Variable *var = m_stack.pop();
    QObject *obj = var->value<QObject *>();
    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tDELETE: " << *var;
    delete obj;
    delete var;
}

void Interpreter::opCall(Function *func)
{
    Variable ret;
    const QMetaMethod &method = func->method();
    if (!method.isValid()) {
        mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "no method found for " << func->guid() << " " << func->name();
        return;
    }

    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tCALL: " << method.returnType() << " " << method.methodSignature();

    QGenericReturnArgument retval;
    QGenericArgument arguments[10];

    if (method.returnType() != QMetaType::Void) {
        switch (method.returnType()) {
        case QMetaType::QString:
            ret.setValue(QString());
            break;
        case QMetaType::Int:
            ret.setValue(0);
            break;
        case QMetaType::Bool:
            ret.setValue(false);
            break;
        case QMetaType::UnknownType:
            ret = MAKI_NULLPTR;
            break;
        default:
            mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "unsupported return type: " << method.returnType();
            return;
        }
        retval = QGenericReturnArgument(method.typeName(), ret.data());
    }

    const char *name;
    for (int i = 0; i < method.parameterCount() && !m_stack.isEmpty(); i++) {
        name = method.parameterTypes().at(i).constData();
        Variable *var = m_stack.pop();
        if (!var->convert(Variable::nameToType(name)))
            mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "failed to convert argument " << i << " to " << name;
        arguments[i] = QGenericArgument(name, var->constData());
    }

    QObject *obj = m_stack.pop()->value<QObject *>();
    if (obj == Q_NULLPTR) {
        qWarning("%s: NULL pointer object", Q_FUNC_INFO);
        return;
    }

    bool res = method.invoke(obj,
                  retval,
                  arguments[0],
                  arguments[1],
                  arguments[2],
                  arguments[3],
                  arguments[4],
                  arguments[5],
                  arguments[6],
                  arguments[7],
                  arguments[8],
                  arguments[9]);

    if (!res) {
        mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "failed to invoke method " << method.name();
        return;
    }

    if (method.returnType() == QMetaType::Void)
        ret = MAKI_NULLPTR;

    m_allocPool.append(ret);
    m_stack.push(&m_allocPool.last());
}

void Interpreter::exec(Callback *callback)
{
    if (callback == Q_NULLPTR)
        return;

    Script *script = qobject_cast<Script *>(callback->parent());
    quint32 offset = callback->codeOffset();

    m_stack.clear();
    m_returnOffsetStack.clear();
    m_curScript = script;
    QVector<Variable> &variables = script->variables();
    const QByteArray& code = script->code();

    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << Q_FUNC_INFO << ": script="<< script->id() << " callback=" << callback->function()->name();

    VariableList &args = callback->arguments();
    for (int i = 0; i < args.size(); i++) {
        m_stack.push(&args[i]);
        mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "arg[" << i << "]=" << *m_stack.top();
    }

    QDataStream stream(code);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.skipRawData(offset);

    while (1) {
        quint8 inst = loadUint8(stream);
        switch (inst) {
        case INST_PUSH:
            {
                quint32 number = loadUint32(stream);
                Variable *var = &variables[number];
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tPUSH: " << number << " " << *var;
                m_stack.push(var);
            }
            break;
        case INST_POP:
            {
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tPOP: stacksize=" << m_stack.size();
                m_stack.pop();
            }
            break;
        case INST_POPTO:
            {                
                quint32 number = loadUint32(stream);
                Variable *var = &variables[number];
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tPOPTO: stacksize=" << m_stack.size() << " " << number << " " << *var;
                *var = *m_stack.pop();
            }
            break;
        case INST_EQ:
            {                
                opEqual();
            }
            break;
        case INST_NEQ:
            {
                opNotEqual();
            }
            break;
        case INST_LE:
            {
                opLess();
            }
            break;
        case INST_LEQ:
            {
                opLessEqual();
            }
            break;
        case INST_GT:
            {
                opGreater();
            }
            break;
        case INST_GTQ:
            {
                opGreaterEqual();
            }
            break;
        case INST_JMPIF:
            {
                // Jump on 'false'
                quint32 to = loadUint32(stream);
                Variable *flag = m_stack.pop();
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tJMPIF: " << *flag;
                if (!flag->toBool()) {
                    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t       + " << to << " bytes";
                    stream.skipRawData(to);
                }
            }
            break;
        case INST_JMPIFNOT:
            {
                // Jump on 'true'
                quint32 to = loadUint32(stream);
                Variable *flag = m_stack.pop();
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tJMPIFNOT: " << *flag;
                if (flag->toBool()) {
                    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\t          + " << to << " bytes";
                    stream.skipRawData(to);
                }
            }
            break;
        case INST_JUMP:
            {
                quint32 to = loadUint32(stream);
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tJUMP: +" << to << " bytes";
                stream.skipRawData(to);
            }
            break;
        case INST_CALL:
            {
                quint32 var_index = loadUint32(stream);
                Function *func = script->function(var_index);
                if (func == Q_NULLPTR) {
                    mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "no function with index=" << var_index << " found";
                    break;
                }
                opCall(func);
            }
            break;
        case INST_CALL_LOC:
            {
                quint32 offset = loadUint32(stream);
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tCALL_LOC: offset=" << offset << " ret=" << stream.device()->pos();
                m_returnOffsetStack.push(stream.device()->pos());
                stream.skipRawData(offset);
            }
            break;
        case INST_RET:
            {
                if (!m_returnOffsetStack.isEmpty()) {
                    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tRET: from local function";
                    quint32 offset = m_returnOffsetStack.pop();
                    stream.skipRawData(offset - stream.device()->pos());
                } else {
                    mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tRET: from callback (alloc=" << m_allocPool.size() << ")";
                    m_allocPool.clear();
                    return;
                }
            }
            break;
        case INST_MOV:
            {
                opMov();
            }
            break;
        case INST_ADD:
            {
                opAdd();
            }
            break;
        case INST_SUB:
            {
                opSubtract();
            }
            break;
        case INST_MUL:
            {
                opMultiply();
            }
            break;
        case INST_DIV:
            {
                opDivide();
            }
            break;
        case INST_MOD:
            {
                opModulus();
            }
            break;
        case INTS_AND:
            {
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tAND: not implemented";
                return;
            }
            break;
        case INTS_OR:
            {
                mCDebug(MODERNUI_SCRIPT_INTERPRETER) << "\tOR: not implemented";
                return;
            }
            break;
        case INST_NOT:
            {
                opNot();
            }
            break;
        case INST_NEGATIVE:
            {
                mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "\tNEGATIVE: not implemented";
                return;
            }
            break;
        case INST_LOGAND:
            {
                opLogAnd();
            }
            break;
        case INST_LOGOR:
            {
                opLogOr();
            }
            break;
        case INST_LSHIFT:
            {
                mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "\tLSHIFT: not implemented";
                return;
            }
        case INST_RSHIFT:
            {
                mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "\tRSHIFT: not implemented";
                return;
            }
        case INST_NEW:
            {
                quint32 type_index = loadUint32(stream);
                const QUuid &guid = script->guid(type_index);
                opNew(guid);
            }
            break;
        case INST_DELETE:
            {
                opDelete();
            }
            break;
        case INST_SCALL:
            {
                quint32 var_index = loadUint32(stream);
                Function *func = script->function(var_index);
                if (func == Q_NULLPTR) {
                    mCWarning(MODERNUI_SCRIPT_INTERPRETER) << "no function with index=" << var_index << " found";
                    break;
                }
                opCall(func);
                stream.skipRawData(1);
            }
            break;
        default:
            qWarning("%s: unknown instruction code: %02x", Q_FUNC_INFO, inst);
            return;
        }
    }
}

const QMap<QUuid, const QMetaObject *> Interpreter::m_metaObjects {
    {QUuid(0x51654971, 0x0d87, 0x4a51, 0x91, 0xe3, 0xa6, 0xb5, 0x32, 0x35, 0xf3, 0xe7), &QObject::staticMetaObject},
    {QUuid(0xd6f50f64, 0x93fa, 0x49b7, 0x93, 0xf1, 0xba, 0x66, 0xef, 0xae, 0x3e, 0x98), &System::staticMetaObject},
    {QUuid(0xe90dc47b, 0x840d, 0x4ae7, 0xb0, 0x2c, 0x04, 0x0b, 0xd2, 0x75, 0xf7, 0xfc), &Container::staticMetaObject},
    {QUuid(0xb2023ab5, 0x434d, 0x4ba1, 0xbe, 0xae, 0x59, 0x63, 0x75, 0x03, 0xf3, 0xc6), &List::staticMetaObject},
    {QUuid(0x38603665, 0x461b, 0x42a7, 0xaa, 0x75, 0xd8, 0x3f, 0x66, 0x67, 0xbf, 0x73), &Map::staticMetaObject},
    {QUuid(0xf4787af4, 0xb2bb, 0x4ef7, 0x9c, 0xfb, 0xe7, 0x4b, 0xa9, 0xbe, 0xa8, 0x8d), &PopupMenu::staticMetaObject},
    {QUuid(0x3a370c02, 0x3cbf, 0x439f, 0x84, 0xf1, 0x86, 0x88, 0x5b, 0xcf, 0x1e, 0x36), &Region::staticMetaObject},
    {QUuid(0x5d0c5bb6, 0x7de1, 0x4b1f, 0xa7, 0x0f, 0x8d, 0x16, 0x59, 0x94, 0x19, 0x41), &Timer::staticMetaObject},
    {QUuid(0x4ee3e199, 0xc636, 0x4bec, 0x97, 0xcd, 0x78, 0xbc, 0x9c, 0x86, 0x28, 0xb0), &GuiObject::staticMetaObject},
    {QUuid(0x45be95e5, 0x2072, 0x4191, 0x93, 0x5c, 0xbb, 0x5f, 0xf9, 0xf1, 0x17, 0xfd), &Group::staticMetaObject},
    {QUuid(0x60906d4e, 0x537e, 0x482e, 0xb0, 0x04, 0xcc, 0x94, 0x61, 0x88, 0x56, 0x72), &Layout::staticMetaObject},
    {QUuid(0x403abcc0, 0x6f22, 0x4bd6, 0x8b, 0xa4, 0x10, 0xc8, 0x29, 0x93, 0x25, 0x47), &WindowHolder::staticMetaObject},
    {QUuid(0x97aa3e4d, 0xf4d0, 0x4fa8, 0x81, 0x7b, 0x0a, 0xf2, 0x2a, 0x45, 0x49, 0x83), &ComponentBucket::staticMetaObject},
    {QUuid(0x64e4bbfa, 0x81f4, 0x49d9, 0xb0, 0xc0, 0xa8, 0x5b, 0x2e, 0xc3, 0xbc, 0xfd), &Edit::staticMetaObject},
    {QUuid(0x62b65e3f, 0x375e, 0x408d, 0x8d, 0xea, 0x76, 0x81, 0x4a, 0xb9, 0x1b, 0x77), &Slider::staticMetaObject},
    {QUuid(0xce4f97be, 0x77b0, 0x4e19, 0x99, 0x56, 0xd4, 0x98, 0x33, 0xc9, 0x6c, 0x27), &Vis::staticMetaObject},
    {QUuid(0xa8c2200d, 0x51eb, 0x4b2a, 0xba, 0x7f, 0x5d, 0x4b, 0xc6, 0x5d, 0x4c, 0x71), &Browser::staticMetaObject},
    {QUuid(0x8d1eba38, 0x489e, 0x483e, 0xb9, 0x60, 0x8d, 0x1f, 0x43, 0xc5, 0xc4, 0x05), &EqVis::staticMetaObject},
    {QUuid(0x0f08c940, 0xaf39, 0x4b23, 0x80, 0xf3, 0xb8, 0xc4, 0x8f, 0x7e, 0xbb, 0x59), &Status::staticMetaObject},
    {QUuid(0xefaa8672, 0x310e, 0x41fa, 0xb7, 0xdc, 0x85, 0xa9, 0x52, 0x5b, 0xcb, 0x4b), &Text::staticMetaObject},
    {QUuid(0x7dfd3244, 0x3751, 0x4e7c, 0xbf, 0x40, 0x82, 0xae, 0x5f, 0x3a, 0xdc, 0x33), &Title::staticMetaObject},
    {QUuid(0x5ab9fa15, 0x9a7d, 0x4557, 0xab, 0xc8, 0x65, 0x57, 0xa6, 0xc6, 0x7c, 0xa9), &Layer::staticMetaObject},
    {QUuid(0x698eddcd, 0x8f1e, 0x4fec, 0x9b, 0x12, 0xf9, 0x44, 0xf9, 0x09, 0xff, 0x45), &Button::staticMetaObject},
    {QUuid(0x6b64cd27, 0x5a26, 0x4c4b, 0x8c, 0x59, 0xe6, 0xa7, 0x0c, 0xf6, 0x49, 0x3a), &AnimatedLayer::staticMetaObject},
    {QUuid(0xb4dccfff, 0x81fe, 0x4bcc, 0x96, 0x1b, 0x72, 0x0f, 0xd5, 0xbe, 0x0f, 0xff), &ToggleButton::staticMetaObject},
    {QUuid(0x01e28ce1, 0xb059, 0x11d5, 0x97, 0x9f, 0xe4, 0xde, 0x6f, 0x51, 0x76, 0x0a), &GroupList::staticMetaObject},
    {QUuid(0x80f0f8bd, 0x1ba5, 0x42a6, 0xa0, 0x93, 0x32, 0x36, 0xa0, 0x0c, 0x8d, 0x4a), &CfgGroup::staticMetaObject},
    {QUuid(0x9b2e341b, 0x6c98, 0x40fa, 0x8b, 0x85, 0x0c, 0x1b, 0x6e, 0xe8, 0x94, 0x05), &MouseRedir::staticMetaObject},
    {QUuid(0x36d59b71, 0x03fd, 0x4af8, 0x97, 0x95, 0x05, 0x02, 0xb7, 0xdb, 0x26, 0x7a), &DropDownList::staticMetaObject},
    {QUuid(0x7fd5f210, 0xacc4, 0x48df, 0xa6, 0xa0, 0x54, 0x51, 0x57, 0x6c, 0xdc, 0x76), &LayoutStatus::staticMetaObject},
    {QUuid(0xb5baa535, 0x05b3, 0x4dcb, 0xad, 0xc1, 0xe6, 0x18, 0xd2, 0x8f, 0x68, 0x96), &TabSheet::staticMetaObject}};

const QMap<QString, QString> Interpreter::m_methodNames {
    // System
    {"messagebox", "messageBox"},
    {"onplay", "playing"},
    {"onpause", "paused"},
    {"onstop", "stopped"},
    {"onresume", "resumed"},
    {"getscriptgroup", "scriptGroup"},
    {"getplayitemstring", "playItemString"},
    {"getvolume", "volume"},
    {"setvolume", "setVolume"},
    {"onvolumechanged", "volumeChanged"},
    {"integertostring", "intToString"},
    {"stringtointeger", "stringToint"},
    {"floattostring", "floatToString"},
    {"stringtofloat", "stringToFloat"},

    // GuiObject
    {"onmousemove", "mouseMoved"},
    {"onleftbuttondown", "leftButtonPressed"},
    {"onleftbuttonup", "leftButtonReleased"},
    {"show", "show"},
    {"hide", "hide"},
    {"findobject", "findObject"},
    {"getleft", "left"},
    {"gettop", "top"},

    // Group
    {"getobject", "object"},

    // Slider
    {"onpostedposition", "positionPosted"},
    {"getlength", "framesCount"},
    {"gotoframe", "setFrame"},

    // Timer
    {"start", "start"},
    {"stop", "stop"},
    {"setdelay", "setDelay"},
    {"ontimer", "timeout"},

    // Map
    {"loadmap", "loadBitmap"},
    {"inregion", "contains"},
    {"loadmap", "load"},
    {"getvalue", "value"},

    // Text
    {"getText", "text"},
    {"settext", "setText"},
    {"setalternatetext", "setAlternateText"}
};
