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

#include "settings.h"

#ifdef Q_OS_WIN
    #define DEFAULT_CSV_APP "excel"
#else
    #define DEFAULT_CSV_APP "libreoffice --calc"
#endif

// Constructors
Settings::Settings()
{
    int idx;
    sets = QVector<SetEntry>(SETTINGS_SIZE);

    // Selectable Port Settings
    idx = PORT_ID;
    sets[idx].name      = "port";
    sets[idx].lblText   = tr("DMM Port");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = BAUD_ID;
    sets[idx].name      = "baud";
    sets[idx].lblText   = tr("Baud Rate");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << "1200" << "2400" << "4800"
        << "9600" << "19200" << "38400" << "57600" << "115200" << "230400";
    sets[idx].valType   = VAL_BAUD;
    sets[idx].values.baudIDs = QVector<BaudRateType>() << BAUD1200 << BAUD2400 << BAUD4800
        << BAUD9600 << BAUD19200 << BAUD38400 << BAUD57600 << BAUD115200 << (BaudRateType) 230400;
    // 230400 may not be available on your serial port driver on Windows and may require
    // custom changes to QextSerialPort as 230400 is non-standard.
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 3;
    sets[idx].cfgVal.id = 3;

    idx = FLOW_ID;
    sets[idx].name      = "flow";
    sets[idx].lblText   = tr("Flow Control");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("None") << "XON/XOFF" << "RTS/CTS";
    sets[idx].valType   = VAL_FLOW;
    sets[idx].values.flowIDs = QVector<FlowType>() << FLOW_OFF << FLOW_XONXOFF << FLOW_HARDWARE;
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = PARITY_ID;
    sets[idx].name      = "parity";
    sets[idx].lblText   = tr("Parity");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("None") << tr("Odd") << tr("Even");
    sets[idx].valType   = VAL_PARITY;
    sets[idx].values.parityIDs = QVector<ParityType>() << PAR_NONE << PAR_ODD << PAR_EVEN;
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = DATA_BITS_ID;
    sets[idx].name      = "data_b";
    sets[idx].lblText   = tr("Data Bits");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << "7" << "8";
    sets[idx].valType   = VAL_DATA_BITS;
    sets[idx].values.dataBitsIDs = QVector<DataBitsType>() << DATA_7 << DATA_8;
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 1;
    sets[idx].cfgVal.id = 1;

    idx = STOP_BITS_ID;
    sets[idx].name      = "stop_b";
    sets[idx].lblText   = tr("Stop Bits");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << QString("1.0") << QString("2.0");
    sets[idx].valType   = VAL_STOP_BITS;
    sets[idx].values.stopBitsIDs = QVector<StopBitsType>() << STOP_1 << STOP_2;
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    // Selectable DMM Settings
    idx = FUNCT_ID;
    sets[idx].name      = "funct";
    sets[idx].lblText   = tr("Function");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("DC Volts") << tr("DC Current");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "CONF:VOLT:DC" << "CONF:CURR:DC";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = INTEGR_ID;
    sets[idx].name      = "integ";
    sets[idx].lblText   = tr("Integration Time");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << QString("0.02") << QString("0.2") << "1"
        << "10" << "100";
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "0.020000" << "0.200000" << "1.000000"
        << "10.000000" << "100.000000";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 3;
    sets[idx].cfgVal.id = 3;

    idx = AUTOZ_ID;
    sets[idx].name      = "autoz";
    sets[idx].lblText   = tr("Autozero");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("Off") << tr("On") << tr("Once");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "OFF" << "ON" << "ONCE";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = RANGE_ID;
    sets[idx].name      = "range";
    sets[idx].lblText   = tr("Range");
    sets[idx].dispType  = DISP_SL_VEC;
    sets[idx].dispVals.slVec = QVector<QStringList>()
        << (QStringList() << tr("auto") << "100 mV" << "1 V" << "10 V"
        << "100 V" << "1000 V")
        << (QStringList() << tr("auto") << "100 uA" << "1 mA" << "10 mA"
        << "100 mA" << "1 A" << "10 A");
    sets[idx].valType   = VAL_SL_VEC;
    sets[idx].values.slVec = QVector<QStringList>()
        << (QStringList() << "DEF" << "0.1" << "1" << "10" << "100" << "1000")
        << (QStringList() << "DEF" << "0.0001" << "0.001" << "0.01"
        << "0.1" << "1" << "10");
    sets[idx].cfgType    = CFG_ID;
    sets[idx].defVal.id  = 0;
    sets[idx].defVal.id2 = 0;
    sets[idx].cfgVal.id  = 0;
    sets[idx].cfgVal.id2 = 0;

    idx = RESOL_ID;
    sets[idx].name      = "resol";
    sets[idx].lblText   = tr("Resolution");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("default") << tr("min") << tr("max");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "DEF" << "MIN" << "MAX";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = TRIG_SRC_ID;
    sets[idx].name      = "trigs";
    sets[idx].lblText   = tr("Trigger Source");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("Immediate") << tr("External") << "IEEE-488";
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "IMM" << "EXT" << "BUS";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = TRIG_ADE_ID;
    sets[idx].name      = "adel";
    sets[idx].lblText   = tr("Auto Delay");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("Off") << tr("On");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "OFF" << "ON";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 1;
    sets[idx].cfgVal.id = 1;

    idx = TRIG_DEL_ID;
    sets[idx].name      = "trigd";
    sets[idx].lblText   = tr("Trigger Delay");
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_INTS;
    sets[idx].values.ints = QVector<int>() << 0 << 3600;
    sets[idx].cfgType   = CFG_INT;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = TRIG_CNT_ID;
    sets[idx].name      = "trigc";
    sets[idx].lblText   = tr("Trigger Count");
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_INTS;
    sets[idx].values.ints = QVector<int>() << -1 << 50000;  // -1 or 0 for INF
    sets[idx].cfgType   = CFG_INT;
    sets[idx].defVal.id = 1;
    sets[idx].cfgVal.id = 1;

    idx = SAMP_ID;
    sets[idx].name      = "sampl";
    sets[idx].lblText   = tr("Samples");
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_INTS;
    sets[idx].values.ints = QVector<int>() << 1 << 50000;
    sets[idx].cfgType   = CFG_INT;
    sets[idx].defVal.id = 1;
    sets[idx].cfgVal.id = 1;

    idx = DISP_ID;
    sets[idx].name      = "disp";
    sets[idx].lblText   = tr("Display");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("Off") << tr("On");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "OFF" << "ON";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    // Export Settings
    idx = EXPORT_ID;
    sets[idx].name      = "expor";
    sets[idx].lblText   = "expor";
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_NONE;
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = CSV_DIR_STR;
    sets[idx].name      = "csv_app_dir";
    sets[idx].lblText   = "csv_app_dir";
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_NONE;
    sets[idx].cfgType   = CFG_STRING;
    sets[idx].defVal.str = "";
    sets[idx].cfgVal.str = "";

    idx = CSV_PATH_STR;
    sets[idx].name      = "csv_app_path";
    sets[idx].lblText   = "csv_app_path";
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_NONE;
    sets[idx].cfgType   = CFG_STRING;
    sets[idx].defVal.str = DEFAULT_CSV_APP;
    sets[idx].cfgVal.str = DEFAULT_CSV_APP;

    idx = XLS_DIR_STR;
    sets[idx].name      = "xls_dir";
    sets[idx].lblText   = "xls_dir";
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_NONE;
    sets[idx].cfgType   = CFG_STRING;
    sets[idx].defVal.str = "";
    sets[idx].cfgVal.str = "";

    idx = XLS_PATH_STR;
    sets[idx].name      = "xls_path";
    sets[idx].lblText   = "xls_path";
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_NONE;
    sets[idx].cfgType   = CFG_STRING;
    sets[idx].defVal.str = "";
    sets[idx].cfgVal.str = "";
}
