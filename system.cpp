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
#include <QMetaEnum>

#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/playlistmanager.h>

#include "system.h"
#include "group.h"
#include "script.h"
#include "interpreter.h"

/* Maki runtime version, up to 5.01 = 0.0, 5.02+ = 1.0 */
static const double rtVer = 1.1;

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
    m_plManager = PlayListManager::instance();
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

// It is not clear what the play item string is...
QString System::playItemString() const
{
    PlayListTrack *track = m_plManager->currentPlayList()->currentTrack();
    return m_formatter.format(track);
}

QString System::playItemMetaDataString(const QString &type) const
{
    QMetaEnum meta = QMetaEnum::fromType<MetaDataType>();
    int value = meta.keyToValue(type.toUpper().toLatin1());
    if (value == -1) {
        qWarning("%s: failed to get metadata of type '%s'", Q_FUNC_INFO, qUtf8Printable(type));
        return QString();
    }

    switch (value) {
    case TITLE:
        return m_core->metaData(Qmmp::TITLE);
        break;
    case ALBUM:
        return m_core->metaData(Qmmp::ALBUM);
        break;
    case ARTIST:
        return m_core->metaData(Qmmp::ARTIST);
        break;
    case ALBUMARTIST:
        return m_core->metaData(Qmmp::ALBUMARTIST);
        break;
    case COMMENT:
        return m_core->metaData(Qmmp::COMMENT);
        break;
    case YEAR:
        return m_core->metaData(Qmmp::YEAR);
        break;
    case COMPOSER:
        return m_core->metaData(Qmmp::COMPOSER);
        break;
    case GENRE:
        return m_core->metaData(Qmmp::GENRE);
        break;
    case TRACK:
        return m_core->metaData(Qmmp::TRACK);
        break;
    case DISC:
        return m_core->metaData(Qmmp::DISCNUMBER);
        break;
    case REPLAYGAIN_TRACK_GAIN:
    case REPLAYGAIN_ALBUM_GAIN:
    case REPLAYGAIN_TRACK_PEAK:
    case REPLAYGAIN_ALBUM_PEAK:
        // TODO: Is there any way to get this using qmmp library?
        return QString();
        break;
    case LENGTH:
        return QString::number(m_core->totalTime()); // Should this be in milliseconds?
        break;
    case BITRATE:
        return QString::number(m_core->bitrate());
        break;
        break;
    default:
        qWarning("%s: metadata of type '%s' is not supported", Q_FUNC_INFO, qUtf8Printable(type));
        return QString();
        break;
    }
}

int System::playItemLength() const
{
    return m_core->totalTime();
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

void System::seek(int position)
{
    m_core->seek(position);
}

int System::position() const
{
    return m_core->elapsed();
}

double System::runtimeVersion() const
{
    return rtVer;
}

int System::status() const
{
    switch (m_core->state()) {
    case Qmmp::Playing:
        return Playing;
        break;
    case Qmmp::Paused:
        return Paused;
        break;
    case Qmmp::Stopped:
        return Stopped;
        break;
    default:
        return Stopped;
        break;
    }
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

QString System::intToLongTime(int value)
{
    QString time = MetaDataFormatter::formatLength(value / 1000, false);
    if (time.length() < 6)
        return QString("00:%1").arg(time);
    return time;
}

QString System::intToTime(int value)
{
    QString time = MetaDataFormatter::formatLength(value / 1000, false);
    if (time.length() > 5)
        time.remove(0, 3);
    return time;
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
