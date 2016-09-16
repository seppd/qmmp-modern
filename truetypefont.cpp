/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QFile>
#include <QVariant>
#include <QFontDatabase>
#include "truetypefont.h"
#include "skin.h"

TrueTypeFont::TrueTypeFont(const QXmlStreamAttributes &attributes, QObject *parent) : QObject(parent)
{
    QString name, value;
    for (QXmlStreamAttribute i : attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }
}

void TrueTypeFont::setFile(const QString &file)
{
    QString path = Skin::instance()->findFile(file);
    int id = QFontDatabase::addApplicationFont(path);
    if (id == -1) {
        qWarning("%s: could not load %s", Q_FUNC_INFO, qUtf8Printable(file));
        return;
    }
    m_fontId = id;
    m_file = file;
#if 0
    for (QString s : QFontDatabase::applicationFontFamilies(m_fontid))
        qDebug("%s", qUtf8Printable(s));
#endif
}

int TrueTypeFont::fontId() const
{ return m_fontId; }
