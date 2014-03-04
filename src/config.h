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

#ifndef CONFIG_H
#define CONFIG_H

#include <QVector>
#include <QString>

// Defines
#define CFG_NUM_TYPES 2
#define PROG_NAME "DMM_get_values"
#define PROG_VERSION "1.0.0"

// Alias
#define getIntVal(x) getID(x)

enum ConfigIDType
{
    PORT_ID,
    BAUD_ID,
    FLOW_ID,
    PARITY_ID,
    DATA_BITS_ID,
    STOP_BITS_ID,
    FUNCT_ID,
    INTEGR_ID,
    AUTOZ_ID,
    TRIG_SRC_ID,
    SAMP_ID,
    EXPORT_ID,
    CFG_ID_SIZE  // always the last entry
};

enum ConfigStrType
{
    CSV_DIR_STR,
    CSV_PATH_STR,
    XLS_DIR_STR,
    XLS_PATH_STR,
    CFG_STR_SIZE  // always the last entry
};

class Config
{
public:
    Config();
    Config(QVector<int> cfgIDs, QVector<QString> cfgStrings);

    QVector<int> getIDs (void);
    int     getID   (ConfigIDType idx);
    QString getStr  (ConfigStrType idx);
    void    setIDs  (QVector<int> cfgIDs);
    void    setID   (ConfigIDType idx, int id);
    void    setStr  (ConfigStrType idx, QString str);
private:
    QVector<int>     configIDs;
    QVector<QString> configStrings;
};

#endif
