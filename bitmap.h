/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <QObject>
#include <QXmlStreamAttributes>
#include <QPixmap>
#include "object.h"

class Bitmap : public QObject
{
    Q_OBJECT
    M_OBJECT
    Q_PROPERTY(int x READ x WRITE setX)
    Q_PROPERTY(int y READ y WRITE setY)
    Q_PROPERTY(int w READ w WRITE setW)
    Q_PROPERTY(int h READ h WRITE setH)
    Q_PROPERTY(QString file MEMBER m_file WRITE setFile)
    Q_PROPERTY(QString gammagroup MEMBER m_gammaGroup WRITE setGammaGroup)

public:
    explicit Bitmap(const QXmlStreamAttributes &attributes, QObject *parent = 0);
    ~Bitmap() {}

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);

    int w() const;
    void setW(int w);

    int h() const;
    void setH(int h);

    void setFile(const QString &file);
    void setGammaGroup(const QString &name);

    const QPixmap &pixmap() const;

private:
    int m_x = 0;
    int m_y = 0;
    int m_w = 0;
    int m_h = 0;
    QString m_file;
    QString m_gammaGroup;
    QPixmap m_pixmap;
};

#endif // BITMAP_H
