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

#include <QtCore>

#include "serialportctr.h"
#include "settings.h"

SerialPortCtr::SerialPortCtr()
{
    enabled = true;
    oldPorts.clear();
    portList.clear();
}

void SerialPortCtr::run()
{
    bool oldEnabled = enabled;

    _detectPorts();
    emit sendInitPorts(portNames);
    oldPorts = portNames;

    while (true) {
        if (!enabled && oldEnabled)
            emit sendPortsReady();
        oldEnabled = enabled;
        if (enabled) {
            sleep(1);
            detectPorts();
            emit sendPortsDetected();
        } else {
            sleep(1);
        }
    }
}

void SerialPortCtr::detectPorts()
{
    _detectPorts();
    if (portNames != oldPorts && enabled) {
        emit sendReinitPorts(portNames);
        oldPorts = portNames;
    }
}

void SerialPortCtr::_detectPorts()
{
    int removed = 0;
    int size;

    portList = QextSerialEnumerator::getPorts();
    size = portList.size();
    portNames.clear();

    for (int i = 0; i < size; i++) {
#ifdef _TTY_POSIX_
        removed += filterPortsPosix(i - removed);
#else
        removed += filterPortsWin(i - removed);
#endif
    }
    debugPorts();
}

/**
 * - removes POSIX serial ports without a driver registered on
 * - must be called within a loop, uses ttyUSB ports unchecked
 *
 * - writing to a port is not an option here
 */
int SerialPortCtr::filterPortsPosix(int i)
{
    QString checkDir = "/sys/devices/platform/serial8250/tty/";
    QString checkPath;
    int     removed = 0;

    if (portList.at(i).portName.startsWith("ttyUSB") == false) {

        checkPath = checkDir;
        checkPath.append(portList.at(i).portName);

        if (!QDir(checkPath).exists()) {
            portNames.append(portList.at(i).portName);
        } else {
            portList.removeAt(i);
            removed = 1;
        }

    } else {
        portNames.append(portList.at(i).portName);
    }

    return removed;
}

/**
 * - filters parallel ports and should filter currently opened ports
 *   but opened USB serial ports are still shown
 *
 * - opening a port is not an option here
 */
int SerialPortCtr::filterPortsWin(int i)
{
    int removed = 0;

    if (portList.at(i).portName.startsWith("LPT")) {
        portList.removeAt(i);
        removed = 1;
    } else {
        portNames.append(portList.at(i).portName);
    }

    return removed;
}

QextSerialPort *SerialPortCtr::openPort(Config *cfg)
{
#ifdef _TTY_WIN_
    QString portStr = portList.at(cfg->getID(PORT_ID)).portName;
#else
    QString portStr = portList.at(cfg->getID(PORT_ID)).physName;
#endif
    QextSerialPort *port = new QextSerialPort(portStr, QextSerialPort::Polling);

    configurePort(port, cfg);
    if (!port->open(QIODevice::ReadWrite | QIODevice::Unbuffered))
        return NULL;

    return port;
}

void SerialPortCtr::configurePort(QextSerialPort *port, Config *cfg)
{
    Settings sets;

    port->setBaudRate(sets.getBaudID(cfg->getID(BAUD_ID)));
    port->setFlowControl(sets.getFlowID(cfg->getID(FLOW_ID)));
    port->setParity  (sets.getParityID(cfg->getID(PARITY_ID)));
    port->setDataBits(sets.getDataBitsID(cfg->getID(DATA_BITS_ID)));
    port->setStopBits(sets.getStopBitsID(cfg->getID(STOP_BITS_ID)));
    port->setTimeout(500);
}

void SerialPortCtr::closePort(QextSerialPort *port)
{
    port->close();
}

// taken from enumerator example
void SerialPortCtr::debugPorts()
{
    static bool alreadyDone = false;

    if (!alreadyDone) {
        qDebug() << "List of ports:";
        for (int i = 0; i < portList.size(); i++) {
            qDebug() << "port name:" << portList.at(i).portName;
            qDebug() << "friendly name:" << portList.at(i).friendName;
            qDebug() << "physical name:" << portList.at(i).physName;
            qDebug() << "enumerator name:" << portList.at(i).enumName;
            qDebug() << "vendor ID:"
                     << QString::number(portList.at(i).vendorID, 16);
            qDebug() << "product ID:"
                     << QString::number(portList.at(i).productID, 16);
            qDebug() << "===================================";
        }
        alreadyDone = true;
    }
}


// Slots

void SerialPortCtr::enable()
{
    qDebug() << "SerialPortCtr::enable() received";
    enabled = true;
}

void SerialPortCtr::disable()
{
    qDebug() << "SerialPortCtr::disable() received";
    enabled = false;
}
