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
#include "../settings.h"
#include "../serialportctr.h"
#include "dmmcontrol.h"

#ifdef PROG_NAME
    #undef PROG_NAME
#endif
#define PROG_NAME "DMM_send_values"
#ifdef PROG_VERSION
    #undef PROG_VERSION
#endif
#define PROG_VERSION "1.0.0"

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
    commandLabel       = new QLabel;
    answerLabel        = new QLabel;
    commandDisplay     = new QTextBrowser;
    answerDisplay      = new QTextBrowser;

    sets     = new Settings();
    started  = false;

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
    connect(dmmCtr, SIGNAL(sendClearTimeout()), this, SLOT(clearTimeout()));
    connect(dmmCtr, SIGNAL(sendSetError()), this, SLOT(setError()));
    connect(dmmCtr, SIGNAL(sendEnable()), portCtr, SLOT(enable()));
    connect(dmmCtr, SIGNAL(sendSetCommand(QString)),
            this, SLOT(setCommand(QString)));
    connect(dmmCtr, SIGNAL(sendSetAnswer(QString)),
            this, SLOT(setAnswer(QString)));
    connect(dmmCtr, SIGNAL(sendStarted()), this, SLOT(ctrStarted()));
    connect(dmmCtr, SIGNAL(sendStopped()), this, SLOT(ctrStopped()));
    dmmCtr->start();
}

void MainWindow::initMenu()
{
    QMenu *fileMenu, *helpMenu;
    QAction *quit = new QAction(this);
    QAction *about = new QAction(this);
    QAction *aboutQt = new QAction(this);

    fileMenu     = menuBar()->addMenu(tr("&File"));
    helpMenu     = menuBar()->addMenu(tr("&Help"));

    quit->setText(tr("&Quit"));
    quit->setShortcut(tr("CTRL+Q"));
    fileMenu->addAction(quit);
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));

    about->setText(tr("&About"));
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

    centerLayout->addWidget(progCtrFrame, 0, Qt::AlignLeft);
    centerLayout->addWidget(commandLabel, 0);
    centerLayout->addWidget(commandDisplay, 0);
    centerLayout->addWidget(answerLabel, 0);
    centerLayout->addWidget(answerDisplay, 0);
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

    stopButton->setEnabled(false);
    stopButton->setVisible(false);
    connect(startButton, SIGNAL(released()), this, SLOT(start()));
    connect(stopButton, SIGNAL(released()), this, SLOT(stop()));

    commandLabel->setText(tr("Command"));
    answerLabel->setText(tr("Answer"));
    commandDisplay->setMaximumSize(progCtrFrame->sizeHint().width(), 90);
    answerDisplay->setMaximumSize(progCtrFrame->sizeHint().width(), 90);
}

void MainWindow::initPortControls()
{
    portGroup->setTitle(tr("Serial Port"));
    this->setStyleSheet("QGroupBox { font: bold }");

    portLabel->setText(tr("DMM Port"));
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
    portComboBox->addItems(portNames);
    portComboBox->setCurrentIndex(sets->getCfgID(PORT_ID));
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
    emit timeoutIndicat->setOn();
}

void MainWindow::clearTimeout()
{
    emit timeoutIndicat->setOff();
}

void MainWindow::setError()
{
    emit errorIndicat->setOn();
    stop();
}

void MainWindow::clearError()
{
    emit errorIndicat->setOff();
}

void MainWindow::portsDetected()
{
    qDebug() << "MainWindow::portsDetected() received";
    if (!started) {
        startButton->setEnabled(true);
        portGroup->setEnabled(true);
    }
}

void MainWindow::setCommand(QString cmd)
{
    commandDisplay->setText(cmd);
}

void MainWindow::setAnswer(QString answ)
{
    answerDisplay->setText(answ);
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
           tr(" is a small application that sends random DC voltage "
              "values to simulate a Tektronix 4050 DMM. This is "
              "intended for developers who have no or only limited "
              "access to such a DMM, but have instead two serial "
              "ports and a null modem cable."))));
}

// Button action
void MainWindow::start()
{
    qDebug() << "MainWindow::start() received";
    if (portComboBox->currentText().isEmpty() || started)
        return;

    started = true;
    startButton->setEnabled(false);
    portGroup->setEnabled(false);
    commandDisplay->clear();
    answerDisplay->clear();

    // Port config
    sets->setCfgID(PORT_ID,   portComboBox->currentIndex());
    sets->setCfgID(BAUD_ID,   baudComboBox->currentIndex());
    sets->setCfgID(FLOW_ID,   flowComboBox->currentIndex());
    sets->setCfgID(PARITY_ID, parityComboBox->currentIndex());
    sets->setCfgID(DATA_BITS_ID, dataBitsComboBox->currentIndex());
    sets->setCfgID(STOP_BITS_ID, stopBitsComboBox->currentIndex());

    emit timeoutIndicat->setOff();
    emit errorIndicat->setOff();
    emit portCtr->disable();
}

// Button action
void MainWindow::stop()
{
    qDebug() << "MainWindow::stop() received";
    if (!started)
        return;

    stopButton->setEnabled(false);
    portGroup->setEnabled(true);
    emit dmmCtr->requestStop();
}
