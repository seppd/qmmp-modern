/***************************************************************************
 *   This file is based on qmmp source file                                *
 *   src/plugins/Ui/skinned/skinreader.h                                   *
 *   which is                                                              *
 *   Copyright (C) 2007-2016 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
 ***************************************************************************/

#ifndef SKINREADER_H
#define SKINREADER_H

#include <QObject>
#include <QMap>
#include <QPixmap>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class QProcess;

class SkinReader : public QObject
{
    Q_OBJECT
public:
    SkinReader(QObject *parent = 0);

    ~SkinReader() {}

    void generateThumbs();
    void unpackSkin(const QString &path);
    const QStringList skins();
    const QPixmap getPreview(const QString &skinPath);

private:
    QProcess *m_process;
    void untar(const QString &from, const QString &to, bool preview);
    void unzip(const QString &from, const QString &to,  bool preview);
    QMap <QString, QString> m_previewMap;
};

#endif
