/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QTimer>

#include "timer.h"

Timer::Timer(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void Timer::start()
{
    m_timer->start();
}

void Timer::stop()
{
    m_timer->stop();
}

void Timer::setDelay(int value)
{
    m_timer->setInterval(value);
}
