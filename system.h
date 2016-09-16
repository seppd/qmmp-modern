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

#include "object.h"
#include "variable.h"

class SoundCore;
class MediaPlayer;
class UiHelper;
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

    static System *instance();

    Q_INVOKABLE Group *scriptGroup() const;
    Q_INVOKABLE QString playItemString() const;
    Q_INVOKABLE int messageBox(const QString &message, const QString &title, int flags, const QString &configEntry);
    Q_INVOKABLE int volume() const;
    Q_INVOKABLE void setVolume(int value);

    Q_INVOKABLE static QString intToString(int value);
    Q_INVOKABLE static int stringToint(const QString &number);
    Q_INVOKABLE static QString floatToString(float value, int precision);
    Q_INVOKABLE static float stringToFloat(const QString &number);

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

    static System *m_instance;
    SoundCore *m_core;
    MediaPlayer *m_player;
    UiHelper *m_uiHelper;
    Interpreter *m_interpreter;
};

#endif // SYSTEM_H
