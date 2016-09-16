/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include "button.h"

class Bitmap;

template <typename T>
class BitmapStorage;

class ToggleButton : public Button
{
    Q_OBJECT
    Q_PROPERTY(QString activeimage READ activeImage WRITE setActiveImage)

public:
    explicit ToggleButton(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    ~ToggleButton();

    QString activeImage() const;
    void setActiveImage(const QString &id);

signals:
    void toggled();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    enum ToggleButtonElement {
        ActiveImage = 0,
        TotalElements,
    };

    bool m_on = false;
    BitmapStorage<ToggleButtonElement> *m_storage = Q_NULLPTR;
};

#endif // TOGGLEBUTTON_H
