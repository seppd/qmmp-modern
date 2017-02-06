/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QPainter>
#include <QMouseEvent>

#include "togglebutton.h"
#include "bitmapstorage.h"

#include <QDebug>

ToggleButton::ToggleButton(const QXmlStreamAttributes &attributes, QWidget *parent) : Button(parent)
{
    m_storage = new BitmapStorage<ToggleButtonElement>(TotalElements);
    QString name, value;
    Q_FOREACH (const QXmlStreamAttribute &i, attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (name == "action")
            /* Replace ':' and remove spaces to fit the enum definitions */
            value = value.replace(':', '_').replace(' ', "").toUpper();
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }
}

ToggleButton::~ToggleButton()
{
    delete m_storage;
}

QString ToggleButton::activeImage() const
{ return m_storage->bitmapId(ActiveImage); }

void ToggleButton::setActiveImage(const QString &id)
{
    if (m_storage->addBitmap(ActiveImage, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
}

void ToggleButton::mouseMoveEvent(QMouseEvent *event)
{
    if (m_on)
        GuiObject::mouseMoveEvent(event);
    else
        Button::mouseMoveEvent(event);
}

void ToggleButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mask().contains(event->pos()))
        return;

    m_on = !m_on;
    if (m_on) {
        setPressed(true);
        const QPixmap& p = m_storage->pixmap(ActiveImage);
        if (!p.isNull())
            setPixmap(p);
        GuiObject::mouseReleaseEvent(event);
    } else {
        Button::mouseReleaseEvent(event);
    }

    //if (mask().contains(event->pos()))
        //emit toggled();
}

void ToggleButton::enterEvent(QEvent *event)
{
    if (m_on)
        GuiObject::enterEvent(event);
    else
        Button::enterEvent(event);
}

void ToggleButton::leaveEvent(QEvent *event)
{
    if (m_on)
        GuiObject::leaveEvent(event);
    else
        Button::leaveEvent(event);
}
