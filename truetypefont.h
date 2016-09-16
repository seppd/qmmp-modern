/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef TRUETYPEFONT_H
#define TRUETYPEFONT_H

#include <QObject>
#include <QXmlStreamAttributes>
#include "object.h"

class TrueTypeFont : public QObject
{
    Q_OBJECT
    M_OBJECT
    Q_PROPERTY(QString file MEMBER m_file WRITE setFile)
    //Q_PROPERTY(int fontid READ fontId)
public:
    explicit TrueTypeFont(const QXmlStreamAttributes &attributes, QObject *parent = 0);
    ~TrueTypeFont() {}

    QString file() const;
    void setFile(const QString &file);
    int fontId() const;

private:
    QString m_file;
    int m_fontId = -1;
};

#endif // TRUETYPEFONT_H
