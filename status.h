/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef STATUS_H
#define STATUS_H

#include "guiobject.h"

class Status : public GuiObject
{
    Q_OBJECT
public:
    explicit Status(QWidget *parent = 0);
    ~Status() {}

signals:

public slots:
};

#endif // STATUS_H
