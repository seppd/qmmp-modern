/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef LIST_H
#define LIST_H

#include <QObject>
#include "object.h"

class List : public QObject
{
    Q_OBJECT
public:
    explicit List(QObject *parent = 0);
    ~List() {}

signals:

public slots:
};

#endif // LIST_H
