/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef SKIN_H
#define SKIN_H

#include <QWidget>
#include <QMap>
#include <QDir>
#include <QStack>
#include <QXmlStreamReader>

#include "debug.h"

Q_DECLARE_LOGGING_CATEGORY(MODERNUI_XML_PARSER)

class MainWindow;
class GuiObject;
class Container;
class Layout;
class Layer;
class AnimatedLayer;
class BitmapFont;
class Group;
class Button;
class ToggleButton;
class Slider;
class GroupDef;
class Text;
class Vis;
class TrueTypeFont;
class Color;
class Script;
class Dock;

class Bitmap;

class Skin : public QWidget
{
    Q_OBJECT
public:
    explicit Skin(QWidget *parent = 0);
    virtual ~Skin() {}

    enum SkinInfo {
        Author = 0,
        Comment,
        Email,
        HomePage,
        Name,
        Screenshot,
        Version,
    };

    static Skin *instance();
    void setSkin(const QString &path);
    QString skinPath() const;
    QString skinInfo(SkinInfo key) const;
    const QString findFile(const QString &name);

    void showUi();

    Container *container(const QString &id) const;
    Bitmap *bitmap(const QString &id) const;
    QObject *font(const QString &id) const;

private:
    static Skin *m_instance;
    static QXmlStreamNamespaceDeclarations m_wxml_decs;

    template <typename T>
    void setObjectAttributes(T *object, const QXmlStreamAttributes &attributes);

    Dock *m_dock;

    MainWindow *m_mw;
    QStack<QWidget *> m_curParent;

    QDir m_skinDir;
    QDir m_includeDir;
    QString m_wxmlVer;

    QMap<SkinInfo, QString> m_skinInfo;
    static const QMap<QString, Skin::SkinInfo> m_validSkinInfo;

    QList<Script *> m_scripts;

    QMap<QString, Container *> m_containers;
    QMap<QString, Group *> m_groups;
    QMap<QString, Bitmap *> m_bitmaps;
    QMap<QString, QObject *> m_fonts;

    void readWasabiVer(QXmlStreamReader &xml);
    void readSkinInfo(QXmlStreamReader &xml);
    void readContainer(QXmlStreamReader &xml);
    void readLayout(QXmlStreamReader &xml);
    void readLayer(QXmlStreamReader &xml);
    void readAnimatedLayer(QXmlStreamReader &xml);
    void readBitmap(QXmlStreamReader &xml);
    void readBitmapFont(QXmlStreamReader &xml);
    void readGroup(QXmlStreamReader &xml);
    void readButton(QXmlStreamReader &xml);
    void readToggleButton(QXmlStreamReader &xml);
    void readSlider(QXmlStreamReader &xml);
    void readGroupDef(QXmlStreamReader &xml);
    void readText(QXmlStreamReader &xml);
    void readVis(QXmlStreamReader &xml);
    void readTrueTypeFont(QXmlStreamReader &xml);
    void readColor(QXmlStreamReader &xml);
    void readStandardFrame(QXmlStreamReader &xml);
    void readScript(QXmlStreamReader &xml);
    void readInclude(QXmlStreamReader &xml);
    void readXmlObjects(QXmlStreamReader &xml);
    static const QMap<QString, void (Skin::*)(QXmlStreamReader &xml)> m_readFuncs;
};

#endif // SKIN_H
