/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This file is based on qmmp source file
 * src/plugins/Ui/skinned/mainvisual.h
 * which is
 * Copyright (C) 2007-2016 by Ilya Kotov
 * forkotov02@hotmail.ru
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QXmlStreamAttributes>
#include <QTimer>

#include <qmmp/visual.h>

#include "guiobject.h"

class QMouseEvent;
class Visualizer;

class VisualizerBase
{
public:
    virtual ~VisualizerBase() {}
    virtual void clear() = 0;
    virtual bool process(float *l, const Visualizer * const v) = 0;
    virtual void draw(QPainter *p, const Visualizer * const v) = 0;
};

class Analyzer : public VisualizerBase
{
public:
    Analyzer();
    ~Analyzer() {}

    void clear();
    bool process(float *l, const Visualizer * const v);
    void draw(QPainter *p, const Visualizer * const v);

private:
    double m_internVisData[75];
    double m_peaks[75];
};

class Scope : public VisualizerBase
{
public:
    Scope();
    ~Scope() {}
    void clear();
    bool process(float *l, const Visualizer * const v);
    void draw(QPainter *p, const Visualizer * const v);

private:
    int m_internVisData[76];
    int m_ratio = 1;
};

class VisualDataStream : public Visual
{
    Q_OBJECT
public:
    VisualDataStream(QWidget *parent = 0);
    ~VisualDataStream() {}

    void add(float *data, size_t samples, int chan);
    void clear();
    float *data();

signals:
    void dataReady();

public slots:
    void timeout();

private:
    QTimer *m_timer = Q_NULLPTR;
    float *m_buffer = Q_NULLPTR;
    int m_buffer_at = 0;
};

class Visualizer : public GuiObject
{
    Q_OBJECT
    Q_PROPERTY(QString colorallbands READ colorAllBands WRITE setColorAllBands)
    Q_PROPERTY(QString colorband1 READ colorBand1 WRITE setColorBand1)
    Q_PROPERTY(QString colorband2 READ colorBand2 WRITE setColorBand2)
    Q_PROPERTY(QString colorband3 READ colorBand3 WRITE setColorBand3)
    Q_PROPERTY(QString colorband4 READ colorBand4 WRITE setColorBand4)
    Q_PROPERTY(QString colorband5 READ colorBand5 WRITE setColorBand5)
    Q_PROPERTY(QString colorband6 READ colorBand6 WRITE setColorBand6)
    Q_PROPERTY(QString colorband7 READ colorBand7 WRITE setColorBand7)
    Q_PROPERTY(QString colorband8 READ colorBand8 WRITE setColorBand8)
    Q_PROPERTY(QString colorband9 READ colorBand9 WRITE setColorBand9)
    Q_PROPERTY(QString colorband10 READ colorBand10 WRITE setColorBand10)
    Q_PROPERTY(QString colorband11 READ colorBand11 WRITE setColorBand11)
    Q_PROPERTY(QString colorband12 READ colorBand12 WRITE setColorBand12)
    Q_PROPERTY(QString colorband13 READ colorBand13 WRITE setColorBand13)
    Q_PROPERTY(QString colorband14 READ colorBand14 WRITE setColorBand14)
    Q_PROPERTY(QString colorband15 READ colorBand15 WRITE setColorBand15)
    Q_PROPERTY(QString colorband16 READ colorBand16 WRITE setColorBand16)
    Q_PROPERTY(QString colorbandpeak READ colorBandPeak WRITE setColorBandPeak)
    Q_PROPERTY(QString colorallosc READ colorAllOsc WRITE setColorAllOsc)
    Q_PROPERTY(QString colorosc1 READ colorOsc1 WRITE setColorOsc1)
    Q_PROPERTY(QString colorosc2 READ colorOsc2 WRITE setColorOsc2)
    Q_PROPERTY(QString colorosc3 READ colorOsc3 WRITE setColorOsc3)
    Q_PROPERTY(QString colorosc4 READ colorOsc4 WRITE setColorOsc4)
    Q_PROPERTY(QString colorosc5 READ colorOsc5 WRITE setColorOsc5)
    Q_PROPERTY(int channel MEMBER m_channel WRITE setChannel)
    Q_PROPERTY(bool fliph MEMBER m_hFlip WRITE setHFlip)
    Q_PROPERTY(bool flipv MEMBER m_vFlip WRITE setVFlip)
    Q_PROPERTY(int mode MEMBER m_mode WRITE setMode)
    Q_PROPERTY(bool peaks MEMBER m_peaks WRITE setPeaks)
    Q_PROPERTY(int peakfalloff READ peakFallOff WRITE setPeakFallOff)
    Q_PROPERTY(int falloff READ fallOff WRITE setFallOff)
    Q_PROPERTY(Coloring coloring MEMBER m_coloring WRITE setColoring)
    Q_PROPERTY(Bandwidth bandwidth MEMBER m_bandwidth WRITE setBandwidth)
    Q_PROPERTY(OscStyle oscstyle MEMBER m_oscstyle WRITE setOscStyle)

public:
    explicit Visualizer(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    ~Visualizer() {}

    friend bool Analyzer::process(float *l, const Visualizer * const v);
    friend void Analyzer::draw(QPainter *painter, const Visualizer * const v);
    friend bool Scope::process(float *l, const Visualizer * const v);
    friend void Scope::draw(QPainter *painter, const Visualizer * const v);

    enum Channel {
        LeftChannel = 1,
        RightChannel = 2,
        Stereo = 3,
    };

    enum Mode {
        NoDisplay = 0,
        Spectroscope = 1,
        Oscilloscope = 2,
    };

    enum FallSpeed {
        Speed0 = 0,
        Speed1,
        Speed2,
        Speed3,
        Speed4,
    };

    enum Coloring {
        NORMAL = 0,
        FIRE,
        LINE,
    };
    Q_ENUM(Coloring)

    enum Bandwidth {
        THIN = 0,
        WIDE,
    };
    Q_ENUM(Bandwidth)

    enum OscStyle {
        SOLID = 0,
        DOTS,
        LINES,
    };
    Q_ENUM(OscStyle)

    QString colorAllBands() const;
    void setColorAllBands(const QString &rgbString);

    QString colorBand1() const;
    void setColorBand1(const QString &rgbString);

    QString colorBand2() const;
    void setColorBand2(const QString &rgbString);

    QString colorBand3() const;
    void setColorBand3(const QString &rgbString);

    QString colorBand4() const;
    void setColorBand4(const QString &rgbString);

    QString colorBand5() const;
    void setColorBand5(const QString &rgbString);

    QString colorBand6() const;
    void setColorBand6(const QString &rgbString);

    QString colorBand7() const;
    void setColorBand7(const QString &rgbString);

    QString colorBand8() const;
    void setColorBand8(const QString &rgbString);

    QString colorBand9() const;
    void setColorBand9(const QString &rgbString);

    QString colorBand10() const;
    void setColorBand10(const QString &rgbString);

    QString colorBand11() const;
    void setColorBand11(const QString &rgbString);

    QString colorBand12() const;
    void setColorBand12(const QString &rgbString);

    QString colorBand13() const;
    void setColorBand13(const QString &rgbString);

    QString colorBand14() const;
    void setColorBand14(const QString &rgbString);

    QString colorBand15() const;
    void setColorBand15(const QString &rgbString);

    QString colorBand16() const;
    void setColorBand16(const QString &rgbString);

    QString colorBandPeak() const;
    void setColorBandPeak(const QString &rgbString);

    QString colorAllOsc() const;
    void setColorAllOsc(const QString &rgbString);

    QString colorOsc1();
    void setColorOsc1(const QString &rgbString);

    QString colorOsc2();
    void setColorOsc2(const QString &rgbString);

    QString colorOsc3();
    void setColorOsc3(const QString &rgbString);

    QString colorOsc4();
    void setColorOsc4(const QString &rgbString);

    QString colorOsc5();
    void setColorOsc5(const QString &rgbString);

    void setChannel(int channel);

    bool setHFlip(bool enable);

    bool setVFlip(bool enable);

    int setMode(int mode);

    bool setPeaks(bool enable);

    int peakFallOff() const;
    void setPeakFallOff(int value);

    int fallOff() const;
    void setFallOff(int value);

    void setColoring(Coloring coloring);

    void setBandwidth(Bandwidth bandwidth);

    void setOscStyle(OscStyle style);

private slots:
    void visualize();

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    VisualDataStream *m_stream = Q_NULLPTR;
    VisualizerBase *m_vis = Q_NULLPTR;
    QPixmap m_pixmap;
    QColor m_colorAllBands;
    QColor m_colorBands[16];
    QColor m_colorBandPeak;
    QColor m_colorAllOsc;
    QColor m_colorOscs[5];
    int m_channel = Stereo;
    bool m_hFlip = false;
    bool m_vFlip = false;
    Mode m_mode = Spectroscope;
    bool m_peaks = false;
    int m_peakFallOffSpeed = Speed2;
    double m_peakFallOff = 0.2;
    int m_fallOffSpeed = Speed2;
    double m_fallOff = 2.2;
    Coloring m_coloring = NORMAL;
    Bandwidth m_bandwidth = THIN;
    OscStyle m_oscstyle = SOLID;
};

#endif // VISUALIZER_H
