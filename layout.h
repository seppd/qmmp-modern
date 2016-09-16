/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef LAYOUT_H
#define LAYOUT_H

#include <QXmlStreamAttributes>

#include "group.h"

class Layout : public Group
{
    Q_OBJECT
public:
    explicit Layout(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    ~Layout() {}

protected:

private:

};

#endif // LAYOUT_H
