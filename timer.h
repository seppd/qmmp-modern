/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>

#include "object.h"

class QTimer;

class Timer : public QObject
{
    Q_OBJECT
    M_OBJECT
public:
    Q_INVOKABLE explicit Timer(QObject *parent = 0);
    ~Timer() {}

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setDelay(int value);

signals:
    void timeout();

public slots:

private:
    QTimer *m_timer;

};

#endif // TIMER_H
