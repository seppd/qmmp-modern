/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WINDOWHOLDER_H
#define WINDOWHOLDER_H

#include "guiobject.h"

class WindowHolder : public GuiObject
{
    Q_OBJECT
public:
    explicit WindowHolder(QWidget *parent = 0);
    ~WindowHolder() {}

signals:

public slots:
};

#endif // WINDOWHOLDER_H
