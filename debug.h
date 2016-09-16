/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <QLoggingCategory>

#define mCDebug(category, ...) qCDebug(category, __VA_ARGS__).nospace().noquote()
#define mCWarning(category, ...) qCWarning(category, __VA_ARGS__).nospace().noquote()

#endif // DEBUG_H
