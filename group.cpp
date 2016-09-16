/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QChildEvent>
#include "group.h"

#include <QDebug>

Group::Group(const QXmlStreamAttributes &attributes, QWidget *parent) : GuiObject(parent)
{
    QString name, value;
    for (QXmlStreamAttribute i : attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO,
                     qUtf8Printable(name), qUtf8Printable(value));
        }
    }
}

GuiObject *Group::object(const QString &id) const
{
    return findChild<GuiObject *>(id.toLower(), Qt::FindDirectChildrenOnly);
}

void Group::childEvent(QChildEvent *event)
{
    if (event->type() == QEvent::ChildPolished && event->child()->isWidgetType()) {
        GuiObject *child = qobject_cast<GuiObject *>(event->child());
        //qDebug("'%s': '%s' polished", qUtf8Printable(property("id").toString()), qUtf8Printable(child->property("id").toString()));
        setMask(mask() + child->mask().translated(child->x(), child->y()));
    }
}
