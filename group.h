/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef GROUP_H
#define GROUP_H

#include <QXmlStreamAttributes>
#include "guiobject.h"

class Group : public GuiObject
{
    Q_OBJECT
    Q_PROPERTY(QString instance_id MEMBER m_instanceId)
public:
    explicit Group(QWidget *parent = 0) : GuiObject(parent) {}
    explicit Group(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    virtual ~Group() {}

    Q_INVOKABLE GuiObject *object(const QString &id) const;

protected:
    virtual void childEvent(QChildEvent *event);

private:
    QString m_instanceId;
};

#endif // GROUP_H
