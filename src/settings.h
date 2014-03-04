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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <qextserialport.h>
#include <QObject>
#include <QStringList>
#include <QVector>

class Settings : QObject
{
    Q_OBJECT

friend class MainWindow;
friend class SerialPortCtr;
friend class DMMControl;

protected:
    Settings();

    // Port Settings
    QStringList baudRates;
    QVector<BaudRateType> baudIDs;
    QStringList flowCtrs;
    QVector<FlowType> flowIDs;
    QStringList parities;
    QVector<ParityType> parityIDs;
    QStringList dataBits;
    QVector<DataBitsType> dataBitsIDs;
    QStringList stopBits;
    QVector<StopBitsType> stopBitsIDs;
    // DMM Settings
    QStringList measFunctions;
    QStringList measFunctCmds;
    QStringList measIntegrTimes;
    QStringList measIntegrTimes2;
    QStringList measAutoZero;
    QStringList measAutoZero2;
    QStringList trigSources;
    QStringList trigSources2;
    QStringList trigRates;
    QStringList trigRates2;

    // Port Settings
    inline BaudRateType getBaudID(int idx)
    {
        return baudIDs.at(idx);
    }

    inline FlowType getFlowID(int idx)
    {
        return flowIDs.at(idx);
    }

    inline ParityType getParityID(int idx)
    {
        return parityIDs.at(idx);
    }

    inline DataBitsType getDataBitsID(int idx)
    {
        return dataBitsIDs.at(idx);
    }

    inline StopBitsType getStopBitsID(int idx)
    {
        return stopBitsIDs.at(idx);
    }

    // DMM Settings
    inline QString getMeasFunctCmd(int idx)
    {
        return measFunctCmds.at(idx);
    }

    inline QString getMeasIntegrTime(int idx)
    {
        return measIntegrTimes2.at(idx);
    }

    inline QString getMeasAutoZero(int idx)
    {
        return measAutoZero2.at(idx);
    }

    inline QString getTrigSource(int idx)
    {
        return trigSources2.at(idx);
    }

    inline QString getTrigRate(int idx)
    {
        return trigRates2.at(idx);
    }
};

#endif
