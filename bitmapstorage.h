/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef BITMAPSTORAGE_H
#define BITMAPSTORAGE_H

#include <QVector>
#include <QPixmap>
#include <QString>

#include "skin.h"
#include "bitmap.h"

template <typename T>
class BitmapStorage
{
public:
    explicit BitmapStorage(T total)
    {        
        m_skin = Skin::instance();
        m_ids.resize(total);
        m_pixmaps.resize(total);
    }

    int addBitmap(T element, const QString &id)
    {
        QString lid = id.toLower();
        Bitmap *b = Skin::instance()->bitmap(lid);
        if (b == Q_NULLPTR)
            return -1;

        m_ids[element] = lid;
        m_pixmaps[element] = b->pixmap();
        return 0;
    }

    const QPixmap &pixmap(T element) const
    {
        return m_pixmaps.at(element);
    }

    QString bitmapId(T element) const
    {
        return m_ids.at(element);
    }

private:
    Skin *m_skin = Q_NULLPTR;
    QVector<QPixmap> m_pixmaps;
    QVector<QString> m_ids;
};

#endif // BITMAPSTORAGE_H
