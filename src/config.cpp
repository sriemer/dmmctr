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

/*
 * The config is set by source code defaults or by XML config file.
 * The GUI elements are accessed by IDs. Some of these need IDs or integer
 * values as configuration. Others need string values.
 *
 * Look at the "Settings" class to find out which setting hides behind a config
 * ID e.g. for a QComboBox.
 */

#include "config.h"
#include <QDebug>

#ifdef Q_OS_WIN
    #define DEFAULT_CSV_APP "excel"
#else
    #define DEFAULT_CSV_APP "libreoffice -calc"
#endif
#define ERR_SIZE_MISMATCH "HAVN'T YOU FORGOTTEN SOMETHING IN THE CONFIG?"

// Constructors
Config::Config()
{
    configIDs = QVector<int>() << 0 << 3 << 0 << 0 << 1 << 0  // Port defaults
                << 0 << 3 << 0 << 0 << 1  // DMM defaults
                << 0;  // Export default

    configStrings = QVector<QString>() << QString("") <<   // Export defaults
                    QString(DEFAULT_CSV_APP) << QString("") << QString("");

    if (configIDs.size() != CFG_ID_SIZE || configStrings.size() != CFG_STR_SIZE)
        qDebug() << ERR_SIZE_MISMATCH;
}

Config::Config(QVector<int> cfgIDs, QVector<QString> cfgStrings)
{
    if (cfgIDs.size() == CFG_ID_SIZE && cfgStrings.size() == CFG_STR_SIZE) {
        configIDs = cfgIDs;
        configStrings = cfgStrings;
    } else {
        qDebug() << ERR_SIZE_MISMATCH;
    }
}

QVector<int> Config::getIDs()
{
    return configIDs;
}

int Config::getID(ConfigIDType idx)
{
    if (idx >= 0 && idx < CFG_ID_SIZE) {
        return configIDs.at(idx);
    } else {
        qDebug() << ERR_SIZE_MISMATCH;
        return -1;
    }
}

QString Config::getStr(ConfigStrType idx)
{
    if (idx >= 0 && idx < CFG_STR_SIZE) {
        return configStrings.at(idx);
    } else {
        qDebug() << ERR_SIZE_MISMATCH;
        return "";
    }
}

void Config::setIDs(QVector<int> cfgIDs)
{
    if (cfgIDs.size() == CFG_ID_SIZE)
        configIDs = cfgIDs;
    else
        qDebug() << ERR_SIZE_MISMATCH;
}

void Config::setID(ConfigIDType idx, int id)
{
    if (idx >= 0 && idx < CFG_ID_SIZE)
        configIDs.replace(idx, id);
    else
        qDebug() << ERR_SIZE_MISMATCH;
}

void Config::setStr(ConfigStrType idx, QString str)
{
    if (idx >= 0 && idx < CFG_STR_SIZE)
        configStrings.replace(idx, str);
    else
        qDebug() << ERR_SIZE_MISMATCH;
}
