/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <QXmlStreamAttributes>
#include <QScopedPointer>

#include "guiobject.h"

class Bitmap;
class System;
class Container;

template <typename T>
class BitmapStorage;

#if 0
// TODO:
    {"action_target"},
    {"borders"},
    {"cbtarget"},
    {"center_image"},
    {"param"},
    {"retcode"},
    {"style"},
    {"text"}};
#endif

class Button : public GuiObject
{
    Q_OBJECT
    Q_PROPERTY(QString image READ image WRITE setImage)
    Q_PROPERTY(QString downimage READ downImage WRITE setDownImage)
    Q_PROPERTY(QString hoverimage READ hoverImage WRITE setHoverImage)
    Q_PROPERTY(Action action MEMBER m_action WRITE setAction)
    Q_PROPERTY(QString param MEMBER m_param)

public:
    explicit Button(QWidget *parent = 0);
    explicit Button(const QXmlStreamAttributes &attributes, QWidget *parent = 0);
    ~Button();

    Q_INVOKABLE void leftClick();
    Q_INVOKABLE void rightClick();

    enum Action {
        PREV = 0,
        PLAY,
        PAUSE,
        STOP,
        NEXT,
        EJECT,
        EJECT_URL,
        EJECT_DIR,
        VOLUME_UP,
        VOLUME_DOWN,
        REWIND_5S,
        FFWD_5S,
        TEXT_LARGER,
        TEXT_SMALLER,
        DOUBLESIZE,
        CONTROLMENU,
        SYSMENU,
        CLOSE,
        MINIMIZE,
        CLOSE_WINDOW,
        TOGGLE_ALWAYS_ON_TOP,
        TOGGLE_REPEAT,
        TOGGLE_SHUFFLE,
        TOGGLE_CROSSFADER,
        EQ_TOGGLE,
        EQ_AUTO,
        MB_BACK,
        MB_FORWARD,
        MB_STOP,
        MB_REFRESH,
        MB_HOME,
        VIDEO_FULLSCREEN,
        VIDEO_SCALE1X,
        VIDEO_SCALE2X,
        VIDEO_TV,
        VIDEO_MISC,
        CB_PREV,
        CB_NEXT,
        SWITCH,
        TOGGLE,
        MENU,
        ML_SendTo,
        PE_Add,
        PE_Rem,
        PE_Sel,
        PE_Misc,
        PE_List,
        VIS_Menu,
        VIS_Prev,
        VIS_Next,
        PAN,
        EQ_BAND,
        WA5_Prefs,
        VOLUME,
        SEEK
    };
    Q_ENUM(Action)

    Action action() const;
    void setAction(Action action);
    QString image() const;
    void setImage(const QString &id);
    QString downImage() const;
    void setDownImage(const QString &id);
    QString hoverImage() const;
    void setHoverImage(const QString &id);

signals:
    void clicked();

protected:
    //void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void setPressed(bool enable);
    void setHovered(bool enable);

private:
    enum ButtonElement {
        Image = 0,
        DownImage,
        HoverImage,
        TotalElements
    };

    System *m_system;
    Action m_action;
    Bitmap *m_bitmap = Q_NULLPTR;
    BitmapStorage<ButtonElement> *m_storage = Q_NULLPTR;
    bool m_pressed = false;
    bool m_hovered = false;
    QString m_param;
    Container *m_container = Q_NULLPTR;

#if 0
    void actionPrev();
    void actionPause();
    void actionStop();
    void actionNext();
    void actionEject();
    void actionEjectUrl();
    void actionEjectDir();
    void actionVolumeUp();
    void actionVolumeDown();
    void actionRewind5S();
    void actionFFwd5S();
    void actionTextLarger();
    void actionTextSmaller();
    void actionDoubleSize();
    void actionControlMenu();
    void actionSysMenu();
    void actionClose();
    void actionMinimize();
    void actionCloseWindow();
    void actionToggleAlwaysOnTop();
    void actionToggleRepeat();
    void actionToggleShuffle();
    void actionToggleCrossFader();
    void actionEqToggle();
    void actionEqAuto();
    void actionMbBack();
    void actionMbForward();
    void actionMbStop();
    void actionMbRefresh();
    void actionMbHome();
    void actionVideoFullscreen();
    void actionVideoScale1x();
    void actionVideoScale2x();
    void actionVideoTv();
    void actionVideoMusic();
    void actionCBPrev();
    void actionCBNext();
    void actionSwitch();
    void actionToggle();
    void actionMenu();
    void actionMLSendTo();
    void actionPEAdd();
    void actionPERem();
    void actionPESel();
    void actionPEMisc();
    void actionPEList();
    void actionVisMenu();
    void actionVisPrev();
    void actionVisNext();
    void actionPan();
    void actionEqBand();
    void actionTrackMenu();
    void actionWA5Prefs();
    void actionVolume();
    void actionSeek();
#endif
};

#endif // BUTTON_H
