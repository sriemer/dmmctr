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
#include "serialportctr.h"
#include "settings.h"

#define RD_BUFF_SIZE    4096
#define RD_POLL_RATE       5  // ms
#define RD_INI_TIMEOUT  5000  // ms

#define DMM_SYS_ERR   ":SYST:ERR?\n"
#define DMM_IDN       "*IDN?\n"      // Identification
#define DMM_OPC       "*OPC?\n"      // Operation Complete Query

#define DMM_SYS_EXP   "+0,\"No error\""
#define DMM_IDN_EXP   "TEKTRONIX,DMM40"
#define DMM_OPC_EXP   "1"


DMMControl::DMMControl(SerialPortCtr *portControl, Settings *settings)
{
    qDebug() << "DMMControl created";
    serPort = NULL;
    portCtr = portControl;
    sets = settings;
    stopRequested = false;
    ready = false;

    sampleCount = 0;
}

DMMControl::~DMMControl()
{
    stopDMMCtr();
}

void DMMControl::run()
{
    int ret = 0;

    while (true) {
        ready = false;
        stopRequested = false;
        while (!ready)
            sleep(1);

        serPort = portCtr->openPort(sets);
        if (!serPort) {
            emit sendSetError();
            goto stop;
        }
        emit sendStarted();
        ret = initDMM();
        if (ret || stopRequested)
            goto stop_dmm;
        ret = retrieveDMMVal();

stop_dmm:
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

    message = "DISP ON\n";
    send();

    portCtr->closePort(serPort);
    delete serPort;
    serPort = NULL;
}

int DMMControl::initDMM(void)
{
    int ret = 0;
    QString expected;

    if (stopRequested)
        goto out;

    // clear DMM buff and set DMM into remote mode
    message  = "*CLS\nSYST:REM\n" DMM_OPC;
    expected = DMM_OPC_EXP;
    ret = sendAndReadBack(&expected, RD_INI_TIMEOUT);
    if (ret || stopRequested)
        goto out;

    // get DMM identification
    message  = DMM_IDN;
    expected = DMM_IDN_EXP;
    ret = sendAndReadBack(&expected);
    if (ret || stopRequested)
        goto out;

    // reset DMM and set status registers
    message = "*RST;\n*ESE 60;*SRE 56;*CLS;"
              ":STAT:QUES:ENAB 32767\n" DMM_SYS_ERR;
    expected = DMM_SYS_EXP;
    ret = sendAndReadBack(&expected, RD_INI_TIMEOUT);
    if (ret || stopRequested)
        goto out;

    // deactivate beeper, configure display
    message = "SYSTEM:ERROR:BEEPER 0;\n";
    message.append("DISP ");
    message.append(sets->getStringFromID(DISP_ID));
    message.append(";\n" DMM_SYS_ERR);
    expected = DMM_SYS_EXP;
    ret = sendAndReadBack(&expected);
    if (ret || stopRequested)
        goto out;

    // set auto zero
    message = ":ZERO:AUTO ";
    message.append(sets->getStringFromID(AUTOZ_ID));
    message.append(";\n" DMM_SYS_ERR);
    expected = DMM_SYS_EXP;
    ret = sendAndReadBack(&expected);
    if (ret || stopRequested)
        goto out;

    // set DMM function
    message = sets->getStringFromID(FUNCT_ID);
    message.append(";\n" DMM_SYS_ERR);
    expected = DMM_SYS_EXP;
    ret = sendAndReadBack(&expected);
    if (ret || stopRequested)
        goto out;

    // set DC volts intgration time
    if (sets->getCfgID(FUNCT_ID) == 0) {  //TODO compare with enum
       message = "VOLT:DC:NPLC ";
       message.append(sets->getStringFromID(INTEGR_ID));
       message.append(";\n" DMM_SYS_ERR);
       expected = DMM_SYS_EXP;
       ret = sendAndReadBack(&expected);
       if (ret || stopRequested)
           goto out;
    }

    // set trigger config
    message = ":TRIG:DEL ";
    message.append(sets->getCfgIntAsStr(TRIG_DEL_ID));
    message.append(";:TRIG:DEL:AUTO ");
    message.append(sets->getStringFromID(TRIG_ADE_ID));
    message.append(";:TRIG:SOUR ");
    message.append(sets->getStringFromID(TRIG_SRC_ID));
    message.append(";\n" DMM_SYS_ERR);
    expected = DMM_SYS_EXP;
    ret = sendAndReadBack(&expected);
out:
    return ret;
}

int DMMControl::retrieveDMMVal(void)
{
    int ret = 0;
    QString expected;
    QRegExp expRegExp;
    QTime   startTime;
    QString duration;
    QStringList values;

    startTime = QTime::currentTime();
    time = QDateTime::currentMSecsSinceEpoch();  // Qt 4.7 needed

    do {
        // set trigger and sample count
        message = ":TRIG:COUN ";
        if (sets->getCfgInt(TRIG_CNT_ID) < 1)
            message.append("INF");
        else
            message.append(sets->getCfgIntAsStr(TRIG_CNT_ID));
        message.append(";:SAMP:COUN ");
        message.append(sets->getCfgIntAsStr(SAMP_ID));
        message.append(";\n" DMM_SYS_ERR);
        expected = DMM_SYS_EXP;
        ret = sendAndReadBack(&expected);
        if (ret)
            break;

        // fetch current value
        message = "INIT;\nFETC?\n";
        expRegExp.setPattern("^[+-]");
        ret = sendAndReadBack(&expRegExp,
            RD_DEF_TIMEOUT * sets->getCfgInt(SAMP_ID));
        if (ret)
            break;

        values.append(message);
        sampleCount += sets->getCfgInt(SAMP_ID);

        message = ":STAT:QUES:EVEN?\n";
        expRegExp.setPattern("^[+-]");
        ret = sendAndReadBack(&expRegExp);
        if (ret) {
            values.removeLast();
            break;
        }

        // check DMM status
        message  = DMM_SYS_ERR;
        expected = DMM_SYS_EXP;
        ret = sendAndReadBack(&expected);
    } while (!ret && !stopRequested);

    time = QDateTime::currentMSecsSinceEpoch() - time;
    qDebug() << "Time diff in ms: " << time;
    duration.setNum(time);
    values.insert(0, duration);
    values.insert(1, startTime.toString("hh:mm:ss"));
    qDebug() << "Values: " << values;
    emit sendResults(values);

    return ret;
}

void DMMControl::send(void)
{
    serPort->write(message.toAscii(), message.length());
    qDebug() << message;
}

template <typename T>
int DMMControl::sendAndReadBack(T *expected, int timeout)
{
    int ret;

    send();

    message.clear();
    ret = readPort(expected, timeout);
    if (ret == ERR_TIMEOUT)
        emit sendSetTimeout();
    else if (ret != ERR_NONE)
        emit sendSetError();

    qDebug() << message;

    return ret;
}

// QString and QRegExp are used
template <typename T>
int DMMControl::readPort(T *expected, int timeout)
{
    char buff[RD_BUFF_SIZE + 1];
    int  numBytes;
    int  readBytes;
    int  i = 0;
    int  error = ERR_TIMEOUT;

    do {
        msleep(RD_POLL_RATE);
        i++;

        while ((numBytes = serPort->bytesAvailable()) > 0) {
            // prevent buffer overflow, need last char for '\0'
            if (numBytes > (int)(sizeof(buff) - 1))
                numBytes = (int)(sizeof(buff) - 1);

            readBytes = serPort->read(buff, numBytes);
            if (readBytes >= 0)
                buff[readBytes] = '\0';
            else
                buff[0] = '\0';

            message.append(buff);
        }

        if (!message.isEmpty() && message.endsWith("\r\n")) {
            if (message.contains(*expected)) {
                error = ERR_NONE;
                break;
            } else {
                error = ERR_UNWANTED;
                break;
            }
        }

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
