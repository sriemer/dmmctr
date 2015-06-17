/*
 * Copyright (c) 2011..2015 Sebastian Parschauer <s.parschauer@gmx.de>
 *
 * This file may be used subject to the terms and conditions of the
 * GNU General Public License Version 3, or any later version
 * at your option, as published by the Free Software Foundation.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef LEDINDICATOR_H
#define LEDINDICATOR_H

#include <QtGlobal>
#ifndef QT_VERSION
    #error QT_VERSION not defined!
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets>
#endif
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

#define DEFAULT_SIZE 24  // px

enum LEDColorType
{
    LED_GREEN,
    LED_RED,
    LED_UNDEF
};

class LEDIndicator : public QWidget
{
    Q_OBJECT

public:
    LEDIndicator(Qt::Alignment alignment = Qt::AlignHCenter);
    LEDIndicator(LEDColorType col,
                 Qt::Alignment alignment = Qt::AlignHCenter);
    LEDIndicator(LEDColorType col, QString label, QString styleSheet = "",
                 Qt::Alignment alignment = Qt::AlignHCenter);

    LEDColorType color();
    bool powered();
    void setColor(LEDColorType col);

public slots:
    void setOn();
    void setOff();
    void setToggle();

private:
    QVBoxLayout  *layout;
    QLabel       *label;
    QLabel       *picture;
    QPixmap      *pixmapOn;
    QPixmap      *pixmapOff;

    LEDColorType m_color;
    bool         m_powered;    // LED on/off

    void initWidget(Qt::Alignment alignment);
    void changeColor(LEDColorType col);
};


#endif
