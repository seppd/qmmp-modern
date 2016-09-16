/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef CONTAINER_H
#define CONTAINER_H

#include <QWidget>
#include <QXmlStreamAttributes>

#include "object.h"

class Layout;

class Container : public QWidget
{
    Q_OBJECT
    M_OBJECT
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(bool default_visible MEMBER m_defaultVisible READ defaultVisible)
    Q_PROPERTY(int default_x READ defaultX WRITE setDefaultX)
    Q_PROPERTY(int default_y READ defaultY WRITE setDefaultY)

public:
    explicit Container(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    ~Container() {}

    bool defaultVisible() const;
    void setDefaultVisible(bool enable);

    int defaultX() const;
    void setDefaultX(int x);

    int defaultY() const;
    void setDefaultY(int y);

    Layout *layout() const;
    int setLayout(const QString &id);

    //void updateMask();

protected:
    //virtual void mousePressEvent(QMouseEvent *event);
    //virtual void mouseMoveEvent(QMouseEvent *event);
    //virtual void mouseReleaseEvent(QMouseEvent *event);
    //void childEvent(QChildEvent *event);

private:
    bool m_defaultVisible = false;
    int m_xDefault = 0;
    int m_yDefault = 0;
    QString m_name;
    Layout *m_layout = Q_NULLPTR;
    QPoint m_oldPos;
    //QMap<QString, Layout *> m_layouts;
};
//Q_DECLARE_METATYPE(Container)

#endif // CONTAINER_H
