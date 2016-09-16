/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#include <QXmlStreamAttributes>

#include "debug.h"
#include "object.h"
#include "variable.h"

Q_DECLARE_LOGGING_CATEGORY(MODERNUI_SCRIPT_LOADER)

//class Variable;
class Callback;
class System;
class Function;

class Script : public QObject
{
    Q_OBJECT
    M_OBJECT
    Q_PROPERTY(QString file MEMBER m_file WRITE setFile)
    Q_PROPERTY(QString param MEMBER m_param WRITE setParam)

public:
    explicit Script(const QXmlStreamAttributes &attributes, QObject *parent = 0);
    virtual ~Script();

    void setFile(const QString &file);
    void setParam(const QString &param);

    int load();

    const QByteArray& code() const
    { return m_code; }

    QVector<Variable> &variables();
    //Variable variable(quint32 number) const;
    Function *function(quint32 index) const;
    const QUuid guid(quint32 index) const;

//signals:
    //void loaded();
    //void unloaded();

private:
    void loadTypes(QDataStream &stream);
    int loadFunctions(QDataStream &stream);
    int loadVariables(QDataStream &stream);
    int loadConstants(QDataStream &stream);
    int loadCallbacks(QDataStream &stream);
    int loadCode(QDataStream &stream);
    System *m_system;
    QVector<QUuid> m_guids;
    QVector<Variable> m_variables;
    //QVector<QMetaMethod> m_func_methods;
    //QVector<QString> m_func_names; // TODO: Temp, remove it
    QByteArray m_code;
    QVector<Function *> m_functions;
    //QVector<Callback *> m_callbacks;

    QString m_file;
    QString m_param;
    QByteArray m_bin;
    //static const QMap<QString, QString> m_typeguids;
    static const QMap<quint8, int> m_varTypes;

    Callback *loadCallback = Q_NULLPTR;
    Callback *unloadCallback = Q_NULLPTR;
    //static const QMap<QString, QMap<QString, QString>> m_objmethods;
};

#endif // SCRIPT_H
