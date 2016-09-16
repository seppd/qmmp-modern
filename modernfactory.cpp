/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QMessageBox>
#include <QtPlugin>
#include <QTranslator>
#include "mainwindow.h"
#include "modernfactory.h"

const UiProperties ModernFactory::properties() const
{
    UiProperties props;
    props.hasAbout = true;
    props.name = tr("Modern User interface");
    props.shortName = "modern";
    return props;
}

QObject *ModernFactory::create()
{
    return new MainWindow();
}

void ModernFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Qmmp Modern User interface"),
                        tr("Qmmp Modern User interface"));

}

QTranslator *ModernFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    //QString locale = Qmmp::systemLanguageID();
    //translator->load(QString(":/translations/qmmp_modern_") + locale);
    return translator;
}
