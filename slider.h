/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef SLIDER_H
#define SLIDER_H

#include <QXmlStreamAttributes>
#include <QMap>
#include <QPoint>
#include <cmath>

#include <qmmp/qmmp.h>

#include "guiobject.h"
#include "bitmap.h"

template <typename T>
class BitmapStorage;

class SoundCore;
class SliderActionHandler;

class Slider : public GuiObject
{
    Q_OBJECT
    Q_PROPERTY(Action action READ action WRITE setAction)
    Q_PROPERTY(QString param MEMBER m_param WRITE setParam)
    Q_PROPERTY(QString barleft READ barLeft WRITE setBarLeft)
    Q_PROPERTY(QString barmiddle READ barMiddle WRITE setBarMiddle)
    Q_PROPERTY(QString barright READ barRight WRITE setBarRight)
    Q_PROPERTY(QString thumb READ thumb WRITE setThumb)
    Q_PROPERTY(QString downthumb READ downThumb WRITE setDownThumb)
    Q_PROPERTY(QString hoverthumb READ hoverThumb WRITE setHoverThumb)
    Q_PROPERTY(QString orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(int low MEMBER m_low WRITE setLow)
    Q_PROPERTY(int high MEMBER m_high WRITE setHigh)

public:
    explicit Slider(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    ~Slider();

    enum Action {
        UNKNOWN = 0,
        VOLUME,
        SEEK,
        EQ_BAND,
    };
    Q_ENUM(Action)

    Slider::Action action() const;
    void setAction(Slider::Action action);

    //QString param() const;
    void setParam(const QString &param);

    QString barLeft() const;
    void setBarLeft(const QString &id);

    QString barMiddle() const;
    void setBarMiddle(const QString &id);

    QString barRight() const;
    void setBarRight(const QString &id);

    QString thumb() const;
    void setThumb(const QString &id);

    QString downThumb() const;
    void setDownThumb(const QString &id);

    QString hoverThumb() const;
    void setHoverThumb(const QString &id);

    QString orientation() const;
    void setOrientation(const QString &value);

    //int low() const;
    void setLow(int value);
    //int high() const;
    void setHigh(int value);

    Q_INVOKABLE void lock();
    Q_INVOKABLE void unlock();

    Q_INVOKABLE int position() const;
    Q_INVOKABLE void setPosition(int position);

signals:
    void moved(double value);
    void released(double value);
    void positionSet(VariableList arg);
    void positionPosted(VariableList arg);
    void finalPositionSet(VariableList arg);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);

private slots:
    void setValue(double value);

private:
    void drawThumb();
    inline void updatePixWeight();
    inline int coordinate(const QPoint &point);
    inline QPoint point();
    inline int thumbSize();

    enum SliderElement {
        Thumb = 0,
        DownThumb,
        HoverThumb,
        BarLeft,
        BarMiddle,
        BarRight,
        TotalElements,
    };

    SoundCore *m_core;
    Action m_action = UNKNOWN;
    QString m_param;
    bool m_horizontal = true;
    bool m_pressed = false;
    bool m_hovered = false;
    int m_size;
    int m_thumbSize = 0;
    int m_drawCoord = 0;
    int m_drawCoordMax = 0;
    int m_pressPos;
    int m_oldValue = 0;
    int m_low = 0;
    int m_high = 255;
    int m_position = 0;
    double m_pixWeight;
    QPixmap m_thumb;
    QPixmap m_pixmap;
    QRegion m_thumbRregion;
    BitmapStorage<SliderElement> *m_storage = Q_NULLPTR;
    SliderActionHandler *m_handler = Q_NULLPTR;
};



inline void Slider::updatePixWeight()
{
    m_pixWeight = (double)(m_size - m_thumbSize) / (m_high - m_low);
}

inline int Slider::coordinate(const QPoint &point)
{
    if (m_horizontal)
        return point.x();
    else
        return point.y();
}

inline QPoint Slider::point() {
    if (m_horizontal)
        return QPoint(m_drawCoord, 0);
    else
        return QPoint(0, m_drawCoord);
}

inline int Slider::thumbSize() {
    if (m_thumb.isNull())
        return 0;
    if (m_horizontal)
        return m_thumb.width();
    else
        return m_thumb.height();
}

class SliderActionHandler
{
public:
    virtual ~SliderActionHandler() {}

//signals:
    virtual void valueChanged(double value) = 0;

//public slots:
    virtual void handleMove(double value) = 0;
    virtual void handleRelease(double value) = 0;
};
Q_DECLARE_INTERFACE(SliderActionHandler, "SliderActionHandler")

class VolumeActionHandler : public QObject, public SliderActionHandler
{
    Q_OBJECT
    Q_INTERFACES(SliderActionHandler)

public:
    VolumeActionHandler(QObject *parent = Q_NULLPTR);

signals:
    void valueChanged(double value);

public slots:
    void handleMove(double value);
    void handleRelease(double value) {Q_UNUSED(value)}

private slots:
    void processVolume(int volume);

private:
    SoundCore *m_core;
};

class SeekActionHandler : public QObject, public SliderActionHandler
{
    Q_OBJECT
    Q_INTERFACES(SliderActionHandler)

public:
    SeekActionHandler(QObject *parent = Q_NULLPTR);

signals:
    void valueChanged(double value);

public slots:
    void handleMove(double value) {Q_UNUSED(value)}
    void handleRelease(double value);

private slots:
    void processState(Qmmp::State state);
    void processElapsed(qint64 elapsed);

private:
    SoundCore *m_core;
    qint64 m_maxValue;
};

class EqBandActionHandler : public QObject, public SliderActionHandler
{
    Q_OBJECT
    Q_INTERFACES(SliderActionHandler)

public:
    EqBandActionHandler(const QString &param, QObject *parent = Q_NULLPTR);

signals:
    void valueChanged(double value);

public slots:
    void handleMove(double value);
    void handleRelease(double value) {Q_UNUSED(value)}

private:
    SoundCore *m_core;
    int m_band;
};

class SliderActionHandlerFactory
{
public:
    static SliderActionHandler *create(Slider::Action action, const QString &param = QString(), QObject *parent = Q_NULLPTR);
};

#endif // SLIDER_H
