/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef BitmapFont_H
#define BitmapFont_H

#include <QXmlStreamAttributes>
#include <QMap>
#include <QChar>
#include <QPixmap>

#include "object.h"

class BitmapFont : public QObject
{
    Q_OBJECT
    M_OBJECT
    Q_PROPERTY(QString file MEMBER m_file WRITE setFile)
    Q_PROPERTY(int charwidth MEMBER m_charWidth/* WRITE setCharWidth*/)
    Q_PROPERTY(int charheight MEMBER m_charHeight/* WRITE setCharHeight*/)
    Q_PROPERTY(int hspacing MEMBER m_hSpacing/* WRITE setHSpacing*/)
    Q_PROPERTY(int vspacing MEMBER m_vSpacing/* WRITE setVSpacing*/)

public:
    explicit BitmapFont(const QXmlStreamAttributes &attributes, QObject *parent = 0);
    ~BitmapFont() {}

    QString file() const;
    void setFile(const QString &file);

    int width() const;
    int height() const;
    const QPixmap getLetter(const QChar &ch) const;

private:
    QString m_file;
    int m_charWidth = 0;
    int m_charHeight = 0;
    int m_hSpacing = 0;
    int m_vSpacing = 0;
    QMap<QChar, QPixmap> m_letters;
};

#endif // BitmapFont_H
