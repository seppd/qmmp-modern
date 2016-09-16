/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef GUIOBJECT_H
#define GUIOBJECT_H

#include <QWidget>

#include "object.h"
#include "variable.h"

class QMouseEvent;

class GuiObject : public QWidget
{
    Q_OBJECT
    M_OBJECT
    Q_PROPERTY(int alpha MEMBER m_alpha WRITE setAlpha)
    Q_PROPERTY(int activealpha MEMBER m_activeAlpha WRITE setActiveAlpha)
    Q_PROPERTY(int inactivealpha MEMBER m_inactiveAlpha WRITE setInactiveAlpha)
    Q_PROPERTY(Cursor cursor READ cursor WRITE setCursor)
    Q_PROPERTY(QString tooltip READ toolTip WRITE setToolTip)
    Q_PROPERTY(bool move MEMBER m_moveFlag WRITE setMoveFlag)
    Q_PROPERTY(bool renderbasetexture MEMBER m_renderBaseTexture WRITE setRenderBaseTexture)
    Q_PROPERTY(QString cfgattrib MEMBER m_cfgAttrib WRITE setCfgAttrib)
    Q_PROPERTY(int x READ x WRITE setX)
    Q_PROPERTY(int y READ y WRITE setY)
    Q_PROPERTY(int w READ w WRITE setW)
    Q_PROPERTY(int h READ h WRITE setH)
    Q_PROPERTY(int xRelat READ xRelat WRITE setXRelat)
    Q_PROPERTY(int yRelat READ yRelat WRITE setYRelat)
    Q_PROPERTY(int wRelat READ wRelat WRITE setWRelat)
    Q_PROPERTY(int hRelat READ hRelat WRITE setHRelat)
    Q_PROPERTY(bool fitparent READ fitParent WRITE setFitParent)
    Q_PROPERTY(int x1 READ x1 WRITE setX1)
    Q_PROPERTY(int y1 READ y1 WRITE setY1)
    Q_PROPERTY(int x2 READ x2 WRITE setX2)
    Q_PROPERTY(int y2 READ y2 WRITE setY2)
    Q_PROPERTY(bool anchor READ anchor WRITE setAnchor)
    Q_PROPERTY(bool xSysmetrics READ xSysmetrics WRITE setXSysmetrics)
    Q_PROPERTY(bool ySysmetrics READ ySysmetrics WRITE setYSysmetrics)
    Q_PROPERTY(bool wSysmetrics READ wSysmetrics WRITE setWSysmetrics)
    Q_PROPERTY(bool hSysmetrics READ hSysmetrics WRITE setHSysmetrics)
    Q_PROPERTY(bool rectrgn MEMBER m_rectRgn WRITE setRectRgn)
    Q_PROPERTY(int regionop READ regionOp WRITE setRegionOp)
    Q_PROPERTY(int sysregion READ regionOp WRITE setRegionOp)
    Q_PROPERTY(bool wantfocus READ wantFocus WRITE setWantFocus)
    Q_PROPERTY(bool focusonclick READ focusOnClick WRITE setFocusOnClick)
    Q_PROPERTY(int taborder READ tabOrder WRITE setTabOrder)
    Q_PROPERTY(bool nodblclick READ noDblClick WRITE setNoDblClick)
    Q_PROPERTY(bool noleftclick READ noLeftClick WRITE setNoLeftClick)
    Q_PROPERTY(bool norightclick READ noRightClick WRITE setNoRightClick)
    Q_PROPERTY(bool nomousemove READ noMouseMove WRITE setNoMouseMove)
    Q_PROPERTY(bool nocontextmenu READ noContextMenu WRITE setNoContextMenu)
    Q_PROPERTY(bool ghost READ ghost WRITE setGhost)
    Q_PROPERTY(QString notify0 MEMBER m_notify0 WRITE setNotify0)
    Q_PROPERTY(QString notify1 MEMBER m_notify1 WRITE setNotify1)
    Q_PROPERTY(QString notify2 MEMBER m_notify2 WRITE setNotify2)
    Q_PROPERTY(QString notify3 MEMBER m_notify3 WRITE setNotify3)
    Q_PROPERTY(QString notify4 MEMBER m_notify4 WRITE setNotify4)
    Q_PROPERTY(QString notify5 MEMBER m_notify5 WRITE setNotify5)
    Q_PROPERTY(QString notify6 MEMBER m_notify6 WRITE setNotify6)
    Q_PROPERTY(QString notify7 MEMBER m_notify7 WRITE setNotify7)
    Q_PROPERTY(QString notify8 MEMBER m_notify8 WRITE setNotify8)
    Q_PROPERTY(QString notify9 MEMBER m_notify9 WRITE setNotify9)
    Q_PROPERTY(QString droptarget READ droptarget WRITE setDropTarget)

public:
    explicit GuiObject(QWidget *parent = 0);
    virtual ~GuiObject() {}

    enum Cursor {
        IDC_ARROW = Qt::ArrowCursor,
        IDC_SIZENS = Qt::SizeVerCursor,
        IDC_SIZEWE = Qt::SizeHorCursor,
        IDC_SIZENWSE = Qt::SizeFDiagCursor,
        IDC_SIZENESW = Qt::SizeBDiagCursor,
        IDC_SIZEALL = Qt::SizeAllCursor,
        IDC_IBEAM = Qt::IBeamCursor,
        IDC_WAIT = Qt::WaitCursor,
        IDC_CROSS = Qt::CrossCursor,
        IDC_UPARROW = Qt::UpArrowCursor,
        IDC_NO = Qt::ForbiddenCursor,
        IDC_HAND = Qt::PointingHandCursor,
        IDC_APPSTARTING = Qt::BusyCursor,
        IDC_HELP = Qt::WhatsThisCursor,
    };
    Q_ENUM(Cursor)

    void setPixmap(const QPixmap &pixmap, bool updateMask = true, bool realtime = false);
    QRegion oldMask() const;

    //int alpha() const;
    void setAlpha(int value);

    //int activeAlpha() const;
    void setActiveAlpha(int value);

    //int inactiveAlpha() const;
    void setInactiveAlpha(int value);

    Cursor cursor() const;
    void setCursor(Cursor cursor);

    bool moveFlag() const;
    void setMoveFlag(bool enable);

    //bool renderBaseTexture() const;
    void setRenderBaseTexture(bool enable);

    //QString cfgAttrib() const;
    void setCfgAttrib(const QString &attrib);

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);

    int w() const;
    void setW(int w);

    int h() const;
    void setH(int h);

    int xRelat() const;
    void setXRelat(int mode);

    int yRelat() const;
    void setYRelat(int mode);

    int wRelat() const;
    void setWRelat(int mode);

    int hRelat() const;
    void setHRelat(int mode);

    bool fitParent() const;
    void setFitParent(bool enable);

    int x1() const;
    void setX1(int x1);

    int y1() const;
    void setY1(int y1);

    int x2() const;
    void setX2(int x2);

    int y2() const;
    void setY2(int y2);

    bool anchor() const;
    void setAnchor(bool enable);

    bool xSysmetrics() const;
    void setXSysmetrics(bool enable);

    bool ySysmetrics() const;
    void setYSysmetrics(bool enable);

    bool wSysmetrics() const;
    void setWSysmetrics(bool enable);

    bool hSysmetrics() const;
    void setHSysmetrics(bool enable);

    //bool rectRgn() const;
    void setRectRgn(bool enable);

    int regionOp() const;
    void setRegionOp(int op);

    bool wantFocus() const;
    void setWantFocus(bool enable);

    bool focusOnClick() const;
    void setFocusOnClick(bool enable);

    int tabOrder() const;
    void setTabOrder(int order);

    bool noDblClick() const;
    void setNoDblClick(bool enable);

    bool noLeftClick() const;
    void setNoLeftClick(bool enable);

    bool noRightClick() const;
    void setNoRightClick(bool enable);

    bool noMouseMove() const;
    void setNoMouseMove(bool enable);

    bool noContextMenu() const;
    void setNoContextMenu(bool enable);

    bool ghost() const;
    void setGhost(bool enable);

    //QString notify0() const;
    void setNotify0(const QString &s);

    //QString notify1() const;
    void setNotify1(const QString &s);

    //QString notify2() const;
    void setNotify2(const QString &s);

    //QString notify3() const;
    void setNotify3(const QString &s);

    //QString notify4() const;
    void setNotify4(const QString &s);

    //QString notify5() const;
    void setNotify5(const QString &s);

    //QString notify6() const;
    void setNotify6(const QString &s);

    //QString notify7() const;
    void setNotify7(const QString &s);

    //QString notify8() const;
    void setNotify8(const QString &s);

    //QString notify9() const;
    void setNotify9(const QString &s);

    QString droptarget() const;
    void setDropTarget(const QString &id);

    Q_INVOKABLE GuiObject *findObject(const QString &id) const;
    Q_INVOKABLE int left() const;
    Q_INVOKABLE int top() const;

signals:
    void mouseMoved(VariableList args);
    void leftButtonPressed(VariableList args);
    void leftButtonReleased(VariableList args);
    void rightButtonPressed(VariableList args);
    void rightButtonReleased(VariableList args);
    void entered();
    void leaved();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    //void childEvent(QChildEvent *event);
    //void customEvent(QEvent *event);

private:
    QPixmap m_pixmap;
    QRegion m_oldMask;
    QPoint m_pos;

    enum Position {
        Normal = 0,
        Relative,
        Proportional
    };

    enum Alpha {
        ALPHA_TRANSPARENT = 0,
        ALPHA_OPAQUE = 255
    };

    int m_alpha = 255;
    double m_opacity = 1;
    int m_activeAlpha = 0;
    int m_inactiveAlpha = 0;
    Cursor m_cursor;
    bool m_moveFlag = false;
    bool m_renderBaseTexture = false;
    QString m_cfgAttrib;
    int m_w = 0;
    int m_h = 0;
    int m_xRelat = 0;
    int m_yRelat = 0;
    int m_wRelat = 0;
    int m_hRelat = 0;
    bool m_fitParent = false;
    int m_x1 = 0;
    int m_y1 = 0;
    int m_x2 = 0;
    int m_y2 = 0;
    bool m_anchor = false;
    bool m_xSysMetrics = false;
    bool m_ySysMetrics = false;
    bool m_wSysMetrics = false;
    bool m_hSysMetrics = false;
    bool m_rectRgn = false;
    int m_regionOp = 0;
    bool m_wantFocus = false;
    bool m_focusOnClick = false;
    int m_tabOrder = 0;
    bool m_noDblClick = false;
    bool m_noLeftClick = false;
    bool m_noRightClick = false;
    bool m_noMouseMove = false;
    bool m_noContextMenu = false;
    QString m_notify0;
    QString m_notify1;
    QString m_notify2;
    QString m_notify3;
    QString m_notify4;
    QString m_notify5;
    QString m_notify6;
    QString m_notify7;
    QString m_notify8;
    QString m_notify9;
    QString m_dropTarget;
};

#endif // GUIOBJECT_H
