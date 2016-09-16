/*
 *   This file is based on qmmp source file
 *   src/plugins/Ui/skinned/dock.cpp
 *   which is
 *   Copyright (C) 2007-2016 by Ilya Kotov
 *   forkotov02@hotmail.ru
 */

#include <QWidget>

#include "dock.h"

Dock *Dock::m_instance = 0;

Dock *Dock::instance()
{
    if (!m_instance)
        m_instance = new Dock();
    return m_instance;
}

Dock::Dock(QObject *parent) : QObject(parent)
{}

Dock::~Dock()
{}

void Dock::setMainWidget(QWidget *widget)
{
    m_mainWidget = widget;
    m_widgetList.prepend(widget);
    m_dockedList.prepend(false);
}

void Dock::addWidget(QWidget *widget)
{
    m_widgetList.append(widget);
    m_dockedList.append(false);
    //if (m_mainWidget)
        //widget->addActions(m_mainWidget->actions());
}

void Dock::move(QWidget* widget, const QPoint &pos)
{
    if (widget == m_mainWidget) {
        widget->move(pos);
    } else {
        widget->move(pos);
    }
}
