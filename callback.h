/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef CALLBACK_H
#define CALLBACK_H

#include <QObject>
#include <QMetaMethod>

#include "interpreter.h"
#include "variable.h"

class Function;

class Callback : public QObject
{
    Q_OBJECT
public:
    explicit Callback(Function *function, quint32 offset, QObject *parent = 0) :
        QObject(parent), m_func(function), m_offset(offset)
    {
        m_interpreter = Interpreter::instance();
    }
    ~Callback() {}

    Function *function() const
    {
        return m_func;
    }

    quint32 codeOffset() const
    {
        return m_offset;
    }

    VariableList &arguments()
    {
        return m_args;
    }

public slots:
#if 0
    void run(Variable arg0 = Variable(), Variable arg1 = Variable(),
             Variable arg2 = Variable(), Variable arg3 = Variable());
#endif
    void run();
    void run(const VariableList &args);

private:
    Interpreter *m_interpreter;
    Function *m_func;
    quint32 m_offset;
    VariableList m_args;
};

#endif // CALLBACK_H
