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
        << "9600" << "19200" << "38400" << "57600" << "115200";
    sets[idx].valType   = VAL_BAUD;
    sets[idx].values.baudIDs = QVector<BaudRateType>() << BAUD1200 << BAUD2400 << BAUD4800
        << BAUD9600 << BAUD19200 << BAUD38400 << BAUD57600 << BAUD115200;
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
    sets[idx].dispVals.strings = QStringList() << tr("DC Volts");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "CONF:VOLT:DC";
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
        << (QStringList() << "100 mV" << "1000 mV" << "10 V" << "100 V" << "1000 V")
        << (QStringList() << "300 mV" << "3000 mV" << "30 V" << "300 V" << "1000 V");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "1" << "2" << "3" << "4" << "5";
    sets[idx].cfgType    = CFG_ID;
    sets[idx].defVal.id  = 0;
    sets[idx].defVal.id2 = 0;
    sets[idx].cfgVal.id  = 0;
    sets[idx].cfgVal.id2 = 0;

    idx = MEAS_RATE_ID;
    sets[idx].name      = "rate";
    sets[idx].lblText   = tr("Rate");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("Slow (2.5/s)") << tr("Medium (5/s)")
        << tr("Fast (20/s)");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "S" << "M" << "F";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = TRIG_SRC_ID;
    sets[idx].name      = "trigs";
    sets[idx].lblText   = tr("Trigger Source");
    sets[idx].dispType  = DISP_STRINGS;
    sets[idx].dispVals.strings = QStringList() << tr("Immediate") << tr("External");
    sets[idx].valType   = VAL_STRINGS;
    sets[idx].values.strings = QStringList() << "IMM" << "EXT";
    sets[idx].cfgType   = CFG_ID;
    sets[idx].defVal.id = 0;
    sets[idx].cfgVal.id = 0;

    idx = TRIG_CNT_ID;
    sets[idx].name      = "trigc";
    sets[idx].lblText   = tr("Trigger Count");
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_INTS;
    sets[idx].values.ints = QVector<int>() << 1 << 5000;
    sets[idx].cfgType   = CFG_INT;
    sets[idx].defVal.id = 1;
    sets[idx].cfgVal.id = 1;

    idx = SAMP_ID;
    sets[idx].name      = "sampl";
    sets[idx].lblText   = tr("Samples");
    sets[idx].dispType  = DISP_NONE;
    sets[idx].valType   = VAL_INTS;
    sets[idx].values.ints = QVector<int>() << 1 << 5000;
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
