/*
 *   This file is based on qmmp source file
 *   src/plugins/Ui/skinned/skin.cpp
 *   which is
 *   Copyright (C) 2007-2016 by Ilya Kotov
 *   forkotov02@hotmail.ru
 */

#include <QVariant>

#include "bitmapfont.h"
#include "bitmap.h"
#include "skin.h"

BitmapFont::BitmapFont(const QXmlStreamAttributes &attributes, QObject *parent) : QObject(parent)
{
    QString name, value;
    for (QXmlStreamAttribute i : attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }

    if (m_letters.isEmpty())
        setFile(attributes.value("file").toString());
}

void BitmapFont::setFile(const QString &file)
{
    if (m_charWidth == 0 || m_charHeight == 0)
        return;

    QPixmap fontpm;
    QString path = Skin::instance()->findFile(file);
    if (!path.isEmpty()) {
        fontpm.load(path);
        if (fontpm.isNull()) {
            qWarning("%s: could not load file '%s'", Q_FUNC_INFO, qUtf8Printable(path));
            return;
        }
    } else {
        //Bitmap *b = qobject_cast<Bitmap *>(Skin::instance()->getGuiObject(file));
        Bitmap *b = Skin::instance()->bitmap(file);
        if (b == Q_NULLPTR) {
            qWarning("%s: could not find bitmap '%s'", Q_FUNC_INFO, qUtf8Printable(file));
            return;
        }
        fontpm = b->pixmap();
        if (fontpm.isNull()) {
            qWarning("%s: bitmap '%s' is null", Q_FUNC_INFO, qUtf8Printable(file));
            return;
        }
    }
    m_file = file;

    QList<QList<QPixmap>> letters;
    for (int i = 0; i < 3; i++) {
        QList<QPixmap> l;
        for (int j = 0; j < 31; j++)
            l.append(fontpm.copy(j * m_charWidth - m_hSpacing, i * m_charHeight - m_vSpacing, m_charWidth, m_charHeight));
        letters.append(l);
    }

    /* alphabet */
    for (uint i = 97; i < 123; i++)
        m_letters.insert(i, letters[0][i - 97]);

    /* digits */
    for (uint i = 0; i <= 9; i++)
        m_letters.insert(i + 48, letters[1][i]);

    /* special characters */
    m_letters.insert('"',  letters[0][27]);
    m_letters.insert('@',  letters[0][28]);
    m_letters.insert(':',  letters[1][12]);
    m_letters.insert('(',  letters[1][13]);
    m_letters.insert(')',  letters[1][14]);
    m_letters.insert('-',  letters[1][15]);
    m_letters.insert('\'', letters[1][16]);
    m_letters.insert('`',  letters[1][16]);
    m_letters.insert('!',  letters[1][17]);
    m_letters.insert('_',  letters[1][18]);
    m_letters.insert('+',  letters[1][19]);
    m_letters.insert('\\', letters[1][20]);
    m_letters.insert('/',  letters[1][21]);
    m_letters.insert('[',  letters[1][22]);
    m_letters.insert(']',  letters[1][23]);
    m_letters.insert('^',  letters[1][24]);
    m_letters.insert('&',  letters[1][25]);
    m_letters.insert('%',  letters[1][26]);
    m_letters.insert('.',  letters[1][27]);
    m_letters.insert(',',  letters[1][27]);
    m_letters.insert('=',  letters[1][28]);
    m_letters.insert('$',  letters[1][29]);
    m_letters.insert('#',  letters[1][30]);

    m_letters.insert(229, letters[2][0]);
    m_letters.insert(246, letters[2][1]);
    m_letters.insert(228, letters[2][2]);
    m_letters.insert('?', letters[2][3]);
    m_letters.insert('*', letters[2][4]);
    m_letters.insert(' ', letters[2][5]);
}

const QPixmap BitmapFont::getLetter(const QChar &ch) const
{ return m_letters[ch]; }

int BitmapFont::width() const
{ return m_charWidth; }

int BitmapFont::height() const
{ return m_charHeight; }
