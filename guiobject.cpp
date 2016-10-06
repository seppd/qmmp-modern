/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QApplication>
#include <QPainter>
#include <QBitmap>
#include <QMouseEvent>

#include "guiobject.h"
#include "dock.h"

#include <QDebug>

GuiObject::GuiObject(QWidget *parent) : QWidget(parent) {}

void GuiObject::setPixmap(const QPixmap &pixmap, bool updateMask, bool realtime)
{
    if (pixmap.isNull() || m_pixmap.cacheKey() == pixmap.cacheKey())
        return;

    m_pixmap = pixmap;
    if (updateMask) {
        QRegion new_mask = pixmap.mask();
        if (m_rectRgn || new_mask.isEmpty())
                new_mask = pixmap.rect();
        if (m_oldMask != new_mask) {
            m_oldMask = new_mask;
            setMask(new_mask);
        }
    }

    if (Q_UNLIKELY(realtime))
        repaint();
    else
        update();
}

void GuiObject::paintEvent(QPaintEvent *)
{
    if (m_pixmap.isNull())
        return;
    QPainter painter(this);
    painter.setOpacity(m_opacity);
    painter.drawPixmap(0, 0, m_pixmap);
}

void GuiObject::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_moveFlag)
            m_pos = mapTo(window(), event->pos());
        emit leftButtonPressed(VariableList({event->globalX(), event->globalY()}));
    } else if (event->button() == Qt::RightButton) {
        //if (!m_noContextMenu)
            //Show menu
        emit rightButtonPressed(VariableList({event->globalX(), event->globalY()}));
    }
}

void GuiObject::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit leftButtonReleased(VariableList({event->globalX(), event->globalY()}));
    else if (event->button() == Qt::RightButton)
        emit rightButtonReleased(VariableList({event->globalX(), event->globalY()}));
}

void GuiObject::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        leftButtonDoubleClicked(VariableList({event->globalX(), event->globalY()}));
    else if (event->button() == Qt::RightButton)
        rightButtonDoubleClicked(VariableList({event->globalX(), event->globalY()}));
}

void GuiObject::mouseMoveEvent(QMouseEvent *event)
{
    if (m_moveFlag)
        Dock::instance()->move(window(), event->globalPos() - m_pos);

    emit mouseMoved(VariableList({event->globalX(), event->globalY()}));
}

void GuiObject::enterEvent(QEvent *)
{
    emit entered();
}

void GuiObject::leaveEvent(QEvent *)
{
    emit leaved();
}

QRegion GuiObject::oldMask() const
{
    return m_oldMask;
}

void GuiObject::setAlpha(int value)
{
    m_alpha = value;
    m_opacity = (double)value / ALPHA_OPAQUE;
    update();
}

void GuiObject::setActiveAlpha(int value)
{
    m_activeAlpha = value;
    update();
}

void GuiObject::setInactiveAlpha(int value)
{
    m_inactiveAlpha = value;
    update();
}

GuiObject::Cursor GuiObject::cursor() const
{
    return m_cursor;
}

void GuiObject::setCursor(Cursor cursor)
{
    if (m_cursor == cursor)
        return;

    m_cursor = cursor;
    QWidget::setCursor(static_cast<Qt::CursorShape>(cursor));
    update();
}

void GuiObject::setMoveFlag(bool enable)
{
    m_moveFlag = enable;
}

void GuiObject::setRenderBaseTexture(bool enable)
{

}

void GuiObject::setCfgAttrib(const QString &attrib)
{

}

int GuiObject::x() const
{ return QWidget::x(); }

void GuiObject::setX(int x)
{
    switch (xRelat()) {
    case 0:
        QWidget::move(x, y());
        break;
    case 1:
        break;
    case 2:
        break;
    default:
        qWarning("%s: Don't know how to set with xRelat=%i", Q_FUNC_INFO, property("xRelat").toInt());
        return;
    }
    //setMask(mask().translated(x, y()));
}

int GuiObject::y() const
{ return QWidget::y(); }

void GuiObject::setY(int y)
{
    switch (yRelat()) {
    case 0:
        QWidget::move(x(), y);
        break;
    case 1:
        break;
    case 2:
        break;
    default:
        qWarning("%s: Don't know how to set with yRelat=%i", Q_FUNC_INFO, property("yRelat").toInt());
        return;
    }
    //setMask(mask().translated(x(), y));
}

int GuiObject::w() const
{ return m_w; }

void GuiObject::setW(int w)
{
    //qDebug("%s: %i", Q_FUNC_INFO, w);
    m_w = w;
    switch (wRelat()) {
    case 0:
        setFixedWidth(w);
        break;
    case 1:
        break;
    case 2:
        break;
    default:
        qWarning("%s: Don't know how to set with wRelat=%i", Q_FUNC_INFO, property("wRelat").toInt());
    }
}

int GuiObject::h() const
{ return m_h; }

void GuiObject::setH(int h)
{
    //qDebug("%s: %i", Q_FUNC_INFO, h);
    m_h = h;
    switch (hRelat()) {
    case 0:
        setFixedHeight(h);
        break;
    case 1:
        break;
    case 2:
        break;
    default:
        qWarning("%s: Don't know how to set with hRelat=%i", Q_FUNC_INFO, property("hRelat").toInt());
    }
}

int GuiObject::xRelat() const
{ return m_xRelat; }

void GuiObject::setXRelat(int mode)
{
    switch (mode) {
    case Normal:
        QWidget::move(x(), y());
        break;
    case Relative:
        break;
    case Proportional:
        break;
    default:
        qWarning("%s: Don't know how to set to %i", Q_FUNC_INFO, property("xRelat").toInt());
        return;
    }
    m_xRelat = mode;
}

int GuiObject::yRelat() const
{ return m_yRelat; }

void GuiObject::setYRelat(int mode)
{
    switch (mode) {
    case Normal:
        QWidget::move(x(), y());
        break;
    case Relative:
        break;
    case Proportional:
        break;
    default:
        qWarning("%s: Don't know how to set to %i", Q_FUNC_INFO, property("yRelat").toInt());
    }
    m_yRelat = mode;
}

int GuiObject::wRelat() const
{ return m_wRelat; }

void GuiObject::setWRelat(int mode)
{
    switch (mode) {
    case Normal:
        setFixedWidth(width());
        break;
    case Relative:
        break;
    case Proportional:
        break;
    default:
        qWarning("%s: Don't know how to set to %i", Q_FUNC_INFO, property("wRelat").toInt());
    }
    m_wRelat = mode;
}

int GuiObject::hRelat() const
{ return m_hRelat; }

void GuiObject::setHRelat(int mode)
{
    switch (mode) {
    case Normal:
        setFixedHeight(height());
        break;
    case Relative:
        break;
    case Proportional:
        break;
    default:
        qWarning("%s: Don't know how to set to %i", Q_FUNC_INFO, property("hRelat").toInt());
    }
    m_hRelat = mode;
}

bool GuiObject::fitParent() const
{ return m_fitParent; }

void GuiObject::setFitParent(bool enable)
{

}

int GuiObject::x1() const
{ return m_x1; }

void GuiObject::setX1(int x1)
{

}

int GuiObject::y1() const
{ return m_y1; }

void GuiObject::setY1(int y1)
{

}

int GuiObject::x2() const
{ return m_x2; }

void GuiObject::setX2(int x2)
{

}

int GuiObject::y2() const
{ return m_y2; }

void GuiObject::setY2(int y2)
{

}

bool GuiObject::anchor() const
{ return m_anchor; }

void GuiObject::setAnchor(bool enable)
{

}

bool GuiObject::xSysmetrics() const
{ return m_xSysMetrics; }

void GuiObject::setXSysmetrics(bool enable)
{

}

bool GuiObject::ySysmetrics() const
{ return m_ySysMetrics; }

void GuiObject::setYSysmetrics(bool enable)
{

}

bool GuiObject::wSysmetrics() const
{ return m_wSysMetrics; }

void GuiObject::setWSysmetrics(bool enable)
{

}

bool GuiObject::hSysmetrics() const
{ return m_hSysMetrics; }

void GuiObject::setHSysmetrics(bool enable)
{

}

void GuiObject::setRectRgn(bool enable)
{
    if (m_rectRgn == enable)
        return;

    m_rectRgn = enable;

    if (m_pixmap.isNull())
        return;

    if (enable)
        setMask(m_pixmap.rect());
    else
        setMask(m_pixmap.mask());
    update();
}

int GuiObject::regionOp() const
{ return m_regionOp; }

void GuiObject::setRegionOp(int op)
{

}

bool GuiObject::wantFocus() const
{ return m_wantFocus; }

void GuiObject::setWantFocus(bool enable)
{

}

bool GuiObject::focusOnClick() const
{ return m_focusOnClick; }

void GuiObject::setFocusOnClick(bool enable)
{

}

int GuiObject::tabOrder() const
{ return m_tabOrder; }

void GuiObject::setTabOrder(int order)
{

}

bool GuiObject::noDblClick() const
{ return m_noDblClick; }

void GuiObject::setNoDblClick(bool enable)
{

}

bool GuiObject::noLeftClick() const
{ return m_noLeftClick; }

void GuiObject::setNoLeftClick(bool enable)
{

}

bool GuiObject::noRightClick() const
{ return m_noRightClick; }

void GuiObject::setNoRightClick(bool enable)
{

}

bool GuiObject::noMouseMove() const
{ return m_noMouseMove; }

void GuiObject::setNoMouseMove(bool enable)
{
    setMouseTracking(!enable);
}

bool GuiObject::noContextMenu() const
{ return m_noContextMenu; }

void GuiObject::setNoContextMenu(bool enable)
{

}

bool GuiObject::ghost() const
{ return testAttribute(Qt::WA_TransparentForMouseEvents); }

void GuiObject::setGhost(bool enable)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, enable);
}

QString GuiObject::droptarget() const
{ return m_dropTarget; }

void GuiObject::setDropTarget(const QString &id)
{
    m_dropTarget = id;
}

void GuiObject::setNotify0(const QString &s)
{
    m_notify0 = s;
}

void GuiObject::setNotify1(const QString &s)
{
    m_notify1 = s;
}

void GuiObject::setNotify2(const QString &s)
{
    m_notify2 = s;
}

void GuiObject::setNotify3(const QString &s)
{
    m_notify3 = s;
}

void GuiObject::setNotify4(const QString &s)
{
    m_notify4 = s;
}

void GuiObject::setNotify5(const QString &s)
{
    m_notify5 = s;
}

void GuiObject::setNotify6(const QString &s)
{
    m_notify6 = s;
}

void GuiObject::setNotify7(const QString &s)
{
    m_notify7 = s;
}
void GuiObject::setNotify8(const QString &s)
{
    m_notify8 = s;
}

void GuiObject::setNotify9(const QString &s)
{
    m_notify9 = s;
}

GuiObject *GuiObject::findObject(const QString &id) const
{
    return findChild<GuiObject *>(id.toLower(), Qt::FindDirectChildrenOnly);
}

int GuiObject::left() const
{
    return mapToGlobal(QPoint(0,0)).x();
}

int GuiObject::top() const
{
    return  mapToGlobal(QPoint(0,0)).y();
}

