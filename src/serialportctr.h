/*
 * Copyright (c) 2011..14, by:  Sebastian Riemer
 *    All rights reserved.     <sebastian.riemer@gmx.de>
 *
 * This file may be used subject to the terms and conditions of the
 * GNU General Public License Version 3, or any later version
 * at your option, as published by the Free Software Foundation.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SERIALPORTCTR_H
#define SERIALPORTCTR_H

#include <QtGlobal>
#ifndef QT_VERSION
    #error QT_VERSION not defined!
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtCore>
#endif
#include <qextserialenumerator.h>
#include <qextserialport.h>
#include <QThread>
#include <QStringList>
#include "settings.h"

class SerialPortCtr : public QThread
{
    Q_OBJECT

public:
    SerialPortCtr();
    void run();
    void detectPorts (void);
    QextSerialPort *openPort   (Settings *sets);
    void           closePort   (QextSerialPort *port);

signals:
    void sendInitPorts  (QStringList ports);
    void sendReinitPorts(QStringList ports);
    void sendPortsReady();
    void sendPortsDetected();
public slots:
    void enable();
    void disable();
private:
    QList<QextPortInfo> portList;
    QStringList portNames;
    QStringList oldPorts;
    bool enabled;

    void _detectPorts (void);
    int  filterPortsPosix (int i);
    int  filterPortsWin   (int i);
    void configurePort    (QextSerialPort *port, Settings *sets);
    void debugPorts  (void);
};

#endif

