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

#include <QtCore>

#include "dmmcontrol.h"
#include "../serialportctr.h"
#include "../settings.h"

#define RD_BUFF_SIZE   4096
#define RD_POLL_RATE     50  // ms
#define RD_DEF_TIMEOUT 1000  // ms

#define DMM_FETC_DBL  -0.000200

typedef enum {
    COUN_ID
} DMMTrigType;


DMMControl::DMMControl(SerialPortCtr *portControl, Settings *settings)
{
    serPort = NULL;
    portCtr = portControl;
    sets = settings;
    stopRequested = false;
    ready = false;
    timeout = RD_DEF_TIMEOUT;
    initCmds();
    trigCount = 1;
    sampCount = 1;
    qDebug() << "DMMControl created";
}

DMMControl::~DMMControl()
{
    stopDMMCtr();
}

void DMMControl::initCmds(void)
{
    int idx;
    CmdEntry cmd_en;
    cmds = QVector<CmdEntry>(CMDS_SIZE);

    idx = OPC_ID;
    cmds[idx].cmd = "*OPC?";
    cmds[idx].response = "1";
    cmds[idx].subcmds = NULL;

    idx = IDN_ID;
    cmds[idx].cmd = "*IDN?";
    cmds[idx].response = "TEKTRONIX,DMM4050,1555202,08/02/10-11:53";
    cmds[idx].subcmds = NULL;

    idx = FETC_ID;
    cmds[idx].cmd = "FETC?";
    cmds[idx].response = "-0.000200";
    cmds[idx].subcmds = NULL;

    idx = CLS_ID;
    cmds[idx].cmd = "*CLS";
    cmds[idx].subcmds = NULL;

    idx = SYST_ID;
    cmds[idx].cmd = "SYST";
    cmds[idx].subcmds = new QVector<CmdEntry>();
    cmd_en.cmd = "REM";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->append(cmd_en);
    cmd_en.cmd = "ERR";
    cmd_en.response = "+0,\"No error\"";
    cmd_en.subcmds = new QVector<CmdEntry>();
    cmds[idx].subcmds->append(cmd_en);
    cmd_en.cmd = "BEEPER";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->last().subcmds->append(cmd_en);

    idx = RST_ID;
    cmds[idx].cmd = "*RST";
    cmds[idx].subcmds = NULL;

    idx = ESE_ID;
    cmds[idx].cmd = "*ESE";
    cmds[idx].subcmds = NULL;

    idx = SRE_ID;
    cmds[idx].cmd = "*SRE";
    cmds[idx].subcmds = NULL;

    idx = STAT_ID;
    cmds[idx].cmd = "STAT";
    cmds[idx].subcmds = new QVector<CmdEntry>();
    cmd_en.cmd = "QUES";
    cmd_en.response.clear();
    cmd_en.subcmds = new QVector<CmdEntry>();
    cmds[idx].subcmds->append(cmd_en);
    cmd_en.cmd = "ENAB";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->last().subcmds->append(cmd_en);
    cmd_en.cmd = "EVEN";
    cmd_en.response = "+8192";
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->last().subcmds->append(cmd_en);

    idx = ZERO_ID;
    cmds[idx].cmd = "ZERO";
    cmds[idx].subcmds = new QVector<CmdEntry>();
    cmd_en.cmd = "AUTO";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->append(cmd_en);

    idx = CONF_ID;
    cmds[idx].cmd = "CONF";
    cmds[idx].subcmds = new QVector<CmdEntry>();
    cmd_en.cmd = "VOLT";
    cmd_en.response.clear();
    cmd_en.subcmds = new QVector<CmdEntry>();
    cmds[idx].subcmds->append(cmd_en);
    cmd_en.cmd = "DC";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->last().subcmds->append(cmd_en);

    idx = VOLT_ID;
    cmds[idx].cmd = "VOLT";
    cmds[idx].subcmds = new QVector<CmdEntry>();
    cmd_en.cmd = "DC";
    cmd_en.response.clear();
    cmd_en.subcmds = new QVector<CmdEntry>();
    cmds[idx].subcmds->append(cmd_en);
    cmd_en.cmd = "NPLC";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->last().subcmds->append(cmd_en);

    idx = TRIG_ID;
    cmds[idx].cmd = "TRIG";
    cmds[idx].subcmds = new QVector<CmdEntry>();
    cmd_en.cmd = "COUN";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->append(cmd_en);
    cmd_en.cmd = "DEL";
    cmd_en.response.clear();
    cmd_en.subcmds = new QVector<CmdEntry>();
    cmds[idx].subcmds->append(cmd_en);
    cmd_en.cmd = "AUTO";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->last().subcmds->append(cmd_en);
    cmd_en.cmd = "SOUR";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->append(cmd_en);

    idx = SAMPLES_ID;
    cmds[idx].cmd = "SAMP";
    cmds[idx].subcmds = new QVector<CmdEntry>();
    cmd_en.cmd = "COUN";
    cmd_en.response.clear();
    cmd_en.subcmds = NULL;
    cmds[idx].subcmds->append(cmd_en);

    idx = INIT_ID;
    cmds[idx].cmd = "INIT";
    cmds[idx].subcmds = NULL;

    idx = DISPLAY_ID;
    cmds[idx].cmd = "DISP";
    cmds[idx].subcmds = NULL;
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
        ret = readAndSendBack();
    } while (!ret && !stopRequested);

    return ret;
}

int DMMControl::readAndSendBack(void)
{
#define READ_TRIES 2
    DMMErrorType error;
    int i, pos = 0, end = 0;
    QString cmd;
    QString response;

    command.clear();
    // concatenate timed out non-empty command strings
    for (i = 0; i < READ_TRIES; i++) {
        error = readPort();
        if (error != ERR_TIMEOUT)
            break;
        emit sendSetTimeout();
        if (command.isEmpty()) {
            error = ERR_NONE;
            goto out;
        }
        if (i == (READ_TRIES - 1))  // unlikely: not empty and still timed out
            error = ERR_UNWANTED;
        else
            qDebug() << "Timed out incomplete command received - retrying.";
    }
    emit sendSetCommand(command);
    qDebug() << command;
    emit sendClearTimeout();
    if (error) {
        emit sendSetError();
        goto out;
    }
    while (true) {
        if (command.startsWith('\n'))
            command.remove(0, 1);
        if (command.startsWith(':'))
            command.remove(0, 1);
        if (command.isEmpty())
            break;
        end = command.indexOf(';');
        pos = command.indexOf('\n');
        if (end >= 0) {
            if (pos >= 0 && pos < end)
                end = pos;
            cmd = command.left(end);
        } else if (pos >= 0) {
            end = pos;
            cmd = command.left(end);
        } else {
            cmd = command;
        }
        error = ERR_UNWANTED;
        qDebug() << "cmd:" << cmd;
        for (int i = 0; i < cmds.size(); i++) {
            if (!cmd.startsWith(cmds.at(i).cmd))
                continue;
            error = handleCmd((CmdIDType) i, &cmd, &response);
            break;
        }
        if (error == ERR_UNWANTED) {
            emit sendSetError();
            //error = ERR_NONE;   // For testing only!
            goto out;
        }
        if (end < 0)
            break;
        command.remove(0, end + 1);
    }
    if (!response.isEmpty()) {
        response.append("\r\n");
        qDebug() << "response: " << response;
        serPort->write(response.toLatin1(), response.length());
        if (serPort->lastError()) {
            qDebug() << serPort->errorString();
            error = ERR_UNWANTED;
        }
    }
out:
    emit sendSetResponse(response);
    return error;
#undef READ_TRIES
}

DMMErrorType DMMControl::readPort(void)
{
    char buff[RD_BUFF_SIZE + 1];
    int  numBytes;
    int  readBytes;
    int  i = 0;
    DMMErrorType error = ERR_TIMEOUT;

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
            if (serPort->lastError())
                break;

            command.append(buff);
        }

        if (serPort->lastError()) {
            qDebug() << serPort->errorString();
            error = ERR_UNWANTED;
            break;
        }
        if (!command.isEmpty() && command.endsWith("\n")) {
            error = ERR_NONE;
            break;
        }
    } while (i <= (timeout / RD_POLL_RATE));

    return error;
}

DMMErrorType DMMControl::handleCmd(CmdIDType id, QString *cmd, QString *response)
{
    DMMErrorType error = ERR_NONE;
    QString value;
    int pos;

    pos = cmd->indexOf(' ');
    if (pos >= 0 && pos + 1 < cmd->size()) {
        value = cmd->right(cmd->size() - (pos + 1));
        qDebug() << "value:" << value;
    } else {
        value = "";
    }

    // Is this a request?
    switch (id) {
    // Requests
    case FETC_ID:
        handleFetch();
    case OPC_ID:
    case IDN_ID:
        response->append(cmds.at(id).response);
        break;
    // Regular commands
    case STAT_ID:
    case SYST_ID:
    case ZERO_ID:
    case CONF_ID:
    case VOLT_ID:
    case TRIG_ID:
    case SAMPLES_ID:
        error = handleSubCmds(id, cmd, response, &value);
        break;
    case CLS_ID:
    case RST_ID:
    case ESE_ID:
    case SRE_ID:
    case INIT_ID:
        break;
    case DISPLAY_ID:
        if (value.startsWith("ON", Qt::CaseInsensitive))
            emit sendDisplayOn();
        else if (value.startsWith("OFF", Qt::CaseInsensitive))
            emit sendDisplayOff();
        else
            error = ERR_UNWANTED;
        break;
    default:
        error = ERR_UNWANTED;
        break;
    }
    return error;
}

DMMErrorType DMMControl::handleSubCmds(CmdIDType id, QString *cmd,
                                       QString *response, QString *value)
{
    DMMErrorType error = ERR_UNWANTED;
    QVector<CmdEntry> *subcmds;
    QString subcmd;
    int cmd_ids[3] = { (int) id, -1, -1 };
    int pos;

    subcmds = cmds.at(id).subcmds;
    pos = cmd->indexOf(':');
    if (pos < 0)
        goto out;
    cmd->remove(0, pos + 1);
    for (int i = 0; i < 2; i++) {
        bool found = false;
        pos = cmd->indexOf(':');
        if (pos >= 0) {
            subcmd = cmd->left(pos);
            cmd->remove(0, pos + 1);
        } else {
            subcmd = *cmd;
        }
        qDebug() << "subcmd:" << subcmd;
        for (int j = 0; j < subcmds->size(); j++) {
            if (!subcmd.startsWith(subcmds->at(j).cmd))
                continue;
            if (subcmd.lastIndexOf('?') >= 0) {
                cmd_ids[i + 1] = j;
                response->append(subcmds->at(j).response);
                error = ERR_NONE;
                goto out;
            }
            if (!subcmds->at(j).subcmds) {
                cmd_ids[i + 1] = j;
                handleSubCmd(cmd_ids, value);
                error = ERR_NONE;
                goto out;
            }
            subcmds = subcmds->at(j).subcmds;
            cmd_ids[i + 1] = j;
            found = true;
            break;
        }
        if (!found)
            goto out;
        if (pos < 0) {
            error = ERR_NONE;
            goto out;
        }
    }
out:
    return error;
}

void DMMControl::handleSubCmd(int cmd_ids[], QString *value)
{
    switch (cmd_ids[0]) {
    case TRIG_ID:
        switch (cmd_ids[1]) {
        case COUN_ID:
            trigCount = value->toInt();
            sendTrigCount(*value);
            break;
        default:
            break;
        }
        break;
    case SAMPLES_ID:
        switch (cmd_ids[1]) {
        case COUN_ID:
            sampCount = value->toInt();
            sendSampCount(*value);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void DMMControl::handleFetch(void)
{
    double voltage;
    QString voltStr;
    int id = (int) FETC_ID;

    cmds[id].response.clear();
    for (int i = 0; i < sampCount * trigCount; i++) {
        voltage = DMM_FETC_DBL;
        voltage += ((double) qrand() / (double) RAND_MAX - 0.5) / 10000;
        voltStr.setNum(voltage, 'f', 6);
        cmds[id].response += voltStr;
        cmds[id].response += ',';
    }
    cmds[id].response.chop(1);
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
