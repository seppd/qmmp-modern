/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <qmmp/soundcore.h>
#include <qmmp/visual.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/commandlinemanager.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/configdialog.h>
#include <qmmpui/qmmpuisettings.h>

#include "mainwindow.h"
#include "interpreter.h"
#include "skin.h"
#include "system.h"
#include "container.h"
#include "layout.h"

#include "windowsystem.h"

#if 1
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
#else
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
#endif
{
#ifdef QMMP_WS_X11
    qDebug("MainWindow: detected wm: %s", qPrintable(WindowSystem::netWindowManagerName()));
#endif

#if 1
#ifdef QMMP_WS_X11
    if (WindowSystem::netWindowManagerName().contains("Marco", Qt::CaseInsensitive) ||
            WindowSystem::netWindowManagerName().contains("Metacity", Qt::CaseInsensitive) ||
            WindowSystem::netWindowManagerName().contains("Mutter", Qt::CaseInsensitive) ||
            WindowSystem::netWindowManagerName().contains("GNOME", Qt::CaseInsensitive)) {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
                       Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);
    } else
#endif
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
                       Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint | Qt::WindowSystemMenuHint);
#endif

    setWindowTitle("Qmmp");

    setAttribute(Qt::WA_TranslucentBackground);

    //m_pl_manager = PlayListManager::instance();
    m_uiHelper = UiHelper::instance();
    m_uiSettings = QmmpUiSettings::instance();

    m_interpreter = new Interpreter(this);
    m_skin = new Skin(this);
    m_system = new System(this);

    //connect(m_uiHelper, SIGNAL(showMainWindowCalled()), SLOT(showAndRaise()));
    //connect(m_uiHelper, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));

    //qDebug() << "Proccessing events...";
    //qApp->processEvents();
    //qDebug() << "Done";

    readSettings();

    show();
    m_skin->showUi();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setContainer(Container *container)
{
    if (container == Q_NULLPTR)
        return;

    setFixedSize(container->size());
    setMask(container->mask());
    setCentralWidget(container);
    container->setFocus();
}

void MainWindow::toggleVisibility()
{
    qDebug("%s", Q_FUNC_INFO);
    if (isHidden() || isMinimized()) {
        show();
        raise();
        activateWindow();
        //m_playlist->setVisible(m_display->isPlaylistVisible());
        //m_equalizer->setVisible(m_display->isEqualizerVisible());
#ifdef QMMP_WS_X11
        if(WindowSystem::netWindowManagerName() == "Metacity") {
            //m_playlist->activateWindow();
            //m_equalizer->activateWindow();
        }
#endif
        qApp->processEvents();
        setFocus ();
        if (isMinimized())
        {
            showNormal();
        }
#ifdef QMMP_WS_X11
        //WindowSystem::changeWinSticky(winId(), ACTION(ActionManager::WM_STICKY)->isChecked());
        WindowSystem::setWinHint(winId(), "player", "Qmmp");
        raise();
#endif
    } else {
#if 0
        if (m_playlist->isVisible())
            m_playlist->hide();
        if (m_equalizer->isVisible())
            m_equalizer->hide();
#endif
        hide();
    }
    qApp->processEvents();
}

void MainWindow::showAndRaise()
{
    qDebug("%s", Q_FUNC_INFO);
    if (isHidden() || isMinimized()) {
        toggleVisibility();
    } else {
        activateWindow();
        raise();
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    m_uiHelper->exit();
}

void MainWindow::readSettings()
{

}

void MainWindow::writeSettings()
{

}


