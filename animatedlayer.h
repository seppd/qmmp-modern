/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ANIMATEDLAYER_H
#define ANIMATEDLAYER_H

#include <QBasicTimer>

#include "layer.h"

class QTimer;

class AnimatedLayer : public Layer
{
    Q_OBJECT
    //Q_PROPERTY(int w MEMBER m_w WRITE setW)
    //Q_PROPERTY(int h MEMBER m_h WRITE setH)
    Q_PROPERTY(QString image MEMBER m_fileId WRITE setImage)
    Q_PROPERTY(int frameheight MEMBER m_frameHeight WRITE setFrameHeight)
    Q_PROPERTY(int framewidth MEMBER m_frameWidth WRITE setFrameWidth)
    Q_PROPERTY(int elementframes MEMBER m_elementFrames WRITE setElementFrames)
    Q_PROPERTY(int start MEMBER m_start WRITE setStart)
    Q_PROPERTY(int end MEMBER m_end WRITE setEnd)
    Q_PROPERTY(int speed MEMBER m_speed WRITE setSpeed)
    Q_PROPERTY(bool realtime MEMBER m_realtime WRITE setRealTime)
    Q_PROPERTY(bool autoreplay MEMBER m_autoReplay WRITE setAutoReplay)
    Q_PROPERTY(bool autoplay MEMBER m_autoPlay WRITE setAutoPlay)
    Q_PROPERTY(bool debug MEMBER m_debug WRITE setDebug)

public:
    AnimatedLayer(const QXmlStreamAttributes &attributes, QWidget *parent = 0);

    //void setW(int w);
    //void setH(int h);
    QString image() const;
    void setImage(const QString &id);
    void setFrameHeight(int value);
    void setFrameWidth(int value);
    void setElementFrames(int value);
    void setStart(int value);
    void setEnd(int value);
    void setSpeed(int value);
    void setRealTime(bool enable);
    void setAutoReplay(bool enable);
    void setAutoPlay(bool enable);
    void setDebug(bool enable);

    Q_INVOKABLE int framesCount();
    Q_INVOKABLE void setFrame(int number);

signals:

public slots:

protected:
    void timerEvent(QTimerEvent *);

private:
    void restart();

    int m_w;
    int m_h;
    bool m_horizontal = true;
    int m_numFrames = 0;
    int m_frameHeight = -1;
    int m_frameWidth = -1;
    int m_elementFrames = -1;
    int m_start = 0;
    int m_end = -1;
    int m_curFrame = 0;
    int m_speed = 200;
    bool m_realtime = false;
    bool m_autoReplay = true;
    bool m_autoPlay = false;
    bool m_debug = false;
    QBasicTimer m_timer;
    QString m_fileId;
    QVector<QPixmap> m_frames;
};

#endif // ANIMATEDLAYER_H
