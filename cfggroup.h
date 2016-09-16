/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef CFGGROUP_H
#define CFGGROUP_H

#include "group.h"

class CfgGroup : public Group
{
    Q_OBJECT
public:
    explicit CfgGroup(QWidget *parent = 0);
    ~CfgGroup() {}

signals:

public slots:
};

#endif // CFGGROUP_H
