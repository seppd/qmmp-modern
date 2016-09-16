/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "callback.h"
#include "function.h"

void Callback::run()
{
    m_interpreter->exec(this);
}

void Callback::run(const VariableList &args)
{
    VariableList::ConstIterator iter = args.constEnd();
    while(iter != args.constBegin()) {
      iter--;
      m_args.append(*iter);
    }

    m_interpreter->exec(this);
    m_args.clear();
}
