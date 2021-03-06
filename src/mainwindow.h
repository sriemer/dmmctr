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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>
#ifndef QT_VERSION
    #error QT_VERSION not defined!
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets>
#endif
#include <QMainWindow>
#include "settings.h"
#include "serialportctr.h"
#include "dmmcontrol.h"
#include "ledindicator.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QFrame;
class QLabel;
class QComboBox;
class QPushButton;
class QTabWidget;
class QSpinBox;
class QLineEdit;
class QFileDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(void);

public slots:
    void initPorts  (QStringList portNames);
    void reinitPorts(QStringList portNames);
    void setTimeout();
    void clearTimeout();
    void setError();
    void clearError();
    void portsDetected();
    void setResults(QStringList strList);
    void ctrStarted();
    void ctrStopped();

private:
    QAction     *exportCsv;
    QAction     *exportXls;

    QWidget     *mainWidget;
    QHBoxLayout *mainLayout;
    QGroupBox   *portGroup;
    QVBoxLayout *portLayout;
    QLabel      *portLabel;
    QComboBox   *portComboBox;
    QLabel      *baudLabel;
    QComboBox   *baudComboBox;
    QLabel      *flowLabel;
    QComboBox   *flowComboBox;
    QLabel      *parityLabel;
    QComboBox   *parityComboBox;
    QLabel      *dataBitsLabel;
    QComboBox   *dataBitsComboBox;
    QLabel      *stopBitsLabel;
    QComboBox   *stopBitsComboBox;

    QWidget      *centerWidget;
    QVBoxLayout  *centerLayout;
    QFrame       *progCtrFrame;
    QHBoxLayout  *progCtrLayout;
    QPushButton  *startButton;
    QPushButton  *stopButton;
    LEDIndicator *timeoutIndicat;
    LEDIndicator *errorIndicat;
    QTabWidget   *dmmTabWidget;
    QWidget      *measWidget;
    QGridLayout  *measLayout;
    QLabel       *measFunctLbl;
    QComboBox    *measFunct;
    QLabel       *measIntegrTimeLbl;
    QComboBox    *measIntegrTime;
    QLabel       *measAutoZeroLbl;
    QComboBox    *measAutoZero;
    QLabel       *measRangeLbl;
    QComboBox    *measRange;
    QLabel       *measResolLbl;
    QComboBox    *measResol;
    QWidget      *trigWidget;
    QGridLayout  *trigLayout;
    QLabel       *trigSourceLbl;
    QComboBox    *trigSource;
    QLabel       *trigAutoDelLbl;
    QComboBox    *trigAutoDel;
    QLabel       *trigDelayLbl;
    QSpinBox     *trigDelay;
    QLabel       *trigCountLbl;
    QSpinBox     *trigCount;
    QLabel       *trigSamplesLbl;
    QSpinBox     *trigSamples;
    QLabel       *genDispLbl;
    QComboBox    *genDisp;
    QLabel       *exportLbl;
    QWidget      *exportWidget;
    QHBoxLayout  *exportLayout;
    QLineEdit    *exportLineEdit;
    QPushButton  *exportSelButton;
    QPushButton  *exportClrButton;

    SerialPortCtr *portCtr;
    Settings      *sets;
    DMMControl    *dmmCtr;
    bool          started;

    void initThreads      (void);
    void initMenu         (void);
    void initLayout       (void);
    void initControls     (void);
    void initPortControls (void);

private slots:
    // Menu actions
    void selectExportCsv (void);
    void selectExportXls (void);
    void displayAbout    (void);

    // Button events
    void start           (void);
    void stop            (void);
    void selectCsvApp    (void);
    void selectXls       (void);
    void clearExportText (void);

    // ComboBox events
    void changeFunction  (int id);
};

#endif
