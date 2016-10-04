/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef REGION_H
#define REGION_H

#include <QObject>
#include <QRegion>
#include <QBitmap>
#include <QRgb>

#include "object.h"

class Map;

class Region : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit Region(QObject *parent = 0);
    ~Region() {}

    QBitmap mask() const;
    Q_INVOKABLE void loadFromMap(Map *map, int threshold, bool reversed);
    Q_INVOKABLE void loadFromBitmap(const QString &id);

signals:

public slots:

private:
    static bool comparePixels(QRgb pixel1, QRgb pixel2);

    QBitmap m_bitmap;
};

#endif // REGION_H
