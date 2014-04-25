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

#include "../settings.h"
#include "../serialportctr.h"

enum DMMErrorType {
    ERR_NONE,
    ERR_TIMEOUT,
    ERR_TERM,
    ERR_UNWANTED
};

class RqEntry
{
public:
    QRegExp     request;
    QString     answer;
};

class DMMControl : public QThread
{
    Q_OBJECT

public:
    DMMControl(SerialPortCtr *portControl, Settings *sets);
    ~DMMControl();
    void run();
    bool isReady();

signals:
    void sendEnable();
    void sendSetTimeout();
    void sendClearTimeout();
    void sendSetError();
    void sendClearError();
    void sendSetCommand(QString cmd);
    void sendSetAnswer(QString answ);
    void sendStarted();
    void sendStopped();

public slots:
    void setReady();
    void requestStop();

private:
    QextSerialPort *serPort;
    SerialPortCtr  *portCtr;
    Settings *sets;
    QVector<RqEntry> rqs;
    QString message;
    int     timeout;
    bool    stopRequested;
    bool    ready;

    void initRequests (void);
    void stopDMMCtr   (void);
    int  emulateDMM   (void);
    void handleFetch  (void);
    int  readAndSendBack (QString term);
    int  readPort     (void);
};

#endif
