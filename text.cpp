/***************************************************************************
 *   Text scrolling functional is based on qmmp source file                *
 *   src/plugins/Ui/skinned/textscroller.cpp                               *
 *   which is                                                              *
 *   Copyright (C) 2007-2015 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
 ***************************************************************************/

#include <QPainter>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QMouseEvent>

#include <qmmpui/mediaplayer.h>

#include "text.h"
#include "skin.h"
#include "truetypefont.h"
#include "bitmapfont.h"

#include <QDebug>

static const int tickerInterval = 50;
static const int freezeInterval = 4000;

#define SONGNAME_FORMAT "%p%if(%p&%t, - ,)%t"
#define SONGINFO_FORMAT "%p%if(%p&%t, - ,)%t%if(%p,,%if(%t,,%f))"
#define SONGARTIST_FORMAT "%p"
#define SONGTITLE_FORMAT "%t"
#define SONGALBUM_FORMAT "%a"
#define SONGLENGTH_FORMAT "%l"

Text::Text(const QXmlStreamAttributes &attributes, QWidget *parent) : GuiObject(parent)
{
    m_core = SoundCore::instance();
    m_effect = new QGraphicsDropShadowEffect(this);
    m_effect->setOffset(0, 0);
    QString name, value;
    for (QXmlStreamAttribute i : attributes) {
        name = i.name().toString().toLower();
        value = i.value().toString();
        if (name == "display" || name == "align" || name == "valign")
            value = value.toUpper();
        else if (name == "default") // 'default' is the alias for 'text' and also a keyword
            name = "text";
        if (!setProperty(qPrintable(name), QVariant(value))) {
            if (dynamicPropertyNames().contains(qPrintable(name))) {
                qWarning("%s: unknown property '%s'", Q_FUNC_INFO, qUtf8Printable(name));
                setProperty(qPrintable(name), QVariant());
            }
            qWarning("%s: failed to set '%s' to '%s'", Q_FUNC_INFO, qUtf8Printable(name), qUtf8Printable(value));
        }
    }
}

Text::Display Text::display() const
{
    return m_display;
}

void Text::setDisplay(Display display)
{
    if (m_display == display)
        return;

    TextDisplayProvider *provider = TextDisplayProviderFactory::create(display, this);
    if (provider == Q_NULLPTR)
        return;

    disconnect();
    delete m_provider;
    m_provider = provider;
    m_display = display;
    connect(dynamic_cast<QObject *>(provider), SIGNAL(textChanged(QString)), SLOT(displayText(QString)));

    if (display == TIME || display == TIMEREMAINING || display == TIMEELAPSED)
        connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(processState(Qmmp::State)));
}

void Text::setTicker(bool enable)
{
    if (m_timer == Q_NULLPTR) {
        m_timer = new QTimer(this);
        m_timer->setInterval(freezeInterval);
        connect(m_timer, SIGNAL(timeout()), SLOT(addOffset()));
    }
    m_ticker = enable;
    updateText();
}

void Text::setAntiAlias(bool enable)
{
    if (enable)
        m_font.setStyleStrategy(QFont::PreferAntialias);
    else
        m_font.setStyleStrategy(QFont::NoAntialias);

    m_antiAlias = enable;
    updateText();
}

QString Text::text() const
{
    return m_displayText;
}

void Text::setText(const QString &text)
{
    if (m_text == text)
        return;

    if (m_altTextDisplayed && text.isEmpty()) {
        m_displayText = m_text;
        m_altTextDisplayed = false;
    } else {
        m_displayText = m_text = text;
    }

    m_xTicker = 0;
    m_scrollBack = true;
    updateText();
}

void Text::setAlternateText(const QString &text)
{
    if (m_displayText == text)
        return;

    m_xTicker = 0;
    m_scrollBack = true;
    m_displayText = text;
    m_altTextDisplayed = true;
    updateText();
}

QString Text::font() const
{ return m_fontId; }

void Text::setFont(const QString &id)
{
    QObject *fontobj = Skin::instance()->font(id);
    if ((m_ttf = qobject_cast<TrueTypeFont *>(fontobj)) != Q_NULLPTR) {
        int fontid = m_ttf->fontId();
        if (fontid >= 0) {
            m_font.setFamily(QFontDatabase::applicationFontFamilies(fontid).at(0));
            setGraphicsEffect(m_effect);
            if (!m_antiAlias)
                m_font.setStyleStrategy(QFont::NoAntialias);
        } else {
            qWarning("%s: no '%s' font found", Q_FUNC_INFO, qUtf8Printable(id));
            return;
        }
    } else if ((m_bmf = qobject_cast<BitmapFont *>(fontobj)) != Q_NULLPTR) {
        m_bitmap = true;
    } else {
        qWarning("%s: unknown font object '%s'", Q_FUNC_INFO, qUtf8Printable(id));
        return;
    }
    m_fontId = id;
    updateText();
}


Text::Alignment Text::align() const
{
    return m_halign;
}

void Text::setAlign(Text::Alignment align)
{
    if (m_halign == align)
        return;

    Qt::Alignment ha;
    switch (align) {
    case LEFT:
        ha = Qt::AlignLeft;
        break;
    case CENTER:
        ha = Qt::AlignHCenter;
        break;
    case RIGHT:
        ha = Qt::AlignRight;
        break;
    default:
        qWarning("%s: unknown alignment: %i", Q_FUNC_INFO, align);
        return;
    }

    m_align &= ~Qt::AlignHorizontal_Mask;
    m_align |= ha;
    m_halign = align;
    updateText();
}

Text::Alignment Text::vAlign() const
{
    return m_valign;
}

void Text::setVAlign(Text::Alignment valign)
{
    if (m_valign == valign)
        return;

    Qt::Alignment va;
    switch (valign) {
    case TOP:
        va = Qt::AlignTop;
        break;
    case CENTER:
        va = Qt::AlignVCenter;
        break;
    case BOTTOM:
        va = Qt::AlignBottom;
        break;
    default:
        qWarning("%s: unknown alignment: %i", Q_FUNC_INFO, valign);
        return;
    }

    m_align &= ~Qt::AlignVertical_Mask;
    m_align |= va;
    m_valign = valign;
    updateText();
}

QString Text::color() const
{
    return QString("%1,%2,%3").arg(m_color.red()).arg(m_color.green()).arg(m_color.blue());
}

void Text::setColor(const QString &rgbString)
{
    if (rgbString == color())
        return;

    QStringList colors = rgbString.split(',');
    m_color.setRgb(colors.at(0).toInt(), colors.at(1).toInt(), colors.at(2).toInt());
    updateText();
}

QString Text::shadowColor() const
{
    QColor color = m_effect->color();
    return QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue());
}

void Text::setShadowColor(const QString &rgbString)
{
    if (shadowColor() == rgbString)
        return;

    QStringList colors = rgbString.split(',');
    m_effect->setColor(QColor(colors.at(0).toInt(), colors.at(1).toInt(), colors.at(2).toInt()));
    update();
}

int Text::fontSize() const
{
    return m_font.pixelSize();
}

void Text::setFontSize(int size)
{
    if (m_font.pixelSize() == size)
        return;

    m_font.setPixelSize(size);
    updateText();
}

int Text::xShadow() const
{
    return m_effect->xOffset();
}

void Text::setXShadow(int x)
{
    if (m_effect->xOffset() == x)
        return;

    m_effect->setXOffset(x);
    update();
}

int Text::yShadow() const
{
    return m_effect->yOffset();
}

void Text::setYShadow(int y)
{
    if (m_effect->yOffset() == y)
        return;

    m_effect->setYOffset(y);
    update();
}

int Text::timerOffStyle() const
{
    return m_offStyle;
}

void Text::setTimerOffStyle(int style)
{
    if (m_offStyle == style)
        return;

    switch (style) {
    case Colon:
        m_text = " : ";
        break;
    case Zeroes:
        m_text = "00:00";
        break;
    case Empty:
        m_text = "";
        break;
    default:
        qWarning("%s: unknown style: %i", Q_FUNC_INFO, style);
        return;
    }
    m_offStyle = style;
}

void Text::setTimeColonWidth(int width)
{
    if (m_timeColonWidth == width)
        return;

    m_timeColonWidth = width;
    updateText();
}

bool Text::noGrab() const
{
    Q_UNIMPLEMENTED();
    return false;
}

void Text::setNoGrab(bool enable)
{
    Q_UNIMPLEMENTED();
}

bool Text::showLen() const
{
    return m_showLen;
}

void Text::setShowLen(bool enable)
{
    if (m_showLen == enable)
        return;

    m_showLen = enable;
}

bool Text::forceFixed() const
{
    return m_font.fixedPitch();
}

void Text::setForceFixed(bool enable)
{
    if (m_font.fixedPitch() == enable)
        return;

    m_font.setFixedPitch(enable);
    updateText();
}

bool Text::forceUpperCase() const
{
    return m_font.capitalization() == QFont::AllUppercase;
}

void Text::setForceUpperCase(bool enable)
{
    if (((m_font.capitalization() == QFont::AllUppercase) && enable) ||
            ((m_font.capitalization() != QFont::AllUppercase) && !enable))
        return;

    if (enable)
        m_font.setCapitalization(QFont::AllUppercase);
    else
        m_font.setCapitalization(QFont::MixedCase);

    updateText();
}

bool Text::forceLowerCase() const
{
    return m_font.capitalization() == QFont::AllLowercase;
}

void Text::setForceLowerCase(bool enable)
{
    if (((m_font.capitalization() == QFont::AllLowercase) && enable) ||
            ((m_font.capitalization() != QFont::AllLowercase) && !enable))
        return;

    if (enable)
        m_font.setCapitalization(QFont::AllLowercase);
    else
        m_font.setCapitalization(QFont::MixedCase);

    updateText();
}

bool Text::bold() const
{
    return m_font.bold();
}

void Text::setBold(bool enable)
{
    if (m_font.bold() == enable)
        return;

    m_font.setBold(enable);
    updateText();
}

bool Text::wrap() const
{
    Q_UNIMPLEMENTED();
    return false;
}

void Text::setWrap(bool enable)
{
    Q_UNIMPLEMENTED();
}

int Text::xOffset() const
{
    Q_UNIMPLEMENTED();
    return 0;
}

void Text::setXOffset(int x)
{
    Q_UNIMPLEMENTED();
}

int Text::yOffset() const
{
    Q_UNIMPLEMENTED();
    return 0;
}

void Text::setYOffset(int y)
{
    Q_UNIMPLEMENTED();
}

QString Text::dblClickAction() const
{
    qWarning("%s: deprecated", Q_FUNC_INFO);
    return QString();
}

void Text::setDblClickAction(const QString &action)
{
    Q_UNUSED(action);
    qWarning("%s: deprecated", Q_FUNC_INFO);
}

void Text::drawBitmapText(const QString &text, QPainter *painter)
{
    QString lowertext = text.toLower();
    int pixwidth = lowertext.size() * m_bmf->width();

    int x = 0;
    switch (m_align & Qt::AlignHorizontal_Mask) {
    case Qt::AlignLeft:
        x = 0;
        break;
    case Qt::AlignHCenter:
        x = (w() - pixwidth) / 2;
        break;
    case Qt::AlignRight:
        x = w() - pixwidth;
        break;
    }

    int y;
    switch (m_align & Qt::AlignVertical_Mask) {
    case Qt::AlignTop:
        y = 0;
        break;
    case Qt::AlignVCenter:
        y = (h() - m_bmf->height()) / 2;
        break;
    case Qt::AlignBottom:
        y = h() - m_bmf->height();
        break;
    }

    for (int i = 0; i < lowertext.size(); i++) {
        const QChar letter = lowertext.at(i);
        QPixmap pixchar = m_bmf->getLetter(letter);
        if (letter == ':' && m_timeColonWidth != -1)
            pixchar = pixchar.copy(0, 0, m_timeColonWidth, pixchar.height());
        painter->drawPixmap(x, y, pixchar);
        x += pixchar.width();
    }
}

void Text::addOffset()
{
    if (m_timer->interval() != tickerInterval)
        m_timer->setInterval(tickerInterval);

    if (m_scrollBack) {
        m_xTicker--;
        if (m_xTicker == m_tickerMin) {
            m_scrollBack = false;
            m_timer->start(freezeInterval);
        }
     } else {
        m_xTicker++;
        if (m_xTicker == 0) {
            m_scrollBack = true;
            m_timer->start(freezeInterval);
        }
    }
    update();
}

void Text::updateText()
{
    int _w = w();
    int _h = h();

    if (_w == 0 || _h == 0)
        return;

    if (m_displayText.isEmpty()) {
        m_pixmap = QPixmap();
        m_scroll = false;
        m_xTicker = 0;
        update();
        return;
    }

    if (m_ticker) {
        if (m_bitmap) {
            _w = m_displayText.size() * m_bmf->width();
        } else {
            QFontMetrics metrics(m_font);
            _w = metrics.width(m_displayText);
        }

        if (_w > w()) {
            m_tickerMin = -(_w - w());
            m_scroll = true;
            m_timer->start();
        } else {
            m_scroll = false;
            m_timer->stop();
        }
    }

    m_pixmap = QPixmap(_w, _h);
    m_pixmap.fill(Qt::transparent);

    QPainter painter(&m_pixmap);
    if (!painter.isActive())
        return;

    if (m_bitmap) {
        drawBitmapText(m_displayText, &painter);
    } else {
        painter.setPen(m_color);
        painter.setFont(m_font);
        painter.drawText(0, 0, _w, _h, Qt::Alignment::Int(m_align), m_displayText);
    }
    update();
}

void Text::displayText(QString text)
{
    if (m_showLen)
        text.append(QString(" (%1)").arg(MetaDataFormatter::formatLength(m_core->totalTime() / 1000)));
    setText(text);
}

void Text::processState(Qmmp::State state)
{
    switch (state) {
    case Qmmp::Stopped:
        switch (m_offStyle) {
        case Colon:
            setText(" : ");
            break;
        case Zeroes:
            setText("00:00");
            break;
        case Empty:
            setText("");
            break;
        }
        break;
    default:
        break;
    }
}

void Text::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(m_xTicker, 0, m_pixmap);
}

void Text::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        //m_menu->exec(e->globalPos());
    } else if (event->button() == Qt::LeftButton) {
        if (m_scroll) {
            m_timer->stop();
            m_pressPos = event->x() - m_xTicker;
            m_pressed = true;
        }
        if (m_display == TIME || m_display == TIMEELAPSED) {
            setDisplay(TIMEREMAINING);
        } else if (m_display == TIMEREMAINING) {
            setDisplay(TIMEELAPSED);
        }
    }

    GuiObject::mousePressEvent(event);
}

void Text::mouseReleaseEvent(QMouseEvent *event)
{
   if (event->button() == Qt::RightButton) {
        //m_menu->exec(e->globalPos());
   } else if (event->button() == Qt::LeftButton && m_scroll)
       m_timer->start(freezeInterval);

   m_pressed = false;
   GuiObject::mouseReleaseEvent(event);
}

void Text::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pressed && m_scroll) {
        m_xTicker = event->x() - m_pressPos;
        if (m_xTicker > 0) {
            m_xTicker = 0;
            m_scrollBack = true;
        }
        else if (m_xTicker < m_tickerMin) {
            m_xTicker = m_tickerMin;
            m_scrollBack = false;
        }
        update();
    }

    GuiObject::mouseMoveEvent(event);
}

MetaDataProvider::MetaDataProvider(Text::Display display, QObject *parent) : QObject(parent)
{
    switch (display) {
    case Text::SONGNAME:
        m_formatter.setPattern(SONGNAME_FORMAT);
        break;
    case Text::SONGINFO:
        m_formatter.setPattern(SONGINFO_FORMAT);
        break;
    case Text::SONGARTIST:
        m_formatter.setPattern(SONGARTIST_FORMAT);
        break;
    case Text::SONGTITLE:
        m_formatter.setPattern(SONGTITLE_FORMAT);
        break;
    case Text::SONGALBUM:
        m_formatter.setPattern(SONGALBUM_FORMAT);
        break;
    case Text::SONGLENGTH:
        m_formatter.setPattern(SONGLENGTH_FORMAT);
        break;
    default:
        qWarning("%s: wrong display: %i", Q_FUNC_INFO, display);
        return;
    }
    m_core = SoundCore::instance();
    connect(m_core, SIGNAL(metaDataChanged()), SLOT(processMetaData()));
}

void MetaDataProvider::processMetaData()
{
    emit textChanged(m_formatter.format(m_core->metaData(), m_core->totalTime() / 1000));
}

TimeProvider::TimeProvider(Text::Display display, QObject *parent) : QObject(parent)
{
    m_core = SoundCore::instance();

    switch (display) {
    case Text::TIMEREMAINING:
        m_remaining = true;
        m_totalSeconds = m_core->totalTime() / 1000;
    case Text::TIME:
    case Text::TIMEELAPSED:
        break;
    default:
        qWarning("%s: wrong display: %i", Q_FUNC_INFO, display);
        return;
    }

    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(processState(Qmmp::State)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(processTime(qint64)));
}

void TimeProvider::processTime(qint64 elapsed)
{
    qint64 sec = elapsed / 1000;
    if (m_seconds == sec)
        return;

    m_seconds = sec;
    QString text;
    if (m_remaining) {
        sec = m_totalSeconds - sec;
        //text.append("-");
    }
    text.append(MetaDataFormatter::formatLength(sec, false));
    emit textChanged(text);
}

void TimeProvider::processState(Qmmp::State state)
{
    switch(state) {
    case Qmmp::Playing:
        m_totalSeconds = m_core->totalTime() / 1000;
        break;
    default:
        break;
    }
}

BitRateProvider::BitRateProvider(QObject *parent) : QObject(parent)
{
    connect(SoundCore::instance(), SIGNAL(bitrateChanged(int)), SLOT(processBitRate(int)));
}

void BitRateProvider::processBitRate(int rate)
{
    emit textChanged(QString::number(rate));
}

SampleRateProvider::SampleRateProvider(QObject *parent) : QObject(parent)
{
    connect(SoundCore::instance(), SIGNAL(frequencyChanged(quint32)), SLOT(processSampleRate(quint32)));
}

void SampleRateProvider::processSampleRate(quint32 rate)
{
    emit textChanged(QString::number(rate / 1000));
}

TextDisplayProvider *TextDisplayProviderFactory::create(Text::Display display, QObject *parent)
{
    switch (display) {
    case Text::SONGNAME:
    case Text::SONGINFO:
    case Text::SONGARTIST:
    case Text::SONGTITLE:
    case Text::SONGALBUM:
    case Text::SONGLENGTH:
        return new MetaDataProvider(display, parent);
        break;
    case Text::TIME:
    case Text::TIMEELAPSED:
    case Text::TIMEREMAINING:
        return new TimeProvider(display, parent);
        break;
    case Text::COMPONENTBUCKET:

        break;
    case Text::SONGBITRATE:
        return new BitRateProvider(parent);
        break;
    case Text::SONGSAMPLERATE:
        return new SampleRateProvider(parent);
        break;
    default:
        qWarning("%s: unknown display: %i", Q_FUNC_INFO, display);
        break;
    }
    return Q_NULLPTR;
}
