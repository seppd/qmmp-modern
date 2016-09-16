/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef POPUPMENU_H
#define POPUPMENU_H

#include <QObject>
#include "object.h"

class PopupMenu : public QObject
{
    Q_OBJECT
public:
    explicit PopupMenu(QObject *parent = 0);
    ~PopupMenu() {}

signals:

public slots:
};

#endif // POPUPMENU_H
