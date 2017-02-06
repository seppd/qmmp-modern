/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QPainter>
#include <QCursor>
#include <QMouseEvent>

#include "button.h"
#include "bitmap.h"
#include "bitmapstorage.h"
#include "skin.h"
#include "system.h"
#include "container.h"

#include <QDebug>
#include <QApplication> // Temp

Button::Button(QWidget *parent) : GuiObject(parent)
{
    m_storage = new BitmapStorage<ButtonElement>(TotalElements);
}

Button::Button(const QXmlStreamAttributes &attributes, QWidget *parent) : GuiObject(parent)
{
    m_system = System::instance();
    m_storage = new BitmapStorage<ButtonElement>(TotalElements);
    QString name, value;
    Q_FOREACH (const QXmlStreamAttribute &i, attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (name == "action")
            /* Replace ':' and remove spaces to fit the enum definitions */
            value = value.replace(':', '_').replace(' ', "").toUpper();
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }
}

Button::~Button()
{
    delete m_storage;
}

void Button::setAction(Action action)
{
    if (m_action == action)
        return;

    disconnect();
    switch (action) {
    case PREV:
        connect(this, SIGNAL(leftButtonClicked()), m_system, SLOT(previous()));
        break;
    case PLAY:
        connect(this, SIGNAL(leftButtonClicked()), m_system, SLOT(play()));
        break;
    case PAUSE:
        connect(this, SIGNAL(leftButtonClicked()), m_system, SLOT(pause()));
        break;
    case STOP:
        connect(this, SIGNAL(leftButtonClicked()), m_system, SLOT(stop()));
        break;
    case NEXT:
        connect(this, SIGNAL(leftButtonClicked()), m_system, SLOT(next()));
        break;
    case EJECT:
        connect(this, SIGNAL(leftButtonClicked()), m_system, SLOT(eject()));
        break;
    case EJECT_URL:
        break;
    case EJECT_DIR:
        break;
    case VOLUME_UP:
        break;
    case VOLUME_DOWN:
        break;
    case REWIND_5S:
        break;
    case FFWD_5S:
        break;
    case TEXT_LARGER:
        break;
    case TEXT_SMALLER:
        break;
    case DOUBLESIZE:
        break;
    case CONTROLMENU:
        break;
    case SYSMENU:
        break;
    case CLOSE:
        // Temp
        connect(this, SIGNAL(leftButtonClicked()), window(), SLOT(close()));
        //connect(this, SIGNAL(clicked()), qApp, SLOT(quit()));
        //connect(this, SIGNAL(clicked()), qobject_cast<QWidget *>(Skin::instance()->parentWidget()), SLOT(close()));
        break;
    case MINIMIZE:
        break;
    case CLOSE_WINDOW:
        break;
    case TOGGLE_ALWAYS_ON_TOP:
        break;
    case TOGGLE_REPEAT:
        break;
    case TOGGLE_SHUFFLE:
        break;
    case TOGGLE_CROSSFADER:
        break;
    case EQ_TOGGLE:
        break;
    case EQ_AUTO:
        break;
    case MB_BACK:
        break;
    case MB_FORWARD:
        break;
    case MB_STOP:
        break;
    case MB_REFRESH:
        break;
    case MB_HOME:
        break;
    case VIDEO_FULLSCREEN:
        break;
    case VIDEO_SCALE1X:
        break;
    case VIDEO_SCALE2X:
        break;
    case VIDEO_TV:
        break;
    case VIDEO_MISC:
        break;
    case CB_PREV:
        break;
    case CB_NEXT:
        break;
    case SWITCH:
        connect(this, &Button::leftButtonClicked, [this]() {
            QObject *p = this->parent();
            while (p != Q_NULLPTR) {
                Container *c = qobject_cast<Container *>(p);
                if (c != Q_NULLPTR) {
                    m_container = c;
                    break;
                }
                p = p->parent();
            }
            if (m_container == Q_NULLPTR) {
                qWarning("%s: failed to find parent Container", Q_FUNC_INFO);
                return;
            }
            m_container->setLayout(m_param);
        });
        break;
    case TOGGLE:
        break;
    case MENU:
        break;
    case ML_SendTo:
        break;
    case PE_Add:
        break;
    case PE_Rem:
        break;
    case PE_Sel:
        break;
    case PE_Misc:
        break;
    case PE_List:
        break;
    case VIS_Menu:
        break;
    case VIS_Prev:
        break;
    case VIS_Next:
        break;
    case PAN:
        break;
    case EQ_BAND:
        break;
    case WA5_Prefs:
        break;
    case VOLUME:
        break;
    case SEEK:
        break;
    default:
        qWarning("%s: unknown button action: %i", Q_FUNC_INFO, action);
        return;
    }

    m_action = action;
}

QString Button::image() const
{ return m_storage->bitmapId(Image); }

void Button::setImage(const QString &id)
{
    if (m_storage->addBitmap(Image, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
    else if (m_bitmap == Q_NULLPTR) {
        const QPixmap& p = m_storage->pixmap(Image);
        if (p.isNull())
            return;
        setPixmap(p);
        if (w() == 0 || h() == 0)
            setFixedSize(p.size());
    }
}

QString Button::downImage() const
{ return m_storage->bitmapId(DownImage); }

void Button::setDownImage(const QString &id)
{
    if (m_storage->addBitmap(DownImage, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
}

QString Button::hoverImage() const
{ return m_storage->bitmapId(HoverImage); }

void Button::setHoverImage(const QString &id)
{
    if (m_storage->addBitmap(HoverImage, id) == -1)
        qWarning("%s: no '%s' bitmap found", Q_FUNC_INFO, qUtf8Printable(id));
}

void Button::setPressed(bool enable)
{
    if (enable) {
        m_pressed = true;
        const QPixmap& p = m_storage->pixmap(DownImage);
        if (!p.isNull())
            setPixmap(p);
    } else {
        m_pressed = false;
        if (m_hovered) {
            const QPixmap& p = m_storage->pixmap(HoverImage);
            if (!p.isNull())
                setPixmap(p);
        } else {
            const QPixmap& p = m_storage->pixmap(Image);
            if (!p.isNull())
                setPixmap(p);
        }
    }
}

void Button::setHovered(bool enable)
{
    if (enable) {
        const QPixmap& p = m_storage->pixmap(HoverImage);
        if (!p.isNull()) {
            setPixmap(p);
            m_hovered = true;
        }
    } else if (m_hovered) {
        const QPixmap& p = m_storage->pixmap(Image);
        if (!p.isNull())
            setPixmap(p);
        m_hovered = false;
    }
}

void Button::leftClick()
{
    QMouseEvent event(QEvent::MouseButtonPress, mapFromGlobal(QCursor::pos()), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    GuiObject::mousePressEvent(&event);
}

void Button::rightClick()
{
    QMouseEvent event(QEvent::MouseButtonPress, mapFromGlobal(QCursor::pos()), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    GuiObject::mousePressEvent(&event);
}

void Button::mousePressEvent(QMouseEvent *event)
{
    //qDebug("%s: '%s'", Q_FUNC_INFO, qUtf8Printable(property("id").toString()));

    setPressed(true);

    GuiObject::mousePressEvent(event);
}

void Button::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug("%s: '%s'", Q_FUNC_INFO, qUtf8Printable(property("id").toString()));

    if (!m_pressed)
        return;

    setPressed(false);

    if (mask().contains(event->pos())) {
        if (event->button() == Qt::LeftButton)
            emit leftButtonClicked();
        else if (event->button() == Qt::RightButton)
            emit rightButtonClicked();
    }

    GuiObject::mouseReleaseEvent(event);
}

void Button::mouseMoveEvent(QMouseEvent *event)
{
    if (mask().contains(event->pos()))
        setPressed(true);
    else
        setPressed(false);

    GuiObject::mouseMoveEvent(event);
}

void Button::enterEvent(QEvent *event)
{
    setHovered(true);
    GuiObject::enterEvent(event);
}

void Button::leaveEvent(QEvent *event)
{
    setHovered(false);
    GuiObject::leaveEvent(event);
}
