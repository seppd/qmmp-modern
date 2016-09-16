/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef MODERNFACTORY_H
#define MODERNFACTORY_H

#include <QObject>
#include <qmmpui/uifactory.h>

class QTranslator;

class ModernFactory : public QObject, public UiFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qmmp.qmmpui.UiFactoryinterface.1.0")
    Q_INTERFACES(UiFactory)
public:
    const UiProperties properties() const;
    QObject *create();
    void showAbout(QWidget *parent);
    QTranslator *createTranslator(QObject *parent);
};

#endif // MODERNFACTORY_H
