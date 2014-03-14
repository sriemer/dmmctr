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

#include <QtGui>

#include "mainwindow.h"
#include "settings.h"
#include "configxml.h"
#include "serialportctr.h"
#include "dmmcontrol.h"
#include "report.h"

#define PROG_NAME "DMM_get_values"
#define PROG_VERSION "1.0.0"
#define ICON_DEFAULT_SIZE 28  // px
#define EXPORT_CSV 0
#define EXPORT_XLS 1

MainWindow::MainWindow()
{
    mainWidget         = new QWidget;
    mainLayout         = new QHBoxLayout;
    portGroup          = new QGroupBox;
    portLayout         = new QVBoxLayout;
    portLabel          = new QLabel;
    portComboBox       = new QComboBox;
    baudLabel          = new QLabel;
    baudComboBox       = new QComboBox;
    flowLabel          = new QLabel;
    flowComboBox       = new QComboBox;
    parityLabel        = new QLabel;
    parityComboBox     = new QComboBox;
    dataBitsLabel      = new QLabel;
    dataBitsComboBox   = new QComboBox;
    stopBitsLabel      = new QLabel;
    stopBitsComboBox   = new QComboBox;
    centerWidget       = new QWidget;
    centerLayout       = new QVBoxLayout;
    progCtrFrame       = new QFrame;
    progCtrLayout      = new QHBoxLayout;
    startButton        = new QPushButton;
    stopButton         = new QPushButton;
    timeoutIndicat     = new LEDIndicator(LED_RED, tr("Timeout"), "font: bold");
    errorIndicat       = new LEDIndicator(LED_RED, tr("Error"), "font: bold");
    dmmTabWidget       = new QTabWidget;
    measWidget         = new QWidget;
    measLayout         = new QGridLayout;
    measFunctLbl       = new QLabel;
    measFunct          = new QComboBox;
    measIntegrTimeLbl  = new QLabel;
    measIntegrTime     = new QComboBox;
    measAutoZeroLbl    = new QLabel;
    measAutoZero       = new QComboBox;
    measRangeLbl       = new QLabel;
    measRange          = new QComboBox;
    measRateLbl        = new QLabel;
    measRate           = new QComboBox;
    trigWidget         = new QWidget;
    trigLayout         = new QGridLayout;
    trigSourceLbl      = new QLabel;
    trigSource         = new QComboBox;
    trigCountLbl       = new QLabel;
    trigCount          = new QSpinBox;
    trigSamplesLbl     = new QLabel;
    trigSamples        = new QSpinBox;
    genDispLbl         = new QLabel;
    genDisp            = new QComboBox;
    exportLbl         = new QLabel;
    exportWidget      = new QWidget;
    exportLayout      = new QHBoxLayout;
    exportLineEdit    = new QLineEdit;
    exportSelButton   = new QPushButton;
    exportClrButton   = new QPushButton;

    sets     = new Settings();
    ConfigXml cfgXml(sets);
    cfgXml.readConfigFile(CFG_FILE_DEFAULT);

    started  = false;

    setWindowIcon(QIcon(":/app/images/vmeter.xpm"));

    initThreads();
    initMenu();
    initLayout();
    initControls();

    setCentralWidget(mainWidget);
}

void MainWindow::initThreads()
{
    portCtr  = new SerialPortCtr();
    connect(portCtr, SIGNAL(sendInitPorts(QStringList)),
            this, SLOT(initPorts(QStringList)));
    connect(portCtr, SIGNAL(sendReinitPorts(QStringList)),
            this, SLOT(reinitPorts(QStringList)));
    connect(portCtr, SIGNAL(sendPortsDetected()), this, SLOT(portsDetected()));
    portCtr->start();

    dmmCtr = new DMMControl(portCtr, sets);
    connect(portCtr, SIGNAL(sendPortsReady()), dmmCtr, SLOT(setReady()));
    connect(dmmCtr, SIGNAL(sendSetTimeout()), this, SLOT(setTimeout()));
    connect(dmmCtr, SIGNAL(sendSetError()), this, SLOT(setError()));
    connect(dmmCtr, SIGNAL(sendEnable()), portCtr, SLOT(enable()));
    connect(dmmCtr, SIGNAL(sendResults(QStringList)),
            this, SLOT(setResults(QStringList)));
    connect(dmmCtr, SIGNAL(sendStarted()), this, SLOT(ctrStarted()));
    connect(dmmCtr, SIGNAL(sendStopped()), this, SLOT(ctrStopped()));
    dmmCtr->start();
}

void MainWindow::initMenu()
{
    QMenu *fileMenu, *optionsMenu, *helpMenu;
    QAction *quit = new QAction(this);
    QAction *about = new QAction(this);
    QAction *aboutQt = new QAction(this);

    exportCsv = new QAction(this);
    exportXls = new QAction(this);

    fileMenu    = menuBar()->addMenu(tr("&File"));
    optionsMenu = menuBar()->addMenu(tr("&Options"));
    helpMenu    = menuBar()->addMenu(tr("&Help"));

    quit->setText(tr("&Quit"));
    quit->setShortcut(tr("CTRL+Q"));
    fileMenu->addAction(quit);
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));

    exportCsv->setText(tr("Export to &CSV"));
    exportCsv->setCheckable(true);
    optionsMenu->addAction(exportCsv);
    connect(exportCsv, SIGNAL(triggered()), this, SLOT(selectExportCsv()));
#ifdef Q_OS_WIN
    exportXls->setText(tr("Export to &Excel"));
    exportXls->setCheckable(true);
    optionsMenu->addAction(exportXls);
    connect(exportXls, SIGNAL(triggered()), this, SLOT(selectExportXls()));
    selectExportXls();
#else
    selectExportCsv();
#endif

    about->setText(tr("&About").append(" " PROG_NAME));
    helpMenu->addAction(about);
    connect(about, SIGNAL(triggered()), this, SLOT(displayAbout()));

    aboutQt->setText(tr("About &Qt"));
    helpMenu->addAction(aboutQt);
    connect(aboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::initLayout()
{
    Qt::WindowFlags flags;
    flags = Qt::Window | Qt::WindowMinimizeButtonHint |
            Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    setMaximumSize(sizeHint());

    portLayout->addWidget(portLabel);
    portLayout->addWidget(portComboBox);
    portLayout->addWidget(baudLabel);
    portLayout->addWidget(baudComboBox);
    portLayout->addWidget(flowLabel);
    portLayout->addWidget(flowComboBox);
    portLayout->addWidget(parityLabel);
    portLayout->addWidget(parityComboBox);
    portLayout->addWidget(dataBitsLabel);
    portLayout->addWidget(dataBitsComboBox);
    portLayout->addWidget(stopBitsLabel);
    portLayout->addWidget(stopBitsComboBox);
    portLayout->addStretch(400);
    portGroup->setLayout(portLayout);

    progCtrLayout->addWidget(startButton);
    progCtrLayout->addWidget(timeoutIndicat);
    progCtrLayout->addWidget(errorIndicat);
    progCtrLayout->setAlignment(Qt::AlignTop);
#ifdef Q_OS_WIN
    progCtrLayout->setContentsMargins(QMargins(15,0,15,0));
#endif
    progCtrLayout->setSpacing(5);
    progCtrFrame->setLayout(progCtrLayout);
    progCtrFrame->setFrameStyle(1);

    measLayout->addWidget(measFunctLbl, 0, 0, 1, 2, Qt::AlignBottom);
    measLayout->addWidget(measFunct, 1, 0, Qt::AlignTop);
    measLayout->addWidget(measIntegrTimeLbl, 2, 0, Qt::AlignBottom);
    measLayout->addWidget(measIntegrTime, 3, 0, Qt::AlignTop);
    measLayout->addWidget(measAutoZeroLbl, 4, 0, Qt::AlignBottom);
    measLayout->addWidget(measAutoZero, 5, 0, Qt::AlignTop);
    measLayout->addWidget(measRangeLbl, 0, 1, 1, 2, Qt::AlignBottom);
    measLayout->addWidget(measRange, 1, 1, Qt::AlignTop);
    measLayout->addWidget(measRateLbl, 2, 1, Qt::AlignBottom);
    measLayout->addWidget(measRate, 3, 1, Qt::AlignTop);
    measWidget->setLayout(measLayout);
    dmmTabWidget->addTab(measWidget, tr("Measurement"));

    trigLayout->addWidget(trigSourceLbl, 0, 0, 1, 2, Qt::AlignBottom);
    trigLayout->addWidget(trigSource, 1, 0, Qt::AlignTop);
    trigLayout->addWidget(trigCountLbl, 2, 0, Qt::AlignBottom);
    trigLayout->addWidget(trigCount, 3, 0, Qt::AlignTop);
    trigLayout->addWidget(trigSamplesLbl, 4, 0, Qt::AlignBottom);
    trigLayout->addWidget(trigSamples, 5, 0, Qt::AlignTop);
    trigLayout->addWidget(genDispLbl, 0, 1, 1, 2, Qt::AlignBottom);
    trigLayout->addWidget(genDisp, 1, 1, Qt::AlignTop);
    trigWidget->setLayout(trigLayout);
    dmmTabWidget->addTab(trigWidget, tr("Trigger/Samples"));

    exportLayout->addWidget(exportLineEdit);
    exportLayout->addWidget(exportSelButton);
    exportLayout->addWidget(exportClrButton);
    exportLayout->addStretch(400);
    exportLayout->setContentsMargins(QMargins(0,0,0,0));
    exportWidget->setLayout(exportLayout);

    centerLayout->addWidget(progCtrFrame, 0, Qt::AlignLeft);
    centerLayout->addWidget(dmmTabWidget, 0);
    centerLayout->addWidget(exportLbl, 0, Qt::AlignBottom);
    centerLayout->addWidget(exportWidget, 0, Qt::AlignTop);
    centerLayout->addStretch(400);
    centerWidget->setLayout(centerLayout);

    mainLayout->addWidget(portGroup, 0, Qt::AlignLeft);
    mainLayout->addWidget(centerWidget, 0, Qt::AlignLeft);
    mainLayout->addStretch(400);
    mainWidget->setLayout(mainLayout);
}

void MainWindow::initControls()
{
    QFont progCtrFont;
    QIcon   *iconSelect;
    QIcon   *iconClear;
    QPixmap *pixmapSelect;
    QPixmap *pixmapClear;

    initPortControls();
    startButton->setText(tr("Start"));
    startButton->setFixedHeight(40);
#ifdef Q_OS_WIN
    startButton->setFixedWidth(80);
#endif
    progCtrFont = startButton->font();
    progCtrFont.setFamily("Nimbus Sans L,Arial,Helvetica,Geneva,sans-serif");
    progCtrFont.setPointSize(13);
    progCtrFont.setBold(true);
    startButton->setFont(progCtrFont);

    stopButton->setText(tr("Stop"));
#ifdef Q_OS_WIN
    stopButton->setFixedSize(startButton->size());
#else
    stopButton->setFixedHeight(startButton->height());
#endif
    stopButton->setFont(progCtrFont);
    stopButton->setStyleSheet("color: red");

    sets->initComboBox(FUNCT_ID, measFunctLbl, measFunct);

    sets->initComboBox(INTEGR_ID, measIntegrTimeLbl, measIntegrTime);

    sets->initComboBox(AUTOZ_ID, measAutoZeroLbl, measAutoZero);

    sets->initComboBox(RANGE_ID, measRangeLbl, measRange);

    sets->initComboBox(MEAS_RATE_ID, measRateLbl, measRate);

    // Range depends on rate
    connect(measRate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRange(int)));
    updateRange(measRate->currentIndex());

    sets->initComboBox(TRIG_SRC_ID, trigSourceLbl, trigSource);

    sets->initSpinBox(TRIG_CNT_ID, trigCountLbl, trigCount);

    sets->initSpinBox(SAMP_ID, trigSamplesLbl, trigSamples);

    sets->initComboBox(DISP_ID, genDispLbl, genDisp);

    pixmapSelect = new QPixmap(":/app/images/folder2.svg", "SVG");
    pixmapClear = new QPixmap(":/app/images/eraser.svg", "SVG");
    *pixmapSelect  = pixmapSelect->scaled(ICON_DEFAULT_SIZE, ICON_DEFAULT_SIZE);
    *pixmapClear   = pixmapClear->scaled(ICON_DEFAULT_SIZE, ICON_DEFAULT_SIZE);
    iconSelect = new QIcon(*pixmapSelect);
    iconClear  = new QIcon(*pixmapClear);
    exportSelButton->setIcon(*iconSelect);
    exportSelButton->setIconSize(QSize(ICON_DEFAULT_SIZE, ICON_DEFAULT_SIZE));
    exportSelButton->setToolTip(tr("Select"));
    exportSelButton->setContentsMargins(0,0,0,0);
    exportSelButton->setFixedSize(ICON_DEFAULT_SIZE, ICON_DEFAULT_SIZE);
    exportClrButton->setIcon(*iconClear);
    exportClrButton->setIconSize(QSize(ICON_DEFAULT_SIZE, ICON_DEFAULT_SIZE));
    exportClrButton->setToolTip(tr("Clear"));
    exportClrButton->setContentsMargins(0,0,0,0);
    exportClrButton->setFixedSize(ICON_DEFAULT_SIZE, ICON_DEFAULT_SIZE);
    exportLineEdit->setFixedWidth(
                progCtrLayout->sizeHint().width() -
                exportSelButton->width() -
                exportClrButton->width() -
                exportLayout->spacing() * 2);

    stopButton->setEnabled(false);
    stopButton->setVisible(false);
    connect(startButton, SIGNAL(released()), this, SLOT(start()));
    connect(stopButton, SIGNAL(released()), this, SLOT(stop()));
    connect(exportClrButton, SIGNAL(released()), this, SLOT(clearExportText()));
}

void MainWindow::initPortControls()
{
    portGroup->setTitle(tr("Serial Port"));
    this->setStyleSheet("QGroupBox { font: bold }");

    sets->initLabel(PORT_ID, portLabel);
    portLabel->setAlignment(Qt::AlignBottom);

    sets->initComboBox(BAUD_ID, baudLabel, baudComboBox);
    baudLabel->setAlignment(Qt::AlignBottom);

    sets->initComboBox(FLOW_ID, flowLabel, flowComboBox);
    flowLabel->setAlignment(Qt::AlignBottom);

    sets->initComboBox(PARITY_ID, parityLabel, parityComboBox);
    parityLabel->setAlignment(Qt::AlignBottom);

    sets->initComboBox(DATA_BITS_ID, dataBitsLabel, dataBitsComboBox);
    dataBitsLabel->setAlignment(Qt::AlignBottom);

    sets->initComboBox(STOP_BITS_ID, stopBitsLabel, stopBitsComboBox);
    stopBitsLabel->setAlignment(Qt::AlignBottom);
}


// Slots

void MainWindow::initPorts(QStringList portNames)
{
    qDebug() << "MainWindow::initPorts() received";
    sets->initComboBox(PORT_ID, portNames, portComboBox);
}

void MainWindow::reinitPorts(QStringList portNames)
{
    qDebug() << "MainWindow::reinitPorts() received";
    int i = portComboBox->currentIndex();

    portComboBox->clear();
    portComboBox->addItems(portNames);
    if (i >= portNames.size())
        i = portNames.size() - 1;
    portComboBox->setCurrentIndex(i);
}

void MainWindow::setTimeout()
{
    timeoutIndicat->setOn();
    stop();
}

void MainWindow::clearTimeout()
{
    timeoutIndicat->setOff();
}

void MainWindow::setError()
{
    errorIndicat->setOn();
    stop();
}

void MainWindow::clearError()
{
    errorIndicat->setOff();
}

void MainWindow::portsDetected()
{
    qDebug() << "MainWindow::portsDetected() received";
    if (!started) {
        startButton->setEnabled(true);
    }
}

void MainWindow::setResults(QStringList strList)
{
    Report report;

    if (sets->getCfgID(EXPORT_ID) == EXPORT_CSV) {
        report.writeToCsv(strList, exportLineEdit->text());
        sets->setCfgStr(CSV_PATH_STR, exportLineEdit->text());
    }
#ifdef Q_OS_WIN
    else {
        report.exportToExcel(strList, exportLineEdit->text());
        sets->setCfgStr(XLS_PATH_STR, exportLineEdit->text());
    }
#endif

    ConfigXml cfgXml(sets);
    cfgXml.writeConfigFile(CFG_FILE_DEFAULT);
}

void MainWindow::ctrStarted()
{
    qDebug() << "MainWindow::ctrStarted() received";
    startButton->setVisible(false);
    stopButton->setVisible(true);
    stopButton->setEnabled(true);
    progCtrLayout->removeWidget(startButton);
    progCtrLayout->insertWidget(0, stopButton);
}

void MainWindow::ctrStopped()
{
    qDebug() << "MainWindow::ctrStopped() received";
    stopButton->setVisible(false);
    startButton->setEnabled(false);
    startButton->setVisible(true);
    progCtrLayout->removeWidget(stopButton);
    progCtrLayout->insertWidget(0, startButton);
    started = false;
}


// Private slots

// Menu action, also called at app start on non-Win32
void MainWindow::selectExportCsv()
{
    exportCsv->setChecked(true);
    exportXls->setChecked(false);
    sets->setCfgID(EXPORT_ID, EXPORT_CSV);
    exportLbl->setText(tr("Application to open CSV"));
    disconnect(exportSelButton, SIGNAL(released()), 0, 0);
    connect(exportSelButton, SIGNAL(released()), this, SLOT(selectCsvApp()));
    exportLineEdit->setEnabled(true);
    exportLineEdit->setText(sets->getCfgStr(CSV_PATH_STR));
}

// Menu action, also called at app start on Win32
void MainWindow::selectExportXls()
{
    exportCsv->setChecked(false);
    exportXls->setChecked(true);
    sets->setCfgID(EXPORT_ID, EXPORT_XLS);
    exportLbl->setText(tr("Excel Template"));
    disconnect(exportSelButton, SIGNAL(released()), 0, 0);
    connect(exportSelButton, SIGNAL(released()), this, SLOT(selectXls()));
    exportLineEdit->setEnabled(false);
    exportLineEdit->setText(sets->getCfgStr(XLS_PATH_STR));
}

// Menu action
void MainWindow::displayAbout()
{
    QMessageBox::about(this, tr("About").append(" " PROG_NAME),
           QString("<h2>" PROG_NAME " " PROG_VERSION "</h2>").append(
           tr("<p>Copyright &copy; 2011-2014 Sebastian Riemer "
              "&lt;sebastian.riemer@gmx.de&gt;. All rights reserved."
              "<p>License: GNU General Public License Version 3"
              "<p>This program is distributed in the hope that it "
              "will be useful, but WITHOUT ANY WARRANTY; without "
              "even the implied warranty of MERCHANTABILITY or "
              "FITNESS FOR A PARTICULAR PURPOSE. See the GNU "
              "General Public License for more details."
              "<p>").append(PROG_NAME).append(
           tr(" is a small application that "
              "requests DC voltage values from a Tektronix "
              "4050 DMM, calculates the sample interval and "
              "exports this data to CSV or Microsoft Excel in "
              "an NI LabVIEW SignalExpress compatible format."))));
}

// Button event
void MainWindow::start()
{
    qDebug() << "MainWindow::start() received";
    if (portComboBox->currentText().isEmpty() || started)
        return;

    started = true;
    startButton->setEnabled(false);
    portGroup->setEnabled(false);
    dmmTabWidget->setEnabled(false);

    sets->setCfgID(PORT_ID,   portComboBox->currentIndex());
    sets->setCfgID(BAUD_ID,   baudComboBox->currentIndex());
    sets->setCfgID(FLOW_ID,   flowComboBox->currentIndex());
    sets->setCfgID(PARITY_ID, parityComboBox->currentIndex());
    sets->setCfgID(DATA_BITS_ID, dataBitsComboBox->currentIndex());
    sets->setCfgID(STOP_BITS_ID, stopBitsComboBox->currentIndex());
    sets->setCfgID(FUNCT_ID,  measFunct->currentIndex());
    sets->setCfgID(INTEGR_ID, measIntegrTime->currentIndex());
    sets->setCfgID(AUTOZ_ID,  measAutoZero->currentIndex());
    sets->setCfgID(MEAS_RATE_ID, measRate->currentIndex());
    sets->setCfgID(TRIG_CNT_ID,  trigCount->value());
    sets->setCfgID(SAMP_ID,  trigSamples->value());
    sets->setCfgID(DISP_ID,  genDisp->currentIndex());

    timeoutIndicat->setOff();
    errorIndicat->setOff();
    portCtr->disable();
}

// Button event
void MainWindow::stop()
{
    qDebug() << "MainWindow::stop() received";
    if (!started)
        return;

    stopButton->setEnabled(false);
    portGroup->setEnabled(true);
    dmmTabWidget->setEnabled(true);
    dmmCtr->requestStop();
}

// Button event
void MainWindow::selectCsvApp()
{
    QString path;
    QString caption = tr("Choose application to open CSV");
#ifdef Q_OS_WIN
    QString filter = "*.exe";
#else
    QString filter = "";
#endif
    int idx;

    path = QFileDialog::getOpenFileName(this, caption, sets->getCfgStr(CSV_DIR_STR),
                                        filter);

    if (path.size() > 1) {
        exportLineEdit->setText(path);
        idx = path.lastIndexOf("/");
        if (idx >= 0)
            path.truncate(idx);
        sets->setCfgStr(CSV_DIR_STR, path);
    }
}

// Button event
void MainWindow::selectXls()
{
    QString path;
    QString caption = tr("Choose template file to open");
    int idx;

    path = QFileDialog::getOpenFileName(this, caption, sets->getCfgStr(XLS_DIR_STR),
                                        "*.xls *.xlsx");
    if (path.size() > 1) {
        exportLineEdit->setText(path);
        idx = path.lastIndexOf("/");
        if (idx >= 0)
            path.truncate(idx);
        sets->setCfgStr(XLS_DIR_STR, path);
    }
}

// Button event
void MainWindow::clearExportText()
{
    exportLineEdit->clear();
}

// DMM ComboBox event
void MainWindow::updateRange(int rateIdx)
{
    int rangeSetID = sets->getCfgID2(RANGE_ID);
    int newRangeSetID = 0;

    if (rateIdx != 0)  // not slow ?
        newRangeSetID = 1;

    if (rangeSetID == newRangeSetID)
        return;

    sets->setCfgID2(RANGE_ID, newRangeSetID);
    sets->setCfgID(RANGE_ID, measRange->currentIndex());
    measRange->clear();
    sets->initComboBox(RANGE_ID, measRange);
}
