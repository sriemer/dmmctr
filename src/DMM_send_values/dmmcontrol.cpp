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

#include "dmmcontrol.h"
#include "../serialportctr.h"
#include "../settings.h"

#define RD_BUFF_SIZE   1024
#define RD_POLL_RATE     50  // ms
#define RD_DEF_TIMEOUT 1000  // ms

#define DMM_OPC       static_cast<QRegExp>(".*\\*OPC\\?*")
#define DMM_IDN       static_cast<QRegExp>(".*\\*IDN\\?*")
#define DMM_SYS_ERR   static_cast<QRegExp>(".*:SYST:ERR\\?*")
#define DMM_FETC      static_cast<QRegExp>(".*FETC\\?*")
#define DMM_STAT      static_cast<QRegExp>(".*:STAT:QUES:EVEN\\?*")

#define DMM_OPC_STR   "1"
#define DMM_IDN_STR   "TEKTRONIX,DMM4050,1555202,08/02/10-11:53"
#define DMM_SYS_STR   "+0,\"No error\""
#define DMM_FETC_STR  "-0.000200"
#define DMM_FETC_DBL  -0.000200
#define DMM_STAT_STR  "+8192"


DMMControl::DMMControl(SerialPortCtr *portControl, Settings *settings, Config *config)
{
    qDebug() << "DMMControl created";
    serPort = NULL;
    portCtr = portControl;
    sets = settings;
    cfg = config;
    stopRequested = false;
    ready = false;
    timeout = RD_DEF_TIMEOUT;
}

DMMControl::~DMMControl()
{
    stopDMMCtr();
}

void DMMControl::setConfig(Config *config)
{
    cfg = config;
}

void DMMControl::run()
{
    int error = 0;

    while (true) {
        ready = false;
        stopRequested = false;
        while (!ready)
            sleep(1);
        serPort = portCtr->openPort(sets, cfg);
        emit sendStarted();
        if (serPort != NULL) {
            error = emulateDMM(error);
        } else {
            emit sendSetError();
        }

        stopDMMCtr();

        emit sendStopped();
        emit sendEnable();
    }
}

bool DMMControl::isReady()
{
    return ready;
}

void DMMControl::stopDMMCtr()
{
    if (serPort != NULL) {
        portCtr->closePort(serPort);
        delete serPort;
        serPort = NULL;
    }
}

int DMMControl::emulateDMM(int error)
{
    QList<QRegExp> knownCommads(QList<QRegExp>() << DMM_OPC << DMM_IDN
                                << DMM_SYS_ERR << DMM_FETC << DMM_STAT);
    QStringList answers(QStringList() << DMM_OPC_STR << DMM_IDN_STR
                        << DMM_SYS_STR << DMM_FETC_STR << DMM_STAT_STR);
    double voltage;
    QString voltStr;

    qsrand(1234567);
    while (!error && !stopRequested) {
        voltage = ((double) qrand() / (double) RAND_MAX - 0.5) / 10000;
        voltage += DMM_FETC_DBL;
        voltStr.setNum(voltage, 'f', 6);
        answers.replace(3, voltStr);
        error = readAndSendBack(&knownCommads, &answers, "\r\n");
    }

    return error;
}

int DMMControl::readAndSendBack(QList<QRegExp> *expected, QStringList *answers,
                                QString term)
{
    int error;

    message.clear();
    error = readPort();
    emit sendSetCommand(message);
    if (error == ERR_TIMEOUT) {
        emit sendSetTimeout();
        error = ERR_NONE;
    } else {
        qDebug() << message;
        emit sendClearTimeout();
        if (!message.endsWith("\n")) {
            error = ERR_TERM;
            emit sendSetError();
        }
        error = ERR_UNWANTED;
        for (int i = 0; i < expected->length(); i++) {
            if (message.contains(expected->at(i))) {
                message = answers->at(i);
                message.append(term);
                serPort->write(message.toAscii(), message.length());
                qDebug() << message;
                error = ERR_NONE;
                break;
            }
        }
        if (error == ERR_UNWANTED) {
            emit sendSetError();
            //error = ERR_NONE;   // For testing only!
        }
    }
    emit sendSetAnswer(message);

    return error;
}

int DMMControl::readPort()
{
    char buff[RD_BUFF_SIZE];
    int  numBytes;
    int  readBytes;
    int  i = 0;
    int  error = ERR_TIMEOUT;

    do {
        msleep(RD_POLL_RATE);
        i++;
        numBytes = serPort->bytesAvailable();

        if (!message.isEmpty() && numBytes <= 0) {
            error = ERR_NONE;
            break;
        }

        if (numBytes > RD_BUFF_SIZE)
            numBytes = RD_BUFF_SIZE;

        readBytes = serPort->read(buff, numBytes);
        if (readBytes != -1)
            buff[readBytes] = '\0';
        else
            buff[0] = '\0';

        message.append(buff);
    } while (i <= (timeout / RD_POLL_RATE));

    return error;
}


// Slots

void DMMControl::setReady()
{
    qDebug() << "DMMControl::setReady() received";
    ready = true;
}

void DMMControl::requestStop()
{
    qDebug() << "DMMControl::requestStop() received";
    if (ready)
        stopRequested = true;
}
