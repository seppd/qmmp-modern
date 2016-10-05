/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Skin;
class System;
class Interpreter;
class Container;

class UiHelper;
class QmmpUiSettings;

#if 1
class MainWindow : public QMainWindow
#else
class MainWindow : public QWidget
#endif
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setContainer(Container *container);

public slots:
    void toggleVisibility();
    void showAndRaise();

protected:
#if 0
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
#endif

private:
    void closeEvent (QCloseEvent *);

    void readSettings();
    void writeSettings();

    Skin *m_skin;
    System *m_system;
    //Container *m_container;
    Interpreter *m_interpreter;
    UiHelper *m_uiHelper;
    QmmpUiSettings *m_uiSettings;
};

#endif // MAINWINDOW_H
