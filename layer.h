/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef LAYER_H
#define LAYER_H

#include <QPair>
#include <QRegion>
#include <QXmlStreamAttributes>
#include "guiobject.h"

class Bitmap;

#if 0
// TODO:
    {"dblclickaction"},
    {"inactiveimage"},
    {"region"},
    {"resize"},
    {"scale"},
    {"tile"}};
#endif

class Layer : public GuiObject
{
    Q_OBJECT
    Q_PROPERTY(QString image READ image WRITE setImage)

public:
    explicit Layer(QWidget *parent = 0) : GuiObject(parent) { }
    explicit Layer(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    ~Layer() {}

    QString image() const;
    void setImage(const QString &id);

private:
    QPair<QString, Bitmap *> m_image;

};

#endif // LAYER_H
