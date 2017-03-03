/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This file is based on qmmp source file
 * src/plugins/Ui/skinned/mainvisual.cpp
 * which is
 * Copyright (C) 2007-2016 by Ilya Kotov
 * forkotov02@hotmail.ru
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QMouseEvent>
#include <QPainter>

#include <cmath>

#include "visualizer.h"
#include "fft.h"
#include "inlines.h"

#include <QDebug>

#define VISUAL_NODE_SIZE 512 //samples
#define VISUAL_BUFFER_SIZE (5 * VISUAL_NODE_SIZE)
#define OSC_COLORS_NUM 5.0

Visualizer::Visualizer(const QXmlStreamAttributes &attributes, QWidget *parent) : GuiObject(parent)
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

    // Temp
    m_vis = new Scope(w(), h());

    m_pixmap = QPixmap(w(), h());
    m_stream = new VisualDataStream(this);
    connect(m_stream, SIGNAL(dataReady()), this, SLOT(visualize()), Qt::DirectConnection);
    Visual::add(m_stream);
}

QString Visualizer::colorAllBands() const
{
    const QColor *color = &m_colorAllBands;
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorAllBands(const QString &rgbString)
{
    QStringList rgb = rgbString.split(',');
    QColor color(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
    for (int i = 0; i < 16; i++)
        m_colorBands[i] = color;
}

QString Visualizer::colorBand1() const
{
    const QColor *color = &m_colorBands[0];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand1(const QString &rgbString)
{
    if (colorBand1() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[0] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand2() const
{
    const QColor *color = &m_colorBands[1];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand2(const QString &rgbString)
{
    if (colorBand2() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[1] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand3() const
{
    const QColor *color = &m_colorBands[2];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand3(const QString &rgbString)
{
    if (colorBand3() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[2] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand4() const
{
    const QColor *color = &m_colorBands[3];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand4(const QString &rgbString)
{
    if (colorBand4() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[3] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand5() const
{
    const QColor *color = &m_colorBands[4];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand5(const QString &rgbString)
{
    if (colorBand5() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[4] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand6() const
{
    const QColor *color = &m_colorBands[5];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand6(const QString &rgbString)
{
    if (colorBand6() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[5] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand7() const
{
    const QColor *color = &m_colorBands[6];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand7(const QString &rgbString)
{
    if (colorBand7() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[6] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand8() const
{
    const QColor *color = &m_colorBands[7];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand8(const QString &rgbString)
{
    if (colorBand8() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[7] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand9() const
{
    const QColor *color = &m_colorBands[8];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand9(const QString &rgbString)
{
    if (colorBand9() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[8] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand10() const
{
    const QColor *color = &m_colorBands[9];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand10(const QString &rgbString)
{
    if (colorBand10() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[9] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand11() const
{
    const QColor *color = &m_colorBands[10];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand11(const QString &rgbString)
{
    if (colorBand11() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[10] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand12() const
{
    const QColor *color = &m_colorBands[11];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand12(const QString &rgbString)
{
    if (colorBand12() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[11] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand13() const
{
    const QColor *color = &m_colorBands[12];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand13(const QString &rgbString)
{
    if (colorBand13() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[12] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand14() const
{
    const QColor *color = &m_colorBands[13];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand14(const QString &rgbString)
{
    if (colorBand14() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[13] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand15() const
{
    const QColor *color = &m_colorBands[14];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand15(const QString &rgbString)
{
    if (colorBand15() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[14] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBand16() const
{
    const QColor *color = &m_colorBands[15];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBand16(const QString &rgbString)
{
    if (colorBand16() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBands[15] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorBandPeak() const
{
    const QColor *color = &m_colorBandPeak;
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorBandPeak(const QString &rgbString)
{
    if (colorBandPeak() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorBandPeak = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorAllOsc() const
{
    const QColor *color = &m_colorAllOsc;
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorAllOsc(const QString &rgbString)
{
    QStringList rgb = rgbString.split(',');
    QColor color(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
    for (int i = 0; i < 5; i++)
        m_colorOscs[i] = color;
}

QString Visualizer::colorOsc1()
{
    const QColor *color = &m_colorOscs[0];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorOsc1(const QString &rgbString)
{
    if (colorOsc1() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorOscs[0] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorOsc2()
{
    const QColor *color = &m_colorOscs[1];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorOsc2(const QString &rgbString)
{
    if (colorOsc2() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorOscs[1] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorOsc3()
{
    const QColor *color = &m_colorOscs[2];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorOsc3(const QString &rgbString)
{
    if (colorOsc3() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorOscs[2] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorOsc4()
{
    const QColor *color = &m_colorOscs[3];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorOsc4(const QString &rgbString)
{
    if (colorOsc4() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorOscs[3] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

QString Visualizer::colorOsc5()
{
    const QColor *color = &m_colorOscs[4];
    return QString("%1,%2,%3").arg(color->red()).arg(color->green()).arg(color->blue());
}

void Visualizer::setColorOsc5(const QString &rgbString)
{
    if (colorOsc5() == rgbString)
        return;

    QStringList rgb = rgbString.split(',');
    m_colorOscs[4] = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
}

void Visualizer::setChannel(int channel)
{

}

bool Visualizer::setHFlip(bool enable)
{

}

bool Visualizer::setVFlip(bool enable)
{

}

int Visualizer::setMode(int mode)
{

}

bool Visualizer::setPeaks(bool enable)
{

}

int Visualizer::peakFallOff() const
{
    return m_peakFallOffSpeed;
}

void Visualizer::setPeakFallOff(int value)
{
    if (m_peakFallOffSpeed == value)
        return;

    switch (value) {
    case Speed0:
        m_peakFallOff = 0.05;
        break;
    case Speed1:
        m_peakFallOff = 0.1;
        break;
    case Speed2:
        m_peakFallOff = 0.2;
        break;
    case Speed3:
        m_peakFallOff = 0.4;
        break;
    case Speed4:
        m_peakFallOff = 0.8;
        break;
    default:
        qDebug("%s: wrong value: %i", Q_FUNC_INFO, value);
        return;
    }
    m_peakFallOffSpeed = value;
}

int Visualizer::fallOff() const
{
    return m_fallOffSpeed;
}

void Visualizer::setFallOff(int value)
{
    if (m_fallOffSpeed == value)
        return;

    switch (value) {
    case Speed0:
        m_fallOff = 1.2;
        break;
    case Speed1:
        m_fallOff = 1.8;
        break;
    case Speed2:
        m_fallOff = 2.2;
        break;
    case Speed3:
        m_fallOff = 2.4;
        break;
    case Speed4:
        m_fallOff = 2.8;
        break;
    default:
        qDebug("%s: wrong value: %i", Q_FUNC_INFO, value);
        return;
    }
    m_fallOffSpeed = value;
}

void Visualizer::setColoring(Coloring coloring)
{

}

void Visualizer::setBandwidth(Bandwidth bandwidth)
{

}

void Visualizer::setOscStyle(OscStyle style)
{

}

void Visualizer::mousePressEvent(QMouseEvent *event)
{
    qDebug() << event;
}

void Visualizer::visualize()
{
    if (m_vis == Q_NULLPTR)
        return;

    m_vis->process(m_stream->data(), this);
    m_pixmap.fill(Qt::transparent);
    QPainter painter(&m_pixmap);
    m_vis->draw(&painter, this);
    setPixmap(m_pixmap, false, false);
}

VisualDataStream::VisualDataStream(QWidget *parent) : Visual (parent)
{
    m_buffer = new float[VISUAL_BUFFER_SIZE];
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));

    // Temp
    m_timer->setInterval(40);
    m_timer->start();
}

void VisualDataStream::clear()
{
    m_buffer_at = 0;
}

void VisualDataStream::add(float *data, size_t samples, int chan)
{
    if (!m_timer->isActive())
        return;

    if (VISUAL_BUFFER_SIZE == m_buffer_at) {
        m_buffer_at -= VISUAL_NODE_SIZE;
        memmove(m_buffer, m_buffer + VISUAL_NODE_SIZE, m_buffer_at * sizeof(float));
        return;
    }

    int frames = qMin(int(samples / chan), VISUAL_BUFFER_SIZE - m_buffer_at);
    mono16_from_multichannel(m_buffer + m_buffer_at, data, frames, chan);

    m_buffer_at += frames;
}

float *VisualDataStream::data()
{
    return m_buffer;
}

void VisualDataStream::timeout()
{
    mutex()->lock();
    if (m_buffer_at < VISUAL_NODE_SIZE) {
        mutex()->unlock();
        return;
    }

    emit dataReady();

    m_buffer_at -= VISUAL_NODE_SIZE;
    memmove(m_buffer, m_buffer + VISUAL_NODE_SIZE, m_buffer_at * sizeof(float));

    mutex()->unlock();
}

Analyzer::Analyzer()
{
    clear();
}

void Analyzer::clear()
{
    memset(m_internVisData, 0, sizeof(m_internVisData));
    memset(m_peaks, 0, sizeof(m_peaks));
}

bool Analyzer::process(float *l, const Visualizer * const v)
{
    static fft_state *state = 0;
    if (!state)
        state = fft_init();
    short dest[256];

    static const int xscale_long[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
        35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
        52, 53, 54, 55, 56, 57, 58, 61, 66, 71, 76, 81, 87, 93, 100, 107,
        114, 122, 131, 140, 150, 161, 172, 184, 255
    };

    static const int xscale_short[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 15, 20, 27,
        36, 47, 62, 82, 107, 141, 184, 255
    };

    calc_freq(dest, l);

    const double y_scale = 3.60673760222;   /* 20.0 / log(256) */
    int max = (v->m_bandwidth == Visualizer::THIN) ? 74 : 18, y, j;

    for (int i = 0; i <= max; i++) {
        if (v->m_bandwidth == Visualizer::THIN) {
            for (j = xscale_long[i], y = 0; j < xscale_long[i + 1]; j++) {
                if (dest[j] > y)
                    y = dest[j];
            }
        } else {
            for (j = xscale_short[i], y = 0; j < xscale_short[i + 1]; j++) {
                if (dest[j] > y)
                    y = dest[j];
            }
        }
        y >>= 7;
        int magnitude = 0;
        if (y != 0) {
            magnitude = int(log(y) * y_scale);
            if (magnitude > 15)
                magnitude = 15;
            if (magnitude < 0)
                magnitude = 0;
        }

        m_internVisData[i] -= v->m_fallOff;
        m_internVisData[i] = magnitude > m_internVisData[i]
                               ? magnitude : m_internVisData[i];
        if (v->m_peaks) {
            m_peaks[i] -= v->m_peakFallOff;
            m_peaks[i] = magnitude > m_peaks[i]
                         ? magnitude : m_peaks[i];
        }
    }
    return true;
}

void Analyzer::draw(QPainter *p, const Visualizer * const v)
{
    int r = 1; //m_skin->ratio();
    if (v->m_bandwidth == Visualizer::THIN) {
        for (int j = 0; j <= 74; ++j) {
            for (int i = 0; i <= m_internVisData[j]; ++i) {
                if (v->m_coloring == Visualizer::NORMAL)
                    p->setPen(v->m_colorBands[i]);
                else if (v->m_coloring == Visualizer::FIRE)
                    p->setPen(v->m_colorBands[15 - (int(m_internVisData[j]) - i)]);
                else
                    p->setPen(v->m_colorBands[int(m_internVisData[j])]);
                p->drawPoint(j * r, v->h() - r * i);
            }

            if (v->m_peaks) {
                p->setPen(v->m_colorBandPeak);
                p->drawPoint(j * r, v->h() - r * m_peaks[j]);
            }
        }
    } else {
        for (int j = 0; j <= 18; ++j) {
            for (int i = 0; i <= m_internVisData[j]; ++i) {
                if (v->m_coloring == Visualizer::NORMAL)
                    p->setPen(v->m_colorBands[i]);
                else if (v->m_coloring == Visualizer::FIRE)
                    p->setPen(v->m_colorBands[15 - (int(m_internVisData[j]) - i)]);
                else
                    p->setPen(v->m_colorBands[int(m_internVisData[j])]);

                p->drawLine(j * 4 * r, v->h() - r * i, (j * 4 + 2) * r, v->h() - r * i);
            }

            if (v->m_peaks) {
                p->setPen(v->m_colorBandPeak);
                p->drawLine(j * 4 * r, v->h() - r * m_peaks[j],
                            (j * 4 + 2) * r, v->h() - r * m_peaks[j]);
            }
        }
    }
}

Scope::Scope(int width, int height) : m_width(width), m_height(height)
{
    m_internVisData = new int[m_width];
    clear();
}

Scope::~Scope()
{
    delete [] m_internVisData;
}

void Scope::clear()
{
    for (int i = 0; i < m_width; ++i)
        m_internVisData[i] = m_height / 2;
}

bool Scope::process(float *l, const Visualizer * const v)
{
    Q_UNUSED(v);
    int step = (VISUAL_NODE_SIZE << 8) / m_width;
    int pos = 0;

    for (int i = 0; i < m_width; ++i) {
        pos += step;
        m_internVisData[i] = static_cast<int>(l[pos >> 8] * m_height);
        m_internVisData[i] = qBound(-m_height / 2, m_internVisData[i], m_height / 2);
    }
    return true;
}

void Scope::draw(QPainter *p, const Visualizer * const v)
{
    int half = m_height / 2;
    float weight = (OSC_COLORS_NUM - 1) / half;
    if (v->m_oscstyle == Visualizer::SOLID) {
        for (int i = 0; i < m_width - 1; ++i) {
            int h1 = half - m_internVisData[i];
            int h2 = half - m_internVisData[i + 1];
            if (h1 > h2)
                std::swap(h1, h2);
            p->setPen(v->m_colorOscs[qRound(weight * qAbs(half - h2))]);
            p->drawLine(i * m_ratio, h1 * m_ratio, (i + 1) * m_ratio, h2 * m_ratio);
        }
    } else if (v->m_oscstyle == Visualizer::DOTS) {
        for (int i = 0; i < m_width; ++i) {
            int h = half - m_internVisData[i];
            p->setPen(v->m_colorOscs[qRound(weight * qAbs(m_internVisData[i]))]);
            p->drawPoint(i * m_ratio, h * m_ratio);
        }
    } else {
        for (int i = 0; i < m_width; ++i) {
            int h = half - m_internVisData[i];
            p->setPen(v->m_colorOscs[qRound(weight * qAbs(m_internVisData[i]))]);
            p->drawLine(i * m_ratio, half, i * m_ratio, h);
        }
    }
    memset(m_internVisData, 0, sizeof(int) * m_width);
}
