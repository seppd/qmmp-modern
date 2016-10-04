/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <cmath>

#include "variable.h"
#include "callback.h"
#include "function.h"

Variable Variable::fromCodedValue(int type_id, quint16 uint1, quint16 uint2, quint16 uint3, quint16 uint4)
{
    Q_UNUSED(uint3);
    Q_UNUSED(uint4);
    Variable var;

    if (type_id == QMetaType::Bool) {
        var.setValue(uint1 != 0);
    } else if (type_id == QMetaType::Int) {
        // What about negative values?
        var.setValue(uint1);
    } else if (type_id == QMetaType::Float || type_id == QMetaType::Double) {
        float fraction;
        int exponent = ((uint2 & 0xff80) >> 7);
        quint32 tmp = ((0x80 | (uint2 & 0x7f)) << 15 | uint1);
        memcpy(&fraction, &tmp, sizeof(tmp));
        var.setValue(ldexp(fraction, exponent));
    } else {
        qWarning("%s: unknown type: %i", Q_FUNC_INFO, type_id);
    }
    if (!var.convert(type_id))
        qWarning("%s: failed to convert to type %i", Q_FUNC_INFO, type_id);
    return var;
}

Variable &Variable::operator=(const Variable &other)
{
    if (m_constant) {
        qWarning("%s: attempt to change a constant variable", Q_FUNC_INFO);
        return *this;
    }

    QVariant::operator=(other);

    if (!m_pendingConnections.empty()) {
        Interpreter *interpreter = Interpreter::instance();
        for (Callback *callback : m_pendingConnections) {
            //qDebug() << "connect pending" << callback->function()->name();
            interpreter->connectCallback(*this, callback);
        }
        m_pendingConnections.clear();
    }

    return *this;
}

int Variable::addCallback(Callback *callback)
{
    int ret = 0;

    if (static_cast<QMetaType::Type>(type()) < QMetaType::User &&
            static_cast<QMetaType::Type>(type()) != QMetaType::QObjectStar) {
        qWarning("%s: variable of type '%s' cannot have callbacks", Q_FUNC_INFO, typeName());
        return -1;
    }

    if (*this == M_NULLPTR) {
        //qDebug() << "added to pending" << callback->function()->name();
        m_pendingConnections.append(callback);
    } else {
        ret = Interpreter::instance()->connectCallback(*this, callback);
    }

    return ret;
}
