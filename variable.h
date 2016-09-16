/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef VARIABLE_H
#define VARIABLE_H

#include <QVariant>

class Variable;
typedef QList<Variable> VariableList;

#include <QDebug>

class Callback;

class Variable : public QVariant
{
public:
    using QVariant::QVariant;

    template<typename T>
    static Variable fromValue(const T &value)
    {
        Variable var;
        var.setValue(value);
        return var;
    }

    static Variable fromCodedValue(int type, quint16 uint1, quint16 uint2, quint16 uint3, quint16 uint4);
    int addCallback(Callback *callback);

    inline void setConstant();

    template<typename T>
    void setValue(const T &value);

    Variable &operator=(const Variable &other);

private:
    bool m_constant = false;
    QVector<Callback *> m_pendingConnections;
};

template<typename T>
inline void Variable::setValue(const T &value)
{
    if (m_constant) {
        qWarning("%s: attempt to change a constant variable", Q_FUNC_INFO);
        return;
    }

    QVariant::setValue(value);
}

inline void Variable::setConstant()
{
    m_constant = true;
}



#endif // VARIABLE_H
