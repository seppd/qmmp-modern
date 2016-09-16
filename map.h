/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QImage>

#include "object.h"

class Map : public QObject
{
    Q_OBJECT
    M_OBJECT
public:
    Q_INVOKABLE explicit Map(QObject *parent = 0);
    ~Map() {}

    Q_INVOKABLE void load(const QString &id);
    Q_INVOKABLE bool contains(int x, int y) const;
    Q_INVOKABLE int value(int x, int y) const;

signals:

public slots:

private:
    QImage m_image;
};

#endif // MAP_H
