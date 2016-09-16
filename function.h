/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef FUNCTION_H
#define FUNCTION_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QMetaMethod>

class Function : public QObject
{
    Q_OBJECT
public:
    explicit Function(QString name, QUuid guid, QMetaMethod method, QObject *parent = 0) :
        QObject(parent), m_name(name), m_guid(guid), m_method(method) { }
    ~Function() {}

    QString name()
    {
        return m_name;
    }

    QUuid guid()
    {
        return m_guid;
    }

    const QMetaMethod& method()
    {
        return m_method;
    }

private:
    QString m_name;
    QUuid m_guid;
    QMetaMethod m_method;
};

#endif // FUNCTION_H
