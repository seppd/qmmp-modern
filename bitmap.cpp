/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QVariant>

#include "bitmap.h"
#include "skin.h"

Bitmap::Bitmap(const QXmlStreamAttributes &attributes, QObject *parent) : QObject(parent)
{
    QString name, value;
    Q_FOREACH (const QXmlStreamAttribute &i, attributes) {
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

int Bitmap::x() const
{ return m_x; }

void Bitmap::setX(int x)
{
    m_x = x;
}

int Bitmap::y() const
{ return m_y; }

void Bitmap::setY(int y)
{
    m_y = y;
}

int Bitmap::w() const
{ return m_w; }

void Bitmap::setW(int w)
{
    m_w = w;
}

int Bitmap::h() const
{ return m_h; }

void Bitmap::setH(int h)
{
    m_h = h;
}

const QPixmap& Bitmap::pixmap() const
{ return m_pixmap; }

void Bitmap::setFile(const QString &file)
{
    QString path = Skin::instance()->findFile(file);
    if (path.isEmpty()) {
         qWarning("%s: could not find file '%s'", Q_FUNC_INFO, qUtf8Printable(file));
        return;
    }
    if (!m_pixmap.load(path)) {
        qWarning("%s: could not load file '%s'", Q_FUNC_INFO, qUtf8Printable(file));
        return;
    }
    m_file = file;
}

void Bitmap::setGammaGroup(const QString &name)
{

}
