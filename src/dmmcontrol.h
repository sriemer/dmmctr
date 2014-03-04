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

#ifndef DMMCONTROL_H
#define DMMCONTROL_H

#include <QThread>
#include <qextserialport.h>

#include "settings.h"
#include "config.h"
#include "serialportctr.h"

enum DMMErrorType {
    ERR_NONE,
    ERR_TIMEOUT,
    ERR_TERM,
    ERR_UNWANTED
};

class DMMControl : public QThread
{
    Q_OBJECT

public:
    DMMControl(SerialPortCtr *portControl, Settings *settings, Config *config);
    ~DMMControl();
    void setConfig(Config *config);
    void run();
    bool isReady();

signals:
    void sendEnable();
    void sendSetTimeout();
    void sendClearTimeout();
    void sendSetError();
    void sendClearError();
    void sendResults(QStringList);
    void sendStarted();
    void sendStopped();

public slots:
    void setReady();
    void requestStop();

private:
    QextSerialPort *serPort;
    SerialPortCtr  *portCtr;
    Settings *sets;
    Config   *cfg;
    QString message;
    int     timeout;
    bool    stopRequested;
    bool    ready;

    qint64  time;
    int     sampleCount;

    void stopDMMCtr (void);
    int  initDMM    (void);
    int  retrieveDMMVal (int error);
    template <typename T>
    int  sendAndReadBack (T *expected);
    template <typename T>
    int  readPort   (T *expected);
};

#endif
