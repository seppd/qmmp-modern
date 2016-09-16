/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QDebug>
#include <QMessageBox>

#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>

#include "system.h"
#include "group.h"
#include "script.h"
#include "interpreter.h"

System *System::m_instance = Q_NULLPTR;

System *System::instance()
{
    if (m_instance == Q_NULLPTR)
        m_instance = new System();
    return m_instance;
}

System::System(QObject *parent) : QObject(parent)
{
    m_instance = this;
    m_player = MediaPlayer::instance();
    m_core = SoundCore::instance();
    m_uiHelper = UiHelper::instance();
    m_interpreter = Interpreter::instance();

    connect(m_core, QOverload<int>::of(&SoundCore::volumeChanged), [this](int volume) {
        emit volumeChanged(VariableList({qRound(volume * 2.55)}));
    });
}

/*
 * Should it be direct script parent? Who knows...
 */
Group *System::scriptGroup() const
{
    const Script *s = m_interpreter->runningScript();

    QObject *p = s->parent();
    while (p != Q_NULLPTR) {
        Group *g = qobject_cast<Group *>(p);
        if (g != Q_NULLPTR) {
            return g;
        }
        p = p->parent();
    }

    return Q_NULLPTR;
}

QString System::playItemString() const
{
    return QString();
}

int System::messageBox(const QString &message, const QString &title, int flags, const QString &configEntry)
{
    QMessageBox box;
    box.setWindowTitle(title);
    box.setText(message);

    if (flags & AllFlag)
        flags = OkFlag | CancelFlag | YesFlag | NoFlag | NextFlag | PreviousFlag;

    if (flags & OkFlag)
        box.addButton(QMessageBox::Ok);
    if (flags & CancelFlag)
        box.addButton(QMessageBox::Cancel);
    if (flags & YesFlag)
        box.addButton(QMessageBox::Yes);
    if (flags & NoFlag)
        box.addButton(QMessageBox::No);
    if (flags & NextFlag)
        box.addButton(QMessageBox::Retry);
    if (flags & PreviousFlag)
        box.addButton(QMessageBox::Abort);

    int ret = 0;
    switch (box.exec()) {
    case QMessageBox::Ok:
        ret = OkFlag;
    case QMessageBox::Cancel:
        ret = CancelFlag;
    case QMessageBox::Yes:
        ret = YesFlag;
    case QMessageBox::No:
        ret = NoFlag;
    case QMessageBox::Retry:
        ret = NextFlag;
    case QMessageBox::Abort:
        ret = PreviousFlag;
    }

    if (!configEntry.isEmpty()) {
        QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
        settings.setValue("Modern/" + configEntry, ret);
    }

    return ret;
}

int System::volume() const
{
    return qRound(m_core->volume() * 2.55);
}

void System::setVolume(int value)
{
    if (value < 0 || value > 255)
        return;
    m_core->setVolume(qRound(value / 2.55));
}

QString System::intToString(int value)
{
    return QString::number(value);
}

int System::stringToint(const QString &number)
{
    return number.toInt();
}

QString System::floatToString(float value, int precision)
{
    return QString::number(value, 'f', precision);
}

float System::stringToFloat(const QString &number)
{
    return number.toFloat();
}

void System::play()
{
    m_player->play();
    emit playing();
}

void System::pause()
{
    m_core->pause();
    emit paused();
}

void System::previous()
{
    m_player->previous();
}

void System::next()
{
    m_player->next();
}

void System::stop()
{
    m_player->stop();
    emit stopped();
}

void System::eject()
{
    m_uiHelper->playFiles();
}
