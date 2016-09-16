/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef TEXT_H
#define TEXT_H

#include <QXmlStreamAttributes>
#include <QTimer>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>

#include <qmmp/qmmp.h>
#include <qmmpui/metadataformatter.h>

#include "guiobject.h"

class BitmapFont;
class TrueTypeFont;
class SoundCore;
class TextDisplayProvider;

class Text : public GuiObject
{
    Q_OBJECT
    Q_PROPERTY(Display display READ display WRITE setDisplay)
    Q_PROPERTY(bool ticker MEMBER m_ticker WRITE setTicker)
    Q_PROPERTY(bool antialias MEMBER m_antiAlias WRITE setAntiAlias)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString font READ font WRITE setFont)
    Q_PROPERTY(int fontsize READ fontSize WRITE setFontSize)
    Q_PROPERTY(Text::Alignment align READ align WRITE setAlign)
    Q_PROPERTY(Text::Alignment valign READ vAlign WRITE setVAlign)
    Q_PROPERTY(QString color READ color WRITE setColor)
    Q_PROPERTY(QString shadowcolor READ shadowColor WRITE setShadowColor)
    Q_PROPERTY(int xShadow READ xShadow WRITE setXShadow)
    Q_PROPERTY(int yShadow READ yShadow WRITE setYShadow)
    Q_PROPERTY(int timeroffstyle READ timerOffStyle WRITE setTimerOffStyle)
    Q_PROPERTY(int timecolonwidth MEMBER m_timeColonWidth WRITE setTimeColonWidth)
    Q_PROPERTY(bool nograb MEMBER m_noGrab WRITE setNoGrab)
    Q_PROPERTY(bool showlen MEMBER m_showLen WRITE setShowLen)
    Q_PROPERTY(bool forcefixed READ forceFixed WRITE setForceFixed)
    Q_PROPERTY(bool forceupcase READ forceUpperCase WRITE setForceUpperCase)
    Q_PROPERTY(bool forceuppercase READ forceUpperCase WRITE setForceUpperCase)
    Q_PROPERTY(bool forcelocase READ forceLowerCase WRITE setForceLowerCase)
    Q_PROPERTY(bool forcelowercase READ forceLowerCase WRITE setForceLowerCase)
    Q_PROPERTY(bool bold READ bold WRITE setBold)
    Q_PROPERTY(bool wrap READ wrap WRITE setWrap)
    Q_PROPERTY(int xOffset READ xOffset WRITE setXOffset)
    Q_PROPERTY(int yOffset READ yOffset WRITE setYOffset)
    Q_PROPERTY(QString dblclickaction READ dblClickAction WRITE setDblClickAction)

public:
    explicit Text(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    virtual ~Text() {}

    enum Display {
        UNKNOWN = -1,
        SONGNAME,
        SONGINFO,
        SONGARTIST,
        SONGTITLE,
        SONGALBUM,
        SONGLENGTH,
        TIME,
        TIMEELAPSED,
        TIMEREMAINING,
        COMPONENTBUCKET,
        SONGBITRATE,
        SONGSAMPLERATE
    };
    Q_ENUM(Display)

    enum Alignment {
        UNSET = -1,
        LEFT,
        CENTER,
        RIGHT,
        TOP,
        BOTTOM,
    };
    Q_ENUM(Alignment)

    Text::Display display() const;
    void setDisplay(Text::Display display);

    bool ticker() const;
    void setTicker(bool enable);

    bool antiAlias() const;
    void setAntiAlias(bool enable);

    Q_INVOKABLE QString text() const;
    Q_INVOKABLE void setText(const QString &text);
    Q_INVOKABLE void setAlternateText(const QString &text);

    QString font() const;
    void setFont(const QString &id);

    Text::Alignment align() const;
    void setAlign(Text::Alignment align);

    Text::Alignment vAlign() const;
    void setVAlign(Text::Alignment valign);

    QString color() const;
    void setColor(const QString &rgbString);

    QString shadowColor() const;
    void setShadowColor(const QString &rgbString);

    int fontSize() const;
    void setFontSize(int size);

    int xShadow() const;
    void setXShadow(int x);

    int yShadow() const;
    void setYShadow(int y);

    int timerOffStyle() const;
    void setTimerOffStyle(int style);

    void setTimeColonWidth(int width);

    bool noGrab() const;
    void setNoGrab(bool enable);

    bool showLen() const;
    void setShowLen(bool enable);

    bool forceFixed() const;
    void setForceFixed(bool enable);

    bool forceUpperCase() const;
    void setForceUpperCase(bool enable);

    bool forceLowerCase() const;
    void setForceLowerCase(bool enable);

    bool bold() const;
    void setBold(bool enable);

    bool wrap() const;
    void setWrap(bool enable);

    int xOffset() const;
    void setXOffset(int x);

    int yOffset() const;
    void setYOffset(int y);

    QString dblClickAction() const;
    void setDblClickAction(const QString &action);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void addOffset();
    void displayText(QString text);
    void processState(Qmmp::State state);

private:
    enum TimerOffStyle {
        Colon = 0,
        Zeroes,
        Empty
    };

    void updateText();
    void drawBitmapText(const QString &text, QPainter *painter);

    SoundCore *m_core;
    BitmapFont *m_bmf = Q_NULLPTR;
    TrueTypeFont *m_ttf = Q_NULLPTR;
    bool m_antiAlias = false;
    int m_timeColonWidth = -1;
    bool m_bitmap = false;
    QString m_text;
    QString m_displayText;
    bool m_altTextDisplayed;
    QFont m_font;
    QColor m_color;
    QString m_fontId;
    QPixmap m_pixmap;
    Display m_display = UNKNOWN;
    QVariant m_data = 0;
    int m_offStyle = 0;
    bool m_ticker = false;
    bool m_scroll = false;
    bool m_scrollBack = true;
    int m_xTicker = 0;
    int m_tickerMin;
    bool m_pressed;
    int m_pressPos;
    Text::Alignment m_halign = Text::UNSET;
    Text::Alignment m_valign = Text::UNSET;
    Qt::Alignment m_align = Qt::AlignLeft | Qt::AlignCenter;
    bool m_noGrab = false;
    bool m_showLen = false;
    QTimer *m_timer = Q_NULLPTR;
    QGraphicsDropShadowEffect *m_effect = Q_NULLPTR;
    TextDisplayProvider *m_provider = Q_NULLPTR;
    MetaDataFormatter m_formatter;
};


class TextDisplayProvider
{
public:
    virtual ~TextDisplayProvider() {}

//signals:
    virtual void textChanged(QString text) = 0;
};
Q_DECLARE_INTERFACE(TextDisplayProvider, "TextDisplayProvider")

class MetaDataProvider : public QObject, public TextDisplayProvider
{
    Q_OBJECT
    Q_INTERFACES(TextDisplayProvider)

public:
    MetaDataProvider(Text::Display display, QObject *parent = Q_NULLPTR);

signals:
    void textChanged(QString text);

private slots:
    void processMetaData();

private:
    SoundCore *m_core;
    MetaDataFormatter m_formatter;
};

class TimeProvider : public QObject, public TextDisplayProvider
{
    Q_OBJECT
    Q_INTERFACES(TextDisplayProvider)

public:
    TimeProvider(Text::Display display, QObject *parent = Q_NULLPTR);

signals:
    void textChanged(QString text);

private slots:
    void processTime(qint64 elapsed);
    void processState(Qmmp::State state);

private:
    bool m_remaining = false;
    qint64 m_seconds = 0;
    qint64 m_totalSeconds;
    SoundCore *m_core;
};

class BitRateProvider : public QObject, public TextDisplayProvider
{
    Q_OBJECT
    Q_INTERFACES(TextDisplayProvider)

public:
    BitRateProvider(QObject *parent = Q_NULLPTR);

signals:
    void textChanged(QString text);

private slots:
    void processBitRate(int rate);
};

class SampleRateProvider : public QObject, public TextDisplayProvider
{
    Q_OBJECT
    Q_INTERFACES(TextDisplayProvider)

public:
    SampleRateProvider(QObject *parent = Q_NULLPTR);

signals:
    void textChanged(QString text);

private slots:
    void processSampleRate(quint32 rate);
};

class TextDisplayProviderFactory
{
public:
    static TextDisplayProvider *create(Text::Display display, QObject *parent = Q_NULLPTR);
};

#endif // TEXT_H
