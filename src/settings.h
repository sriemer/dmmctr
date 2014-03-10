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
#include <QDebug>
#include <QObject>
#include <QStringList>
#include <QVector>

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>

typedef enum {
    PORT_ID,
    BAUD_ID,
    FLOW_ID,
    PARITY_ID,
    DATA_BITS_ID,
    STOP_BITS_ID,
    FUNCT_ID,
    INTEGR_ID,
    AUTOZ_ID,
    MEAS_RATE_ID,
    TRIG_SRC_ID,
    SAMP_ID,
    DISP_ID,
    EXPORT_ID,
    CSV_DIR_STR,
    CSV_PATH_STR,
    XLS_DIR_STR,
    XLS_PATH_STR,
    SETTINGS_SIZE  // always the last entry
} SetIDType;

typedef enum {
    DISP_NONE,
    DISP_STRINGS,
    DISP_SIZE  // always last
} DispType;

typedef enum {
    VAL_NONE,
    VAL_BAUD,
    VAL_FLOW,
    VAL_PARITY,
    VAL_DATA_BITS,
    VAL_STOP_BITS,
    VAL_STRINGS,
    VAL_INTS,
    VAL_SIZE   // always last
} ValType;

typedef enum {
    CFG_ID,
    CFG_INT,
    CFG_STRING,
    CFG_TYPES   // always last
} CfgType;

struct SetValues {
    QVector<BaudRateType> baudIDs;
    QVector<FlowType> flowIDs;
    QVector<ParityType> parityIDs;
    QVector<DataBitsType> dataBitsIDs;
    QVector<StopBitsType> stopBitsIDs;
    QVector<int> ints;
    QStringList strings;
};

struct CfgValue {
    int id;
    QString str;
};

class SetEntry
{
public:
    QString     name;
    QString     lblText;
    DispType    dispType;
    QStringList dispStrings;
    ValType     valType;
    SetValues   values;
    CfgType     cfgType;
    CfgValue    defVal;
    CfgValue    cfgVal;
};

class Settings : QObject
{
    Q_OBJECT

friend class MainWindow;
friend class SerialPortCtr;
friend class DMMControl;
friend class ConfigXml;

protected:
    Settings();

    inline void initComboBox(SetIDType id, QStringList sl, QComboBox *combo)
    {
         combo->addItems(sl);
         if (sets[id].cfgVal.id < sl.size())
             combo->setCurrentIndex(sets[id].cfgVal.id);
         else
             combo->setCurrentIndex(sl.size() - 1);
    }

    inline void initComboBox(SetIDType id, QLabel *lbl, QComboBox *combo)
    {
         lbl->setText(sets[id].lblText);
         combo->addItems(sets[id].dispStrings);
         if (sets[id].cfgVal.id < sets[id].dispStrings.size())
             combo->setCurrentIndex(sets[id].cfgVal.id);
         else
             combo->setCurrentIndex(sets[id].dispStrings.size() - 1);
    }

    inline void initSpinBox(SetIDType id, QLabel *lbl, QSpinBox *spin)
    {
        lbl->setText(sets[id].lblText);
        spin->setMinimum(sets[id].values.ints.at(0));
        spin->setMaximum(sets[id].values.ints.at(1));
        spin->setValue(sets[id].cfgVal.id);
    }

    inline QString getStringFromID(SetIDType id)
    {
        int cfg_id = sets[id].cfgVal.id;
        return sets[id].values.strings.value(cfg_id);
    }

    inline BaudRateType getBaudFromID(SetIDType id)
    {
        int cfg_id = sets[id].cfgVal.id;
        return sets[id].values.baudIDs.value(cfg_id);
    }

    inline ParityType getParityFromID(SetIDType id)
    {
        int cfg_id = sets[id].cfgVal.id;
        return sets[id].values.parityIDs.value(cfg_id);
    }

    inline FlowType getFlowFromID(SetIDType id)
    {
        int cfg_id = sets[id].cfgVal.id;
        return sets[id].values.flowIDs.value(cfg_id);
    }

    inline DataBitsType getDataBitsFromID(SetIDType id)
    {
        int cfg_id = sets[id].cfgVal.id;
        return sets[id].values.dataBitsIDs.value(cfg_id);
    }

    inline StopBitsType getStopBitsFromID(SetIDType id)
    {
        int cfg_id = sets[id].cfgVal.id;
        return sets[id].values.stopBitsIDs.value(cfg_id);
    }

    inline int getCfgID(SetIDType id)
    {
        return sets[id].cfgVal.id;
    }

    inline int getCfgInt(SetIDType id)
    {
        return getCfgID(id);
    }

    inline QString getCfgStr(SetIDType id)
    {
        return sets[id].cfgVal.str;
    }

    inline void setCfgID(SetIDType id, int id_val)
    {
        sets[id].cfgVal.id = id_val;
    }

    inline void setCfgID(QString name, int id_val)
    {
        for (int i = 0; i < SETTINGS_SIZE; i++) {
            if (sets[i].name == name) {
                sets[i].cfgVal.id = id_val;
                break;
            }
        }
    }

    inline void setCfgStr(SetIDType id, QString str)
    {
        sets[id].cfgVal.str = str;
    }

    inline void setCfgStr(QString name, QString str)
    {
        for (int i = 0; i < SETTINGS_SIZE; i++) {
            if (sets[i].name == name) {
                sets[i].cfgVal.str = str;
                break;
            }
        }
    }

    inline CfgType getCfgType(SetIDType id)
    {
        return sets[id].cfgType;
    }

    inline QString getName(SetIDType id)
    {
        return sets[id].name;
    }

private:
    QVector<SetEntry> sets;
};

#endif
