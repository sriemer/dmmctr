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

public:
    Settings();
    QStringList  getBaudRates  (void);
    BaudRateType getBaudID     (int idx);
    QStringList  getFlowCtrs   (void);
    FlowType     getFlowID     (int idx);
    QStringList  getParities   (void);
    ParityType   getParityID   (int idx);
    QStringList  getDataBits   (void);
    DataBitsType getDataBitsID (int idx);
    QStringList  getStopBits   (void);
    StopBitsType getStopBitsID (int idx);

    QStringList  getMeasFunctions  (void);
    QString      getMeasFunctCmd   (int idx);
    QStringList  getMeasIntegrTimes(void);
    QString      getMeasIntegrTime (int idx);
    QStringList  getMeasAutoZero   (void);
    QString      getMeasAutoZero   (int idx);
    QStringList  getTrigSources    (void);
    QString      getTrigSource     (int idx);

private:
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
    QStringList measFunctions;
    QStringList measFunctCmds;
    QStringList measIntegrTimes;
    QStringList measIntegrTimes2;
    QStringList measAutoZero;
    QStringList measAutoZero2;
    QStringList trigSources;
    QStringList trigSources2;
};

#endif
