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

#define RD_BUFF_SIZE    1024
#define RD_POLL_RATE       5  // ms
#define RD_INI_TIMEOUT  5000  // ms
#define RD_DEF_TIMEOUT  2000  // ms

#define DMM_SYS_ERR   ":SYST:ERR?\n"
#define DMM_IDN       "*IDN?\n"      // Identification
#define DMM_OPC       "*OPC?\n"      // Operation Complete Query

#define DMM_SYS_EXP   "+0,\"No error\""
#define DMM_IDN_EXP   "TEKTRONIX,DMM40"
#define DMM_OPC_EXP   "1"


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

    sampleCount = 0;
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
            error = initDMM();
            retrieveDMMVal(error);
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

int DMMControl::initDMM(void)
{
    int error = 0;
    QString expected;

    // clear DMM buff and set DMM into remote mode
    if (!error && !stopRequested) {
        message  = "*CLS\nSYST:REM\n" DMM_OPC;
        expected = DMM_OPC_EXP;
        timeout  = RD_INI_TIMEOUT;
        error = sendAndReadBack(&expected);
        timeout  = RD_DEF_TIMEOUT;
    }

    // get DMM identification
    if (!error && !stopRequested) {
        message  = DMM_IDN;
        expected = DMM_IDN_EXP;
        error = sendAndReadBack(&expected);
    }

    // reset DMM and set status registers
    if (!error && !stopRequested) {
        message = "*RST;\n*ESE 60;*SRE 56;*CLS;"
                  ":STAT:QUES:ENAB 32767\n" DMM_SYS_ERR;
        expected = DMM_SYS_EXP;
        timeout  = RD_INI_TIMEOUT;
        error = sendAndReadBack(&expected);
        timeout  = RD_DEF_TIMEOUT;
    }

    // check status again (ask Tektronix why)
    if (!error && !stopRequested) {
        message  = DMM_SYS_ERR;
        expected = DMM_SYS_EXP;
        error = sendAndReadBack(&expected);
    }

    // deactivate beeper
    if (!error && !stopRequested) {
        message = "SYSTEM:ERROR:BEEPER 0\n" DMM_IDN;
        expected = DMM_IDN_EXP;
        error = sendAndReadBack(&expected);
    }

    // set auto zero
    if (!error && !stopRequested) {
        message = ":ZERO:AUTO ";
        message.append(sets->getMeasAutoZero(cfg->getID(AUTOZ_ID)));
        message.append(";\n" DMM_SYS_ERR);
        expected = DMM_SYS_EXP;
        error = sendAndReadBack(&expected);
    }

    // set DMM function
    if (!error && !stopRequested) {
        message = sets->getMeasFunctCmd(cfg->getID(FUNCT_ID));
        message.append(";\n" DMM_SYS_ERR);
        expected = DMM_SYS_EXP;
        error = sendAndReadBack(&expected);
    }

    // set DC volts intgration time
    if (cfg->getID(FUNCT_ID) == 0) {  //TODO compare with enum
       if (!error && !stopRequested) {
           message = "VOLT:DC:NPLC ";
           message.append(sets->getMeasIntegrTime(
                          cfg->getID(INTEGR_ID)));
           message.append(";\n" DMM_SYS_ERR);
           expected = DMM_SYS_EXP;
           error = sendAndReadBack(&expected);
       }
    }

    // set trigger config
    if (!error && !stopRequested) {
        message = ":TRIG:DEL:AUTO ON;:TRIG:SOUR ";
        message.append(sets->getTrigSource(cfg->getID(TRIG_SRC_ID)));
        message.append(";\n" DMM_SYS_ERR);
        expected = DMM_SYS_EXP;
        error = sendAndReadBack(&expected);
    }

    return error;
}

int DMMControl::retrieveDMMVal(int error)
{
    QString expected;
    QRegExp expRegExp;
    QTime   startTime;
    QString samples;
    QString duration;
    QStringList values;

    samples.setNum(cfg->getIntVal(SAMP_ID));
    startTime = QTime::currentTime();
    time = QDateTime::currentMSecsSinceEpoch();  // Qt 4.7 needed

    while (!error && !stopRequested) {
        // set trigger and sample count
        message = ":TRIG:COUN ";
        message.append(samples);
        message.append(";:SAMP:COUN ");
        message.append(samples);
        message.append(";\n" DMM_SYS_ERR);
        expected = DMM_SYS_EXP;
        error = sendAndReadBack(&expected);

        if (!error) {
            // fetch current value
            message = "INIT;\nFETC?\n";
            expRegExp.setPattern("^[+-]");
            timeout  = RD_DEF_TIMEOUT;
            error = sendAndReadBack(&expRegExp);
            values.append(message);
        }

        if (!error) {
            sampleCount += cfg->getIntVal(SAMP_ID);

            message = ":STAT:QUES:EVEN?\n";
            expRegExp.setPattern("^[+-]");
            error = sendAndReadBack(&expRegExp);
        } else {
            values.removeLast();
        }

        if (!error) {
            // check DMM status
            message  = DMM_SYS_ERR;
            expected = DMM_SYS_EXP;
            error = sendAndReadBack(&expected);
        } else {
            values.removeLast();
        }
    }
    time = QDateTime::currentMSecsSinceEpoch() - time;
    qDebug() << "Time diff in ms: " << time;
    duration.setNum(time);
    values.insert(0, duration);
    values.insert(1, startTime.toString("hh:mm:ss"));
    qDebug() << "Values: " << values;
    emit sendResults(values);

    return error;
}

template <typename T>
int DMMControl::sendAndReadBack(T *expected)
{
    int error;

    serPort->write(message.toAscii(), message.length());
    qDebug() << message;

    message.clear();
    error = readPort(expected);
    if (error == ERR_TIMEOUT) {
        emit sendSetTimeout();
    } else if (error != ERR_NONE) {
        emit sendSetError();
    }
    qDebug() << message;

    return error;
}

// QString and QRegExp are used
template <typename T>
int DMMControl::readPort(T *expected)
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

        // prevent buffer overflow, need last char for '\0'
        if (numBytes > RD_BUFF_SIZE - 1)
            numBytes = RD_BUFF_SIZE - 1;

        readBytes = serPort->read(buff, numBytes);
        if (readBytes >= 0)
            buff[readBytes] = '\0';
        else
            buff[0] = '\0';

        message.append(buff);

        if (!message.isEmpty()) {
            if (message.contains(*expected)) {
                if (message.endsWith("\r\n")) {
                    error = ERR_NONE;
                    break;
                } else {
                   error = ERR_TERM;
                }
            } else {
                error = ERR_UNWANTED;
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
