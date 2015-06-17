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
#include "../settings.h"
#include "../serialportctr.h"
#include "dmmcontrol.h"
#include "../ledindicator.h"

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
class QTextBrowser;
class QLineEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(void);

public slots:
    void initPorts  (QStringList portNames);
    void reinitPorts(QStringList portNames);
    void setError();
    void portsDetected();
    void ctrStarted();
    void ctrStopped();

private:
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
    QLabel       *commandLabel;
    QLabel       *responseLabel;
    QTextBrowser *commandDisplay;
    QTextBrowser *responseDisplay;

    QGroupBox    *settingsGroup;
    QVBoxLayout  *settingsLayout;
    LEDIndicator *displayIndicat;
    QLabel       *trigCountLabel;
    QLineEdit    *trigCountDisplay;
    QLabel       *sampCountLabel;
    QLineEdit    *sampCountDisplay;

    SerialPortCtr *portCtr;
    Settings      *sets;
    DMMControl    *dmmCtr;
    bool          started;

    void initThreads (void);
    void initMenu    (void);
    void initLayout  (void);
    void initControls(void);
    void initPortControls(void);

private slots:
    void displayAbout(void);

    void start(void);
    void stop (void);
};

#endif
