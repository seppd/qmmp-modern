/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QPainter>
#include <QBitmap>
#include <QMouseEvent>

#include <qmmp/eqsettings.h>
#include <qmmpui/mediaplayer.h>

#include "slider.h"
#include "bitmap.h"
#include "bitmapstorage.h"

Slider::Slider(const QXmlStreamAttributes &attributes, QWidget *parent) : GuiObject(parent)
{
    m_core = SoundCore::instance();
    m_storage = new BitmapStorage<SliderElement>(TotalElements);
    QString name, value;
    for (QXmlStreamAttribute i : attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (name == "action")
            value = value.toUpper();
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }

    const QPixmap& p = m_storage->pixmap(Thumb);
    if (p.isNull()) {
        qWarning("%s: no thumb specified", Q_FUNC_INFO);
        return;
    }
    m_thumb = p;
    if (m_horizontal) {
        m_size = w();
        m_thumbSize = m_thumb.width();
        m_drawCoordMax = w() - m_thumbSize;
    } else {
        m_size = h();
        m_thumbSize = m_thumb.height();
        m_drawCoordMax = h() - m_thumbSize;
    }
    m_thumbRregion = m_thumb.mask();
    if (m_thumbRregion.isEmpty())
        m_thumbRregion = m_thumb.rect();

    updatePixWeight();
    drawThumb();    
}

Slider::~Slider()
{
    delete m_storage;
}

Slider::Action Slider::action() const
{
    return m_action;
}

void Slider::setAction(Slider::Action action)
{
    if (m_action == action)
        return;

    SliderActionHandler *handler = SliderActionHandlerFactory::create(action, m_param, this);
    if (handler == Q_NULLPTR)
        return;

    disconnect();

    m_action = action;
    m_handler = handler;

    connect(this, SIGNAL(moved(double)), dynamic_cast<QObject *>(handler), SLOT(handleMove(double)));
    connect(this, SIGNAL(released(double)), dynamic_cast<QObject *>(handler), SLOT(handleRelease(double)));
    connect(dynamic_cast<QObject *>(handler), SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
}

void Slider::setParam(const QString &param)
{
    m_param = param;

    if (m_action != EQ_BAND)
        return;
}

QString Slider::barLeft() const
{ return m_storage->bitmapId(BarLeft); }

void Slider::setBarLeft(const QString &id)
{
    if (m_storage->addBitmap(BarLeft, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
}

QString Slider::barMiddle() const
{ return m_storage->bitmapId(BarMiddle); }

void Slider::setBarMiddle(const QString &id)
{
    if (m_storage->addBitmap(BarMiddle, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
}

QString Slider::barRight() const
{ return m_storage->bitmapId(BarRight); }

void Slider::setBarRight(const QString &id)
{
    if (m_storage->addBitmap(BarRight, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
}

QString Slider::thumb() const
{ return m_storage->bitmapId(Thumb); }

void Slider::setThumb(const QString &id)
{
    if (m_storage->addBitmap(Thumb, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
}

QString Slider::downThumb() const
{ return m_storage->bitmapId(DownThumb); }

void Slider::setDownThumb(const QString &id)
{
    if (m_storage->addBitmap(DownThumb, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
}

QString Slider::hoverThumb() const
{ return m_storage->bitmapId(HoverThumb); }

void Slider::setHoverThumb(const QString &id)
{
    if (m_storage->addBitmap(HoverThumb, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
    else
        setMouseTracking(true);
}

QString Slider::orientation() const
{
    if (m_horizontal)
        return "h";
    else
        return "v";
}

void Slider::setOrientation(const QString &value)
{
    if (value == "v" || value == "vertical") {
        m_horizontal = false;
        update();
    }
}

void Slider::setLow(int value)
{
    if (m_low == value)
        return;

    m_low = value;
    updatePixWeight();
}

void Slider::setHigh(int value)
{
    if (m_high == value)
        return;

    m_high = value;
    updatePixWeight();
}

int Slider::position() const
{
    return ceil(m_drawCoord * (double)(m_high - m_low) / (m_size - m_thumbSize));
}

void Slider::setPosition(int position)
{
    if ((m_position == position) || (position < m_low) || (position > m_high))
        return;

    m_position = position;
    m_drawCoord = ceil(position * m_pixWeight);
    drawThumb();
}

void Slider::setValue(double value)
{
    if (m_pressed)
        return;
    setPosition(ceil(value * (m_high - m_low)));
}

void Slider::lock()
{
    Q_UNIMPLEMENTED();
}

void Slider::unlock()
{
    Q_UNIMPLEMENTED();
}

void Slider::drawThumb()
{
    m_pixmap = QPixmap(w(), h());
    m_pixmap.fill(Qt::transparent);
    QPainter painter(&m_pixmap);
    painter.drawPixmap(point(), m_thumb);
    setPixmap(m_pixmap, false);
}

void Slider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        m_pressPos = coordinate(event->pos());
        if (m_pressPos < m_drawCoord || (m_drawCoord + m_thumbSize - 1) < m_pressPos) {
            m_drawCoord = m_pressPos - m_thumbSize / 2;
            if (m_drawCoord > m_drawCoordMax)
                m_drawCoord = m_drawCoordMax;
            else if (m_drawCoord < 0)
                m_drawCoord = 0;
        }
        m_pressPos -= m_drawCoord;
        const QPixmap& p = m_storage->pixmap(DownThumb);
        if (!p.isNull()) {
            m_thumb = p;
            drawThumb();
        }
    }

    GuiObject::mousePressEvent(event);
}

void Slider::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressed = false;
    QPixmap p;
    if (m_hovered)
        p = m_storage->pixmap(HoverThumb);
    else
        p = m_storage->pixmap(Thumb);
    if (!p.isNull()) {
        m_thumb = p;
        drawThumb();
    }

    emit released((double)m_drawCoord / (m_size - m_thumbSize));
    GuiObject::mouseReleaseEvent(event);
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pressed) {
        m_drawCoord = coordinate(event->pos()) - m_pressPos;
        if (m_drawCoord < 0)
            m_drawCoord = 0;
        else if (m_drawCoord > m_drawCoordMax)
            m_drawCoord = m_drawCoordMax;
        drawThumb();
        emit moved((double)m_drawCoord / (m_size - m_thumbSize));
    } else {
        QPixmap p;
        bool contains = m_thumbRregion.translated(point()).contains(event->pos());
        if (contains) {
            p = m_storage->pixmap(HoverThumb);
            if (!p.isNull())
                m_hovered = true;
        } else {
            p = m_storage->pixmap(Thumb);
            if (!p.isNull())
                m_hovered = false;
        }

        if (!p.isNull()) {
            m_thumb = p;
            drawThumb();
        }
    }

    GuiObject::mouseMoveEvent(event);
}

void Slider::leaveEvent(QEvent *)
{
    if (!m_hovered)
        return;
    m_hovered = false;
    const QPixmap& p = m_storage->pixmap(Thumb);
    if (!p.isNull()) {
        m_thumb = p;
        drawThumb();
    }
}

VolumeActionHandler::VolumeActionHandler(QObject *parent) : QObject(parent)
{
    m_core = SoundCore::instance();
    connect(m_core, SIGNAL(volumeChanged(int)), this, SLOT(processVolume(int)));
}

void VolumeActionHandler::processVolume(int volume)
{
    emit valueChanged((double)volume / 100);
}

void VolumeActionHandler::handleMove(double value)
{
    m_core->setVolume(value);
}

SeekActionHandler::SeekActionHandler(QObject *parent) : QObject(parent)
{
    m_core = SoundCore::instance();
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), this, SLOT(processState(Qmmp::State)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), this, SLOT(processElapsed(qint64)));
}

void SeekActionHandler::processState(Qmmp::State state)
{
    switch (state) {
    case Qmmp::Playing:
        m_maxValue = m_core->totalTime();
        break;
    case Qmmp::Stopped:
        emit valueChanged(0);
        break;
    default:
        break;
    }
}

void SeekActionHandler::processElapsed(qint64 elapsed)
{
    emit valueChanged((double)elapsed / m_maxValue);
}

void SeekActionHandler::handleRelease(double value)
{
    m_core->seek(ceil(value * m_maxValue));
}

EqBandActionHandler::EqBandActionHandler(const QString &param, QObject *parent) : QObject(parent)
{
    if (param.isEmpty())
        return;

    m_band = param.toInt();
}

void EqBandActionHandler::handleMove(double value)
{

}

SliderActionHandler *SliderActionHandlerFactory::create(Slider::Action action, const QString &param, QObject *parent)
{
    switch (action) {
    case Slider::VOLUME:
        return new VolumeActionHandler(parent);
        break;
    case Slider::SEEK:
        return new SeekActionHandler(parent);
        break;
    case Slider::EQ_BAND:
        return new EqBandActionHandler(param, parent);
        break;
    default:
        qWarning("%s: unknown slider action: %i", Q_FUNC_INFO, action);
        break;
    }
    return Q_NULLPTR;
}
