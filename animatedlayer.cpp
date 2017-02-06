/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QTimer>

#include "animatedlayer.h"
#include "bitmap.h"
#include "skin.h"

// TODO: deal with printf-style bitmap element ids

AnimatedLayer::AnimatedLayer(const QXmlStreamAttributes &attributes, QWidget *parent) : Layer(parent)
{
    QString name, value;
    Q_FOREACH (const QXmlStreamAttribute &i, attributes) {
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

    if (m_frames.isEmpty())
        setImage(attributes.value("image").toString());
}

QString AnimatedLayer::image() const
{
    return m_fileId;
}

void AnimatedLayer::setImage(const QString &id)
{
    if (w() == 0 || h() == 0)
        return;

    Bitmap *b = Skin::instance()->bitmap(id);
    if (b == Q_NULLPTR) {
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
        return;
    }

    m_fileId = id;

    int i = 0;
    const QPixmap &pixmap = b->pixmap();    
    if ((w() == pixmap.width()) && (pixmap.height() % h() == 0)) {
        m_horizontal = false;
        m_numFrames = pixmap.height() / h();
        while (i < pixmap.height()) {
            m_frames.append(pixmap.copy(0, i, pixmap.width(), h()));
            i += h();
        }
    } else if ((h() == pixmap.height()) && (pixmap.width() % w() == 0)) {
        m_horizontal = true;
        m_numFrames = pixmap.width() / w();
        while (i < pixmap.width()) {
            m_frames.append(pixmap.copy(i, 0, w(), pixmap.height()));
            i += w();
        }
    } else {
        qWarning("%s: don't know how to get frames from the bitmap", Q_FUNC_INFO);
        return;
    }

    m_frames.resize(m_numFrames);
    if (m_end == -1)
        m_end = m_numFrames;
}

void AnimatedLayer::setFrameHeight(int value)
{

}

void AnimatedLayer::setFrameWidth(int value)
{

}

void AnimatedLayer::setElementFrames(int value)
{

}

void AnimatedLayer::setStart(int value)
{
    m_start = value;
    restart();
}

void AnimatedLayer::setEnd(int value)
{
    m_end = value;
    restart();
}

void AnimatedLayer::setSpeed(int value)
{
    if (m_speed == value)
        return;

    m_speed = value;
    restart();
}

void AnimatedLayer::setRealTime(bool enable)
{
    if (m_realtime == enable)
        return;

    m_realtime = enable;
    restart();
}

void AnimatedLayer::setAutoReplay(bool enable)
{
    if (m_autoReplay == enable)
        return;

    m_autoReplay = enable;
    restart();
}

void AnimatedLayer::setAutoPlay(bool enable)
{
    if (m_autoPlay == enable)
        return;

    m_autoPlay = enable;
    if (enable)
        m_timer.start(m_speed, this);
    else
        m_timer.stop();
}

void AnimatedLayer::setDebug(bool enable)
{
    if (m_debug == enable)
        return;

    m_debug = enable;
}

int AnimatedLayer::framesCount()
{
    return m_numFrames;
}

void AnimatedLayer::setFrame(int number)
{
    if (number >= m_frames.size())
        return;

    m_curFrame = number;
    setPixmap(m_frames.at(m_curFrame), false, m_realtime);
}

void AnimatedLayer::restart()
{
    if (m_timer.isActive())
        m_timer.start(m_speed, this);
}

void AnimatedLayer::timerEvent(QTimerEvent *)
{
    setPixmap(m_frames.at(m_curFrame), false, m_realtime);
    m_curFrame++;
    if (m_curFrame >= m_end) {
        if (m_autoReplay)
            m_curFrame = m_start;
        else
            m_timer.stop();
    }
}
