/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef TABSHEET_H
#define TABSHEET_H

#include "guiobject.h"

class TabSheet : public GuiObject
{
    Q_OBJECT
public:
    explicit TabSheet(QWidget *parent = 0);
    ~TabSheet() {}

signals:

public slots:
};

#endif // TABSHEET_H
