/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <algorithm>

#include "skin.h"
#include "region.h"
#include "bitmap.h"
#include "map.h"

#include <QDebug>

Region::Region(QObject *parent) : QObject(parent)
{

}

QBitmap Region::mask() const
{
    return m_bitmap;
}

// TODO: Possibly requires rework
void Region::loadFromMap(Map *map, int threshold, bool reversed)
{
    if (threshold < 0 || threshold > 255) {
        qWarning("%s: wrong treshold: %i", Q_FUNC_INFO, threshold);
        return;
    }

    if (reversed)
        threshold = 255 - threshold;

    const QImage &image = map->image();
    const QRgb *row = (const QRgb *)image.scanLine(0);
    const QRgb *pix = std::lower_bound(row, row + image.width(), qRgb(threshold, threshold, threshold), comparePixels);
    int col = image.width() - std::distance(row, pix);
    m_bitmap = QBitmap(col, image.height());
    m_bitmap.fill(Qt::color1);
}

void Region::loadFromBitmap(const QString &id)
{
    Bitmap *b = Skin::instance()->bitmap(id);
    if (b == Q_NULLPTR) {
        qWarning("%s: could not find bitmap '%s'", Q_FUNC_INFO, qUtf8Printable(id));
        return;
    }

    m_bitmap = b->pixmap().mask();
}

bool Region::comparePixels(QRgb pixel1, QRgb pixel2)
{
    return qGray(pixel1) < qGray(pixel2);
}
