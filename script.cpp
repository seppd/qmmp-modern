/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "script.h"
#include "skin.h"
#include "system.h"
#include "callback.h"
#include "function.h"
#include "interpreter.h"

Q_LOGGING_CATEGORY(MODERNUI_SCRIPT_LOADER, "modernui.script.loader", QtWarningMsg)

static const quint16 maki_magic = 0x4746;
static const int guid_length = 16;

Script::Script(const QXmlStreamAttributes &attributes, QObject *parent) : QObject(parent)
{
    QString name, value;
    for (QXmlStreamAttribute i : attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }

    m_system = System::instance();
}

Script::~Script()
{
    //if (unloadCallback != Q_NULLPTR)
        //unloadCallback->run();
}

void Script::setFile(const QString &file)
{
    QString path = Skin::instance()->findFile(file);
    if (path.isEmpty()) {
         qWarning("%s: could not find file '%s'", Q_FUNC_INFO, qUtf8Printable(file));
        return;
    }
    QFile script(path);
    if (!script.open(QFile::ReadOnly)) {
        qWarning("%s: %s", qUtf8Printable(path), qUtf8Printable(script.errorString()));
        return;
    }
    m_bin = script.readAll();
    script.close();

    if (m_bin.isEmpty())
        qWarning("%s: failed to read script file", Q_FUNC_INFO);
    else
        m_file = file;
}

void Script::setParam(const QString &param)
{ m_param = param; }

int Script::load()
{
    int ret;

    if (m_bin.isEmpty())
        return -1;

    mCDebug(MODERNUI_SCRIPT_LOADER) << Q_FUNC_INFO << " : " << id();

    QDataStream stream(m_bin);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Check magic
    quint16 magic = load16(stream);
    if (magic != maki_magic) {
        qWarning("%s: %s: bad script magic", Q_FUNC_INFO, qUtf8Printable(m_file));
        return -1;
    }

    // Skip version and something else
    stream.skipRawData(6);

    loadTypes(stream);
    ret = loadFunctions(stream);
    if (ret == -1)
        return ret;
    ret = loadVariables(stream);
    if (ret == -1)
        return ret;
    ret = loadConstants(stream);
    if (ret == -1)
        return ret;
    ret = loadCallbacks(stream);
    if (ret == -1)
        return ret;
    ret = loadCode(stream);
    if (ret == -1)
        return ret;

    if (!stream.atEnd())
        qWarning("%s: %s: extra data left, possible parsing error", Q_FUNC_INFO, qUtf8Printable(m_file));

    m_bin.clear();

    if (loadCallback != Q_NULLPTR)
        loadCallback->run();

    return 0;
}

QVector<Variable> &Script::variables()
{
    return m_variables;
}

Function *Script::function(quint32 index) const
{
    if (index >= static_cast<quint32>(m_functions.size()))
        return Q_NULLPTR;

    return m_functions.at(index);
}

const QUuid Script::guid(quint32 index) const
{
    if (qint64(index) >= m_guids.size())
        return QUuid();

    return m_guids.at(index);
}

void Script::loadTypes(QDataStream &stream)
{
    QUuid guid;
    quint32 count = load32(stream);

    mCDebug(MODERNUI_SCRIPT_LOADER) << Q_FUNC_INFO;
    while (count--) {
        uint l = load32(stream);
        ushort w1 = load16(stream);
        ushort w2 = load16(stream);
        uchar b1 = load8(stream);
        uchar b2 = load8(stream);
        uchar b3 = load8(stream);
        uchar b4 = load8(stream);
        uchar b5 = load8(stream);
        uchar b6 = load8(stream);
        uchar b7 = load8(stream);
        uchar b8 = load8(stream);

        guid = QUuid(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
        mCDebug(MODERNUI_SCRIPT_LOADER) << "\t" << guid;
        m_guids.append(guid);
    }
}

int Script::loadFunctions(QDataStream &stream)
{
    quint32 count = load32(stream);

    mCDebug(MODERNUI_SCRIPT_LOADER) << Q_FUNC_INFO;
    while (count--) {
        quint16 type_index = load16(stream);

        QUuid guid = m_guids.at(type_index & 0xff);

        // Dummy
        stream.skipRawData(2);

        QMetaMethod method;
        QString name = loadString(stream).toLower();        
        if (name != "onscriptloaded" && name != "onscriptunloading") {
            method = Interpreter::methodOfMakiFunction(guid, name);
            if (!method.isValid()) {
                qWarning("%s: %s: no method found for %s", Q_FUNC_INFO, qUtf8Printable(id()), qUtf8Printable(name));
                return -1;
            }
        }
        mCDebug(MODERNUI_SCRIPT_LOADER) << "\t" << m_functions.size() << " " << name;
        Function *func = new Function(name, guid, method, this);
        m_functions.append(func);
    }

    return 0;
}

int Script::loadVariables(QDataStream &stream)
{
    quint32 count = load32(stream);

    mCDebug(MODERNUI_SCRIPT_LOADER) << Q_FUNC_INFO;
    for (quint32 i = 0; i < count; i++) {
        Variable var;
        quint8 type_index	= load8(stream);
        quint8 object = load8(stream);
        quint16 subclass = load16(stream);
        quint16 uint1 = load16(stream);
        quint16 uint2 = load16(stream);
        quint16 uint3 = load16(stream);
        quint16 uint4 = load16(stream);
        quint16 global = load8(stream);
        quint16 system = load8(stream);

        if (object) {
            if (type_index >= m_guids.size()) {
                qWarning("%s: wrong object type index", Q_FUNC_INFO);
                return -1;
            }

            QUuid guid = m_guids.at(type_index);
            const QMetaObject *meta_object = Interpreter::metaObjectForGuid(guid);
            if (meta_object == Q_NULLPTR) {
                qWarning("%s: no metaobject found for guid '%s'", Q_FUNC_INFO, qUtf8Printable(guid.toString()));
                return -1;
            }
            QString type_name = meta_object->className();
            mCDebug(MODERNUI_SCRIPT_LOADER) << "\t " << i << " index=" << type_index << " subclass=" << subclass << " global="
                                            << global << " system=" << system << " type=" << type_name;

            //int type_id = QMetaType::type(qPrintable(type_name));

            if (global) {
                if (type_name == "System") {
                    var.setValue(m_system);
                    var.setConstant();
                } else {
                    var = M_NULLPTR;
                }
            } else if (type_name == "Config") {


            } else {
                var = M_NULLPTR;
            }
        } else if (subclass) {
            Q_UNIMPLEMENTED();
            return -1;
        } else {
            if (!m_varTypes.contains(type_index)) {
                qWarning("%s: wrong variable type number", Q_FUNC_INFO);
                return -1;
            }
            //QString type_name = m_varTypes.value(type);
            //int type_id = QMetaType::type(qPrintable(type_name));
            int type_id = m_varTypes.value(type_index);
            mCDebug(MODERNUI_SCRIPT_LOADER) << "\t " << i << " index=" << type_index << " global=" << global << " system="
                                            << system << " type=" << QMetaType::typeName(type_id);
            if (type_id == QMetaType::QString)
                var.setValue(QString());
            else
                var = Variable::fromCodedValue(type_id, uint1, uint2, uint3, uint4);

            /* Special: this is NULL */
            if ((i == 1) && (type_id == QMetaType::Int)) {
                var = M_NULLPTR;
                var.setConstant();
            }
        }
        m_variables.append(var);
    }

    return 0;
}

int Script::loadConstants(QDataStream &stream)
{
    quint32 count = load32(stream);

    mCDebug(MODERNUI_SCRIPT_LOADER) << Q_FUNC_INFO;
    while (count--) {
        quint32 var_num = load32(stream);
        int type_id = m_variables.at(var_num).userType();
        if (type_id == QMetaType::QString) {
            m_variables[var_num].setValue(loadString(stream));
            m_variables[var_num].setConstant();
            mCDebug(MODERNUI_SCRIPT_LOADER) << "\t '" << m_variables.at(var_num).toString() << "'";
        } else {
            qWarning("%s: unknown type for constant: %i", Q_FUNC_INFO, type_id);
            return -1;
        }
    }

    return 0;
}

int Script::loadCallbacks(QDataStream &stream)
{
    quint32 count = load32(stream);

    mCDebug(MODERNUI_SCRIPT_LOADER) << Q_FUNC_INFO;
    while (count--) {
        quint32 var_num = load32(stream);
        quint32 func_num = load32(stream);
        quint32 offset = load32(stream);

        if (var_num >= static_cast<quint32>(m_variables.size())) {
            qWarning("%s: wrong variable nuber code", Q_FUNC_INFO);
            return -1;
        }

        if (func_num >= static_cast<quint32>(m_functions.size())) {
            qWarning("%s: wrong function nuber code", Q_FUNC_INFO);
            return -1;
        }

        Variable &var = m_variables[var_num];
        Function *func = m_functions.at(func_num);
        Callback *cb = new Callback(func, offset, this);

        mCDebug(MODERNUI_SCRIPT_LOADER) << "\tvar: " << var << " func: " << func->name() << " offset=" << offset;

        if (func->name() == "onscriptloaded") {
            loadCallback = cb;
        } else if (func->name() == "onscriptunloading") {
            unloadCallback = cb;
        } else if (var.addCallback(cb) == -1) {
            qWarning("%s: failed to add callback function", Q_FUNC_INFO);
            return -1;
        }
    }

    return 0;
}

int Script::loadCode(QDataStream &stream)
{
    quint32 code_length = load32(stream);
    mCDebug(MODERNUI_SCRIPT_LOADER) << Q_FUNC_INFO << ": length=" << code_length;
    m_code.resize(code_length);
    int nread = stream.readRawData(m_code.data(), code_length);
    if (nread == -1) {
        qWarning("%s: read error", Q_FUNC_INFO);
        return -1;
    }
    if (code_length > static_cast<quint32>(nread)) {
        qWarning("%s: actual code length is less then expected", Q_FUNC_INFO);
        return -1;
    }

    return 0;
}

const QMap<quint8, int> Script::m_varTypes {
    {2, QMetaType::Int},
    {3, QMetaType::Float},
    {4, QMetaType::Double},
    {5, QMetaType::Bool},
    {6, QMetaType::QString}};
