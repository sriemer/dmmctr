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

#include "ledindicator.h"

LEDIndicator::LEDIndicator(Qt::Alignment alignment)
{
    initWidget(alignment);
    layout->removeWidget(label);

    changeColor(LED_GREEN);
}

LEDIndicator::LEDIndicator(LEDColorType col,
                           Qt::Alignment alignment)
{
    initWidget(alignment);
    layout->removeWidget(label);

    changeColor(col);
}

LEDIndicator::LEDIndicator(LEDColorType col, QString labelText,
                           QString styleSheet, Qt::Alignment alignment)
{
    initWidget(alignment);
    changeColor(col);

    label->setText(labelText);
    label->setStyleSheet(styleSheet);
}

LEDColorType LEDIndicator::color()
{
    return m_color;
}

bool LEDIndicator::powered()
{
    return m_powered;
}

void LEDIndicator::setColor(LEDColorType col)
{
    changeColor(col);
}

void LEDIndicator::setOn()
{
    m_powered = true;
    picture->setPixmap(*pixmapOn);
}

void LEDIndicator::setOff()
{
    m_powered = false;
    picture->setPixmap(*pixmapOff);
}

void LEDIndicator::setToggle()
{
    m_powered = !m_powered;

    if (m_powered)
        picture->setPixmap(*pixmapOn);
    else
        picture->setPixmap(*pixmapOff);
}

void LEDIndicator::initWidget(Qt::Alignment alignment)
{
    layout    = new QVBoxLayout;
    label     = new QLabel;
    picture   = new QLabel;

    layout->addWidget(label, 0, alignment);
    layout->addWidget(picture, 0, alignment);
    this->setLayout(layout);

    m_color   = LED_UNDEF;
    m_powered = false;
}

void LEDIndicator::changeColor(LEDColorType col)
{
    if (col == m_color)
        return;

    switch (col) {
    case LED_RED:
        pixmapOn  = new QPixmap(":/led/images/red-led-on.svg", "SVG");
        pixmapOff = new QPixmap(":/led/images/red-led-off.svg", "SVG");
        m_color = col;
        break;
    case LED_GREEN:
    default:
        pixmapOn  = new QPixmap(":/led/images/green-led-on.svg", "SVG");
        pixmapOff = new QPixmap(":/led/images/green-led-off.svg", "SVG");
        m_color = LED_GREEN;
        break;
    }

    *pixmapOn  = pixmapOn->scaled(DEFAULT_SIZE, DEFAULT_SIZE);
    *pixmapOff = pixmapOff->scaled(DEFAULT_SIZE, DEFAULT_SIZE);

    if (m_powered)
        picture->setPixmap(*pixmapOn);
    else
        picture->setPixmap(*pixmapOff);
}
