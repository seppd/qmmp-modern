/*
 * Copyright (C) 2016 Dmitry Morozov <dmorozov@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <QFile>
#include <QApplication>
#include <QSettings>
#include <QFileDialog>

#include "skin.h"
#include "skinreader.h"
#include "mainwindow.h"
#include "container.h"
#include "layout.h"
#include "group.h"
#include "layer.h"
#include "button.h"
#include "togglebutton.h"
#include "slider.h"
#include "text.h"
#include "truetypefont.h"
#include "bitmapfont.h"
#include "bitmap.h"
#include "animatedlayer.h"
#include "script.h"
#include "interpreter.h"
#include "dock.h"
#include "windowsystem.h"

Q_LOGGING_CATEGORY(MODERNUI_XML_PARSER, "modernui.xml.parser", QtWarningMsg)

Skin *Skin::m_instance = Q_NULLPTR;

Skin *Skin::instance()
{
    if (!m_instance)
        m_instance = new Skin();
    return m_instance;
}

Skin::Skin(QWidget *parent) : QWidget(parent)
{
    m_instance = this;
    m_mw = qobject_cast<MainWindow *>(parent);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QString path = settings.value("Modern/skin_path").toString();
    if (path.isEmpty() || !QDir(path).exists()) {
        path = QFileDialog::getOpenFileName(this, tr("Select Skin File"), QDir::homePath(),
                                            tr("Skin Files (*.wal *.zip *.tar.bz2 *.tar.gz *.tgz)"));
        if (path.isEmpty()) {
            window()->close();
            return;
        }

        SkinReader *reader = new SkinReader(this);
        reader->unpackSkin(path);
        path = Qmmp::configDir() + "cache/skin";
        reader->deleteLater();
    }
    m_dock = new Dock(this);
    m_dock->setMainWidget(m_mw);

    setSkin(QDir::cleanPath(path));
}

void Skin::setSkin(const QString &path)
{
    m_skinDir = m_includeDir = path;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Modern/skin_path", path);

    //m_cur_container = 0;

    mCDebug(MODERNUI_XML_PARSER) << "path: " << path;
    QString skin_xml(m_skinDir.absolutePath() + "/skin.xml");
    QFile file(skin_xml);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << skin_xml << ": " << qUtf8Printable(file.errorString());
        return;
    }

    QXmlStreamReader xml;
    xml.setDevice(&file);
    xml.addExtraNamespaceDeclarations(m_wxml_decs);
    readXmlObjects(xml);

    QList<Script *>::Iterator i = m_scripts.begin();
    while (i != m_scripts.end()) {
        if ((*i)->load()/*m_system->loadScript(*i)*/ == -1) {
            delete *i;
            i = m_scripts.erase(i);
        } else {
            i++;
        }
    }
}

QString Skin::skinPath() const
{
    return m_skinDir.absolutePath();
}

Container *Skin::container(const QString &id) const
{
   if (m_containers.contains(id))
       return m_containers.value(id);
   return Q_NULLPTR;
}

Bitmap *Skin::bitmap(const QString &id) const
{
   if (m_bitmaps.contains(id))
       return m_bitmaps.value(id);
   return Q_NULLPTR;
}

QObject *Skin::font(const QString &id) const
{
    if (m_fonts.contains(id))
        return m_fonts.value(id);
    return Q_NULLPTR;
}

void Skin::showUi()
{
    Container *main = container("main");
    if (main == Q_NULLPTR)
        qFatal("%s: main container not found", Q_FUNC_INFO);

    //qApp->processEvents();

    for (QString id : m_containers.keys()) {
        Container *c = m_containers.value(id);
        if (c->defaultVisible()/* && c != main*/) {
            qInfo() << c->parent() << c->objectName() << c->windowFlags();
            c->setLayout("normal");
            c->adjustSize();
            c->show();
            c->raise();
            c->activateWindow();
        }
    }

    m_mw->setFixedSize(main->size());
    m_mw->setMask(main->mask());
    m_mw->setCentralWidget(main);
    main->setFocus();
}

void Skin::readXmlObjects(QXmlStreamReader &xml)
{
    QString name;
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartDocument)
            continue;
        name = xml.name().toString().toLower();
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (m_readFuncs.contains(name)) {
                (this->*(m_readFuncs.value(name)))(xml);
            } else if (name != "includeroot" && name != "elements") {
                qWarning() << "no parser handler for" << name;
            }
        } else if (xml.tokenType() == QXmlStreamReader::EndElement) {
            if (name == "container" || name == "layout" || name == "groupdef")
                m_curParent.pop();
        }
    }
}

void Skin::readWasabiVer(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();

    m_wxmlVer = attributes.value("version").toString();
    mCDebug(MODERNUI_XML_PARSER) << "\twxml=" << m_wxmlVer;
}

void Skin::readSkinInfo(QXmlStreamReader &xml)
{
    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "skininfo"))
    {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            QString name = xml.name().toString();

            if (isSkinInfoTag(name)) {
                xml.readNext();
                m_skinInfo.insert(name, xml.text().toString());
            }
        }
        xml.readNext();
    }

    for (auto i : m_skinInfo.keys())
        mCDebug(MODERNUI_XML_PARSER) << "\t" << i << "=" << m_skinInfo.value(i);
}

void Skin::readContainer(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QString id = attributes.value("id").toString();

    if (!m_containers.contains(id)) {
        Container *c = new Container(attributes, m_mw);
        mCDebug(MODERNUI_XML_PARSER) << "\t" << c << " parent: " << this;
        if (id.toLower() != "main") {
            m_dock->addWidget(c);
#ifdef QMMP_WS_X11
            QString wm_name = WindowSystem::netWindowManagerName();
            if (wm_name.contains("metacity", Qt::CaseInsensitive) ||
                    wm_name.contains("openbox", Qt::CaseInsensitive))
                c->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
            else
#endif
                c->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        }
        m_containers.insert(id, c);
        m_curParent.push(c);
    } else {
        qWarning() << "container '" << id << "' already allocated";
        m_curParent.push(qobject_cast<QWidget *>(m_containers.value(id)));
    }
}

void Skin::readLayout(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    //QString id = attributes.value("id").toString();

    Layout *lo = new Layout(attributes, m_curParent.top());
    mCDebug(MODERNUI_XML_PARSER) << "\t" << lo << " parent: " << m_curParent.top();

    if (!xml.attributes().value("background").isEmpty() ||
            !xml.attributes().value("alphabackground").isEmpty()) {
        qWarning() << "support for 'background' attribute is not implemented";
    }

    m_curParent.push(lo);
}

void Skin::readLayer(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    //QString id = attributes.value("id").toString();

    Layer *l = new Layer(attributes, m_curParent.top());
    mCDebug(MODERNUI_XML_PARSER) << "\t" << l << " parent: " << m_curParent.top();
}

void Skin::readAnimatedLayer(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    //QString id = attributes.value("id").toString();

    AnimatedLayer *al =  new AnimatedLayer(attributes, m_curParent.top());
    mCDebug(MODERNUI_XML_PARSER) << "\t" << al << " parent: " << m_curParent.top();
}

void Skin::readBitmap(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QString id = attributes.value("id").toString();

    Bitmap *bm = new Bitmap(attributes, this);
    mCDebug(MODERNUI_XML_PARSER) << "\t" << bm;

    m_bitmaps.insert(id, bm);
}

void Skin::readBitmapFont(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QString id = attributes.value("id").toString();

    BitmapFont *bmf = new BitmapFont(attributes, this);
    mCDebug(MODERNUI_XML_PARSER) << "\t" << bmf;
    m_fonts.insert(id, bmf);
    //ints.insert(id, bmf);
}

void Skin::readTrueTypeFont(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QString id = attributes.value("id").toString();

    TrueTypeFont *ttf = new TrueTypeFont(attributes, this);
    mCDebug(MODERNUI_XML_PARSER) << "\t" << ttf;
    m_fonts.insert(id, ttf);
    //ints.insert(id, ttf);
}

void Skin::readButton(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    //QString id = attributes.value("id").toString();

    // Does this matter?
    if (xml.namespaceUri() == "WasabiXML") {
        mCDebug(MODERNUI_XML_PARSER) << "processing Wasabi button";
    } else {
        Button *b = new Button(attributes, m_curParent.top());
        mCDebug(MODERNUI_XML_PARSER) << "\t" << b << " parent: " << m_curParent.top();
    }    
}

void Skin::readToggleButton(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    //QString id = attributes.value("id").toString();

    // Does this matter?
    if (xml.namespaceUri() == "WasabiXML") {
        mCDebug(MODERNUI_XML_PARSER) << "processing Wasabi toggle button";
    } else {
        ToggleButton *tb = new ToggleButton(attributes, m_curParent.top());
        mCDebug(MODERNUI_XML_PARSER) << "\t" << tb << " parent: " << m_curParent.top();
        //qDebug("%s: %p parent=%p", Q_FUNC_INFO, b, b->parent());
    }
}

void Skin::readSlider(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    //QString id = attributes.value("id").toString();

    Slider *s = new Slider(attributes, m_curParent.top());
    mCDebug(MODERNUI_XML_PARSER) << "\t" << s << " parent: " << m_curParent.top();
}

void Skin::readGroup(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QString id = attributes.value("id").toString();

    if (m_groups.contains(id)) {
        Group *g = m_groups.value(id);
        g->setParent(m_curParent.top());
        mCDebug(MODERNUI_XML_PARSER) << "\t" << g << " parent: " << m_curParent.top();
        QString name, value;
        for (QXmlStreamAttribute i : attributes) {
            name = i.name().toString().toLower();
            value = i.value().toString();
            if (!g->setProperty(qPrintable(name), QVariant(value))) {
                if (g->dynamicPropertyNames().contains(qPrintable(name))) {
                    qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                    g->setProperty(qPrintable(name), QVariant());
                }
                qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO,
                         qUtf8Printable(name), qUtf8Printable(value));
            }
        }
    } else {
        qWarning("%s: group '%s' was not defined by groupdef", Q_FUNC_INFO, qUtf8Printable(id));
    }
}

void Skin::readGroupDef(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QString id = attributes.value("id").toString();

    if (!m_groups.contains(id)) {
        Group *g = new Group(attributes, this);
        mCDebug(MODERNUI_XML_PARSER) << "\t" << g;
        m_groups.insert(id, g);
        m_curParent.push(g);
    } else {
        qWarning("%s: group '%s' already exists", Q_FUNC_INFO, qUtf8Printable(id));
    }
}

void Skin::readText(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();

    Text *t = new Text(attributes, m_curParent.top());
    mCDebug(MODERNUI_XML_PARSER) << "\t" << t << " parent: " << m_curParent.top();
}

void Skin::readVis(QXmlStreamReader &xml)
{

}

void Skin::readColor(QXmlStreamReader &xml)
{

}

void Skin::readStandardFrame(QXmlStreamReader &xml)
{

}

void Skin::readScript(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    //QString id = attributes.value("id").toString();

    Script *s = new Script(attributes, m_curParent.top());
    mCDebug(MODERNUI_XML_PARSER) << "\t" << s << " parent: " << m_curParent.top();

    m_scripts.append(s);
}

void Skin::readInclude(QXmlStreamReader &xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QString include_file = attributes.value("file").toString();

    QString path(m_includeDir.absolutePath() + '/' + include_file);
    mCDebug(MODERNUI_XML_PARSER) << "\t" << "include " << path;

    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning("%s: %s", qUtf8Printable(path), qUtf8Printable(file.errorString()));
        return;
    }

    QXmlStreamReader include_xml;
    //include_xml.setNamespaceProcessing(false);
    include_xml.addExtraNamespaceDeclarations(m_wxml_decs);
    /* Include files may not contain root elements */
    include_xml.addData("<includeroot>");
    QTextStream include_stream(&file);
    while (!include_stream.atEnd()) {
        QString line = include_stream.readLine();
        /*
         * QXmlStreamReader cannot handle namespaces like 'Wasabi:StandardFrame:Status'
         * (or at least I don't know how to use it), and as the only tags with two colons
         * are 'Wasabi:StandardFrame:*', replace the second colon with an underscore
         */
        if (line.contains("<Wasabi:StandardFrame:"))
            line.replace("<Wasabi:StandardFrame:", "<Wasabi:StandardFrame_");
        include_xml.addData(line);
    }
    include_xml.addData("</includeroot>");

    file.close();

    QDir return_dir = m_includeDir;
    if (include_file.contains('/'))
        m_includeDir.cd(include_file.section('/', 0, -2));
    readXmlObjects(include_xml);
    m_includeDir = return_dir;

    if (include_xml.hasError())
        qWarning("%s:%lli:%lli: error: %s", qUtf8Printable(path), xml.lineNumber(),
               xml.characterOffset(), qUtf8Printable(include_xml.errorString()));
}

const QString Skin::findFile(const QString &name)
{
    QDir file_dir(m_skinDir);
    QString file_path(name);

    /*
     * TODO:
     * Can a directory name be mixed case? Can the directories be nested?
     */
    if (file_path.contains('/')) {
        if (!file_dir.cd(file_path.section('/', 0, 0)))
            return QString();
        file_path = file_path.section('/', 1, 1);
    }

    file_dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    file_dir.setNameFilters(QStringList() << file_path);
    QFileInfoList f = file_dir.entryInfoList();
    if (!f.isEmpty())
        return f.first().filePath();

    return QString();
}

QXmlStreamNamespaceDeclarations Skin::m_wxml_decs {
    {"Wasabi", "WasabiXML"},
    {"ColorThemes", "WasabiXML"}};

const QMap<QString, void (Skin::*)(QXmlStreamReader &xml)> Skin::m_readFuncs {
        {"wasabixml", &Skin::readWasabiVer},
        {"winampabstractionlayer", &Skin::readWasabiVer},
        {"skininfo", &Skin::readSkinInfo},
        {"container", &Skin::readContainer},
        {"layout", &Skin::readLayout},
        {"layer", &Skin::readLayer},
        {"animatedlayer", &Skin::readAnimatedLayer},
        {"bitmap", &Skin::readBitmap},
        {"bitmapfont", &Skin::readBitmapFont},
        {"group", &Skin::readGroup},
        {"button", &Skin::readButton},
        {"togglebutton", &Skin::readToggleButton},
        {"slider", &Skin::readSlider},
        {"groupdef", &Skin::readGroupDef},
        {"text", &Skin::readText},
        {"vis", &Skin::readVis},
        {"truetypefont", &Skin::readTrueTypeFont},
        {"color", &Skin::readColor},
        {"standardframe_status", &Skin::readStandardFrame},
        {"standardframe_nostatus", &Skin::readStandardFrame},
        {"standardframe_modal", &Skin::readStandardFrame},
        {"standardframe_static", &Skin::readStandardFrame},
        {"script", &Skin::readScript},
        {"include", &Skin::readInclude}};

inline bool Skin::isSkinInfoTag(const QString &name) const
{
    return std::binary_search(m_validSkininfo.begin(), m_validSkininfo.end(), name);
}

const QStringList Skin::m_validSkininfo {
    {"author"},
    {"comment"},
    {"email"},
    {"homepage"},
    {"name"},
    {"screenshot"},
    {"version"}};
