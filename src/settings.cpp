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

#include "settings.h"

// Constructors
Settings::Settings()
{
    // Selectable Port Settings
    baudRates = QStringList() << "1200" << "2400" << "4800"
        << "9600" << "19200" << "38400" << "57600" << "115200";
    baudIDs = QVector<BaudRateType>() << BAUD1200 << BAUD2400 << BAUD4800
        << BAUD9600 << BAUD19200 << BAUD38400 << BAUD57600 << BAUD115200;

    flowCtrs = QStringList() << tr("None") << "XON/XOFF" << "RTS/CTS";
    flowIDs = QVector<FlowType>() << FLOW_OFF << FLOW_XONXOFF << FLOW_HARDWARE;

    parities = QStringList() << tr("None") << tr("Odd") << tr("Even");
    parityIDs = QVector<ParityType>() << PAR_NONE << PAR_ODD << PAR_EVEN;

    dataBits = QStringList() << "7" << "8";
    dataBitsIDs = QVector<DataBitsType>() << DATA_7 << DATA_8;

    stopBits = QStringList()
        << QString("1.0") << QString("2.0");
    stopBitsIDs = QVector<StopBitsType>() << STOP_1 << STOP_2;

    // Selectable DMM Settings
    measFunctions = QStringList() << tr("DC Volts");
    measFunctCmds = QStringList() << "CONF:VOLT:DC";

    measIntegrTimes = QStringList()
        << QString("0.02")
        << QString("0.2")
        << "1" << "10" << "100";
    measIntegrTimes2 = QStringList() << "0.020000" << "0.200000" << "1.000000"
        << "10.000000" << "100.000000";

    measAutoZero = QStringList() << tr("Off") << tr("On") << tr("Once");
    measAutoZero2 = QStringList() << "OFF" << "ON" << "ONCE";

    trigSources = QStringList() << tr("Immediate") << tr("External");
    trigSources2 = QStringList() << "IMM" << "EXT";

    trigRates = QStringList() << tr("Slow (2.5/s)") << tr("Medium (5/s)")
        << tr("Fast (20/s)");
    trigRates2 = QStringList() << "S" << "M" << "F";
}

// Port Settings
QStringList Settings::getBaudRates()
{
    return baudRates;
}

BaudRateType Settings::getBaudID(int idx)
{
    return baudIDs.at(idx);
}

QStringList Settings::getFlowCtrs()
{
    return flowCtrs;
}

FlowType Settings::getFlowID(int idx)
{
    return flowIDs.at(idx);
}

QStringList Settings::getParities()
{
    return parities;
}

ParityType Settings::getParityID(int idx)
{
    return parityIDs.at(idx);
}

QStringList Settings::getDataBits()
{
    return dataBits;
}

DataBitsType Settings::getDataBitsID(int idx)
{
    return dataBitsIDs.at(idx);
}

QStringList Settings::getStopBits()
{
    return stopBits;
}

StopBitsType Settings::getStopBitsID(int idx)
{
    return stopBitsIDs.at(idx);
}

// DMM Settings
QStringList Settings::getMeasFunctions()
{
    return measFunctions;
}

QString Settings::getMeasFunctCmd(int idx)
{
    return measFunctCmds.at(idx);
}

QStringList Settings::getMeasIntegrTimes()
{
    return measIntegrTimes;
}

QString Settings::getMeasIntegrTime(int idx)
{
    return measIntegrTimes2.at(idx);
}

QStringList Settings::getMeasAutoZero()
{
    return measAutoZero;
}

QString Settings::getMeasAutoZero(int idx)
{
    return measAutoZero2.at(idx);
}

QStringList Settings::getTrigSources()
{
    return trigSources;
}

QString Settings::getTrigSource(int idx)
{
    return trigSources2.at(idx);
}

QStringList Settings::getTrigRates()
{
    return trigRates;
}

QString Settings::getTrigRate(int idx)
{
    return trigRates2.at(idx);
}
