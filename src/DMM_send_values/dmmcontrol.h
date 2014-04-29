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

typedef enum {
    ERR_NONE,
    ERR_TIMEOUT,
    ERR_TERM,
    ERR_UNWANTED
} DMMErrorType;

typedef enum {
    // Requests
    OPC_ID,
    IDN_ID,
    FETC_ID,
    // Commands
    CLS_ID,
    SYST_ID,
    RST_ID,
    ESE_ID,
    SRE_ID,
    STAT_ID,
    ZERO_ID,
    CONF_ID,
    VOLT_ID,
    TRIG_ID,
    SAMPLES_ID,
    INIT_ID,
    DISPLAY_ID,
    CMDS_SIZE  // always the last entry
} CmdIDType;

class CmdEntry
{
public:
    QString cmd;
    QString answer;
    QVector<CmdEntry> *subcmds;
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
    QVector<CmdEntry> cmds;
    QString message;
    int     timeout;
    bool    stopRequested;
    bool    ready;

    void initRequests (void);
    void initCmds     (void);
    void stopDMMCtr   (void);
    int  emulateDMM   (void);
    int  readAndSendBack   (void);
    DMMErrorType readPort  (void);
    DMMErrorType handleCmd (CmdIDType id, QString *cmd, QString *answer);
    DMMErrorType handleSubCmds (CmdIDType id, QString *cmd, QString *answer);
    void handleFetch  (void);
};

#endif
