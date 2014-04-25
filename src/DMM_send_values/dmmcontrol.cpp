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

#define DMM_FETC_DBL  -0.000200

typedef enum {
    OPC_ID,
    IDN_ID,
    SYS_ERR_ID,
    FETC_ID,
    STAT_ID,
    RQS_SIZE  // always the last entry
} RequestIDType;


DMMControl::DMMControl(SerialPortCtr *portControl, Settings *settings)
{
    serPort = NULL;
    portCtr = portControl;
    sets = settings;
    stopRequested = false;
    ready = false;
    timeout = RD_DEF_TIMEOUT;
    initRequests();
    qDebug() << "DMMControl created";
}

DMMControl::~DMMControl()
{
    stopDMMCtr();
}

void DMMControl::initRequests(void)
{
    int idx;
    rqs = QVector<RqEntry>(RQS_SIZE);

    idx = OPC_ID;
    rqs[idx].request = static_cast<QRegExp>(".*\\*OPC\\?*");
    rqs[idx].answer = "1";

    idx = IDN_ID;
    rqs[idx].request = static_cast<QRegExp>(".*\\*IDN\\?*");
    rqs[idx].answer = "TEKTRONIX,DMM4050,1555202,08/02/10-11:53";

    idx = SYS_ERR_ID;
    rqs[idx].request = static_cast<QRegExp>(".*:SYST:ERR\\?*");
    rqs[idx].answer = "+0,\"No error\"";

    idx = FETC_ID;
    rqs[idx].request = static_cast<QRegExp>(".*FETC\\?*");
    rqs[idx].answer = "-0.000200";

    idx = STAT_ID;
    rqs[idx].request = static_cast<QRegExp>(".*:STAT:QUES:EVEN\\?*");
    rqs[idx].answer = "+8192";
}

void DMMControl::run()
{
    while (true) {
        ready = false;
        stopRequested = false;
        while (!ready)
            sleep(1);
        serPort = portCtr->openPort(sets);
        if (!serPort) {
            sendSetError();
            goto stop;
        }
        emit sendStarted();
        emulateDMM();

        stopDMMCtr();
stop:
        emit sendStopped();
        emit sendEnable();
    }
}

bool DMMControl::isReady()
{
    return ready;
}

void DMMControl::stopDMMCtr(void)
{
    if (!serPort)
        return;

    portCtr->closePort(serPort);
    delete serPort;
    serPort = NULL;
}

int DMMControl::emulateDMM(void)
{
    int ret = 0;

    qsrand(1234567);
    do {
        ret = readAndSendBack("\r\n");
    } while (!ret && !stopRequested);

    return ret;
}

void DMMControl::handleFetch(void)
{
    double voltage;
    QString voltStr;

    voltage = DMM_FETC_DBL;
    voltage += ((double) qrand() / (double) RAND_MAX - 0.5) / 10000;
    voltStr.setNum(voltage, 'f', 6);
    rqs[FETC_ID].answer = voltStr;
}

int DMMControl::readAndSendBack(QString term)
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
        for (int i = 0; i < rqs.size(); i++) {
            if (message.contains(rqs.at(i).request)) {
                if (i == FETC_ID)
                    handleFetch();
                message = rqs.at(i).answer;
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

int DMMControl::readPort(void)
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
