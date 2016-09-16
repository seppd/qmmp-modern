/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "map.h"
#include "skin.h"
#include "bitmap.h"

#include <QDebug>

Map::Map(QObject *parent) : QObject(parent) {}

void Map::load(const QString &id)
{
    Bitmap *b = Skin::instance()->bitmap(id.toLower());
    if (b == Q_NULLPTR) {
        qWarning("%s: could not find bitmap '%s'", Q_FUNC_INFO, qUtf8Printable(id));
        return;
    }
    m_image = b->pixmap().toImage();
    if (m_image.isNull()) {
        qWarning("%s: image for '%s' is null", Q_FUNC_INFO, qUtf8Printable(id));
        return;
    }
}

bool Map::contains(int x, int y) const
{    
    return m_image.rect().contains(x, y);
}

int Map::value(int x, int y) const
{
    return qGray(m_image.pixel(x, y));
}

