/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QMouseEvent>

#include "container.h"
#include "layout.h"
#include "skin.h"

#include <QDebug>

Container::Container(const QXmlStreamAttributes &attributes, QWidget *parent) : QWidget(parent)
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
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }

    setAttribute(Qt::WA_TranslucentBackground);
}

bool Container::defaultVisible() const
{ return m_defaultVisible; }

int Container::defaultX() const
{ return m_xDefault; }

int Container::defaultY() const
{ return m_yDefault; }

void Container::setDefaultX(int x)
{
    move(x, y());
}

void Container::setDefaultY(int y)
{
    move(x(), y);
}

void Container::setDefaultVisible(bool enable)
{
    setVisible(enable);
    qDebug() << Q_FUNC_INFO << enable;
}

Layout *Container::layout() const
{
    return m_layout;
}

int Container::setLayout(const QString &id)
{
    Layout *lo = findChild<Layout *>(id);
    if (lo == Q_NULLPTR)
        return -1;
    m_layout = lo;
    //setFixedSize(lo->size());
    //qDebug() << this << lo->mask();
    setMask(lo->mask());
    return 0;
}

#if 0
void Container::updateMask()
{
    QRegion mask;
    for (auto child : findChildren<GuiObject *>("widgetname"))
        mask += child->mask();

    setMask(mask);
}
#endif

#if 0
void Container::mousePressEvent(QMouseEvent *event)
{
    //if (m_id == "main")
        //QWidget::mousePressEvent(event);
    //else
    qDebug("%s: '%s' PRESSED", Q_FUNC_INFO, qUtf8Printable(property("id").toString()));
        m_oldPos = event->globalPos();
}

void Container::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint delta = event->globalPos() - m_oldPos;
    QWidget::move(x() + delta.x(), y() + delta.y());
    m_oldPos = event->globalPos();
}

void Container::mouseReleaseEvent(QMouseEvent *event)
{

}
#endif

#if 0
void Container::childEvent(QChildEvent *event)
{
    if (event->type() == QEvent::ChildPolished && event->child()->isWidgetType()) {
        GuiObject *child = qobject_cast<GuiObject *>(event->child());
        //qDebug("'%s': '%s' polished", qUtf8Printable(property("id").toString()), qUtf8Printable(child->property("id").toString()));
        setMask(mask() + child->mask().translated(child->x(), child->y()));
    }
}
#endif
