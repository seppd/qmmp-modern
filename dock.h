/*
 *   This file is based on qmmp source file
 *   src/plugins/Ui/skinned/dock.h
 *   which is
 *   Copyright (C) 2007-2016 by Ilya Kotov
 *   forkotov02@hotmail.ru
 */

#ifndef DOCK_H
#define DOCK_H

#include <QObject>

class QWidget;

class Dock : public QObject
{
    Q_OBJECT
public:
    explicit Dock(QObject *parent = 0);
    virtual ~Dock();

    static Dock *instance();
    void setMainWidget(QWidget *widget);
    void addWidget(QWidget *widget);
    void move(QWidget *widget, const QPoint &pos);

signals:

public slots:

private:
    static Dock *m_instance;
    QWidget *m_mainWidget;
    QList<QWidget *> m_widgetList;
    QList<bool> m_dockedList;
};

#endif // DOCK_H
