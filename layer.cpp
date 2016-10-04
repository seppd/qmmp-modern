/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QPainter>

#include "layer.h"
#include "bitmap.h"
#include "region.h"
#include "skin.h"

#include <QDebug>

Layer::Layer(const QXmlStreamAttributes &attributes, QWidget *parent) : GuiObject(parent)
{
    setMoveFlag(true);
    QString name, value;
    for (QXmlStreamAttribute i : attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }
}

QString Layer::image() const
{
    return m_imageId;
}

void Layer::setImage(const QString &id)
{
    Bitmap *b = Skin::instance()->bitmap(id);
    if (b == Q_NULLPTR) {
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
        return;
    }

    m_bitmap = b;
    if (w() == 0 || h() == 0)
        setFixedSize(b->pixmap().size());
    setPixmap(b->pixmap());
}

void Layer::setRegion(Region *region)
{
    QPixmap p = m_bitmap->pixmap();
    QBitmap mask = QBitmap(p.size());
    mask.fill(Qt::color0);
    QPainter painter(&mask);
    painter.drawPixmap(0, 0, region->mask());
    p.setMask(mask);
    setPixmap(p, false);
}
