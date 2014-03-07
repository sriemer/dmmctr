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
    _measFunctions = QStringList() << "CONF:VOLT:DC";

    measIntegrTimes = QStringList()
        << QString("0.02")
        << QString("0.2")
        << "1" << "10" << "100";
    _measIntegrTimes = QStringList() << "0.020000" << "0.200000" << "1.000000"
        << "10.000000" << "100.000000";

    measAutoZero = QStringList() << tr("Off") << tr("On") << tr("Once");
    _measAutoZero = QStringList() << "OFF" << "ON" << "ONCE";

    measRates = QStringList() << tr("Slow (2.5/s)") << tr("Medium (5/s)")
        << tr("Fast (20/s)");
    _measRates = QStringList() << "S" << "M" << "F";

    trigSources = QStringList() << tr("Immediate") << tr("External");
    _trigSources = QStringList() << "IMM" << "EXT";

    genDisp = QStringList() << tr("Off") << tr("On");
    _genDisp = QStringList() << "OFF" << "ON";
}
