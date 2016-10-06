/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>

#include <qmmpui/metadataformatter.h>

#include "object.h"
#include "variable.h"

class Skin;
class SoundCore;
class MediaPlayer;
class UiHelper;
class PlayListManager;
class Group;
class Script;
class Interpreter;

class System : public QObject
{
    Q_OBJECT
    M_OBJECT
public:
    explicit System(QObject *parent = 0);
    ~System() {}

    enum MetaDataType {
        TITLE = 0,
        ALBUM,
        ARTIST,
        ALBUMARTIST,
        COMMENT,
        YEAR,
        COMPOSER,
        GENRE,
        TRACK,
        DISC,
        REPLAYGAIN_TRACK_GAIN,
        REPLAYGAIN_ALBUM_GAIN,
        REPLAYGAIN_TRACK_PEAK,
        REPLAYGAIN_ALBUM_PEAK,
        LENGTH,
        BITRATE,        
        // The following are not supported
        SRATE,
        STEREO,
        VBR,
        GAIN,
        BPM,
        CONDUCTOR,
        KEY,
        MOOD,
        SUBTITLE,
        LYRICIST,
        ISRC,
        MEDIA,
        REMIXER,
        ENCODER,
        PUBLISHER,
        TOOL,
        PREGAP,
        POSTGAP,
        NUMSAMPLES,
    };
    Q_ENUM(MetaDataType)

    static System *instance();

    Q_INVOKABLE Group *scriptGroup() const;
    Q_INVOKABLE QString playItemString() const;
    Q_INVOKABLE QString playItemMetaDataString(const QString &type) const;
    Q_INVOKABLE int playItemLength() const;
    Q_INVOKABLE int messageBox(const QString &message, const QString &title, int flags, const QString &configEntry);
    Q_INVOKABLE int volume() const;
    Q_INVOKABLE void setVolume(int value);
    Q_INVOKABLE void seek(int position);
    Q_INVOKABLE int position() const;
    Q_INVOKABLE double runtimeVersion() const;
    Q_INVOKABLE int status() const;

    Q_INVOKABLE int privateInt(QString section, QString key, int defaultValue) const;
    Q_INVOKABLE void setPrivateInt(QString section, QString key, int value);
    Q_INVOKABLE QString privateString(QString section, QString key, QString defaultValue) const;
    Q_INVOKABLE void setPrivateString(QString section, QString key, QString value);

    Q_INVOKABLE static int integer(double value);
    Q_INVOKABLE static double fraction(double value);
    Q_INVOKABLE static int stringLength(const QString &string);
    Q_INVOKABLE static QString stringUpper(const QString &string);
    Q_INVOKABLE static QString stringLower(const QString &string);
    Q_INVOKABLE static QString integerToString(int value);
    Q_INVOKABLE static int stringToInteger(const QString &number);
    Q_INVOKABLE static QString floatToString(float value, int precision);
    Q_INVOKABLE static float stringToFloat(const QString &number);
    Q_INVOKABLE static QString integerToLongTime(int value);
    Q_INVOKABLE static QString integerToTime(int value);

signals:
    void playing();
    void paused();
    void stopped();
    void resumed();
    void volumeChanged(VariableList args);

public slots:
    void previous();
    void play();
    void pause();
    void stop();
    void next();
    void eject();

private:
    enum MessageBoxFlag {
        OkFlag          = 0x01,
        CancelFlag      = 0x02,
        YesFlag         = 0x04,
        NoFlag          = 0x08,
        AllFlag         = 0x10,
        NextFlag        = 0x20,
        PreviousFlag    = 0x40,
    };

    enum Status {
        Paused = -1,
        Stopped,
        Playing
    };

    static System *m_instance;
    Skin *m_skin;
    SoundCore *m_core;
    MediaPlayer *m_player;
    UiHelper *m_uiHelper;
    PlayListManager *m_plManager;
    Interpreter *m_interpreter;
    MetaDataFormatter m_formatter;
};

#endif // SYSTEM_H
