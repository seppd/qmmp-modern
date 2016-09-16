/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef BROWSER_H
#define BROWSER_H

#include "guiobject.h"

class Browser : public GuiObject
{
    Q_OBJECT
    M_OBJECT
public:
    explicit Browser(QWidget *parent = 0);
    ~Browser() {}

signals:

public slots:
};

#endif // BROWSER_H
