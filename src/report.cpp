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
#ifdef Q_OS_WIN
    #include <QAxObject>
#endif
#include "report.h"

#define NUM_EXTRA_VALS 3
#define OUTPUT_CSV "out.csv"

Report::Report()
{

}

void Report::writeToCsv(QStringList results, const QString csvApp = "")
{
    int     ret;
    QFile   outputFile;
    int     samples  = (results.size() - NUM_EXTRA_VALS) * results.at(2).toInt();
    int     valStart;
    int     valEnd;
    long long j = 0;
    QDate   date = QDate::currentDate();
    QString sep = ";";
    QString tmpStr;
    QStringList strList;
    double  tmpDbl;
    QString output;
    QLocale sysLocale = QLocale::system();

    if (samples < 3)
        return;

    output = QString("Channels%1%2").arg(sep).arg(1);
    output.append(QString("\nSamples%1%2").arg(sep).arg(samples));
    output.append(QString("\nDate%1%2").arg(sep)
                  .arg(date.toString("dd.MM.yyyy")));
    output.append(QString("\nTime%1%2").arg(sep).arg(results.at(1)));
    output.append(QString("\nY_Unit_Label %1").arg("Voltage (V)"));
    tmpStr = QString("%L1").arg(results.at(0).toDouble()
                             / (double)samples / 1000.0);
    output.append(QString("%1%1%1Interval [s]%1%2").arg(sep).arg(tmpStr));
    output.append(QString("\nX_Dimension %1").arg("Iteration"));
    output.append(QString("\nX0%1%L2").arg(sep).arg(0.0, 0, 'E', 2));
    output.append(QString("\nDelta_X%1%2").arg(sep).arg(1 * 1000000));
    output.append("\n***End_of_Header***");
    output.append(QString("\nX_Value%1%1Comment\n").arg(sep));

    valStart = NUM_EXTRA_VALS;
    valEnd   = results.size();

    for (long long i = valStart; i < valEnd; i++) {
        QStringList::Iterator it;
        tmpStr = results.at(i);
        strList = tmpStr.split(',');
        for (it = strList.begin(); it != strList.end(); ++it) {
            it->replace('.', sysLocale.decimalPoint());
            tmpDbl = it->toDouble();
            tmpStr = QString("%1").arg(tmpDbl);
            output.append(QString("%1%2%3\n")
                          .arg(j * 1000000)
                          .arg(sep)
                          .arg(tmpStr));
            j++;
        }
    }

    qDebug() << "Output: " << output;
    outputFile.setFileName(OUTPUT_CSV);
    outputFile.open(QIODevice::Truncate |
                    QIODevice::WriteOnly);
    outputFile.write(output.toAscii());
    outputFile.close();

    tmpStr = csvApp;
    tmpStr.append(" " OUTPUT_CSV);
    ret = system(tmpStr.toAscii());
    qDebug() << "system(" << tmpStr.toAscii() << ") returned " << ret << ".";
}

#ifdef Q_OS_WIN
void Report::exportToExcel(QStringList results, QString templatePath)
{
    int samples  = (results.size() - NUM_EXTRA_VALS) * results.at(2).toInt();
    int valStart, valEnd;
    long long i;
    QDate     date = QDate::currentDate();
    QLocale   sysLocale = QLocale::system();
    QAxObject *excel = new QAxObject();
    QAxObject *wb, *awb, *ws, *aws, *cell;
    QFileInfo fi(templatePath);
    QStringList rprt;
    QString   tmpStr;
    double    tmpDbl;

    if (samples < 3) {
        delete excel;
        return;
    }

    if (!excel->setControl("Excel.Application")) {
        delete excel;
        writeToCsv(results);
        return;
    }

    wb = excel->querySubObject("Workbooks");
    templatePath.replace(QString("/"), QString("\\"));
    if (templatePath.size() > 1 && QFile::exists(templatePath)) {
        wb->dynamicCall("open(QString)", templatePath);
        qDebug() << "Template path: " << templatePath;
        qDebug() << "Template file: " << fi.fileName();
        if (wb->property("Count").toInt() < 1) {
            delete excel;
            writeToCsv(results);
            return;
        }

        do {
            awb = excel->querySubObject("ActiveWorkbook");
            qDebug() << "Workbooks.Count: " << wb->property("Count").toInt();
            qDebug() << "ActiveWorkbook.Name: " << awb->property("Name").toString();
            if (awb->property("Name").toString() != fi.fileName()) {
                qDebug() << awb->property("Name").toString() << " is the wrong file! Closing it.";
                awb->dynamicCall("close");
            }
        } while (wb->property("Count").toInt() > 1);

        ws = excel->querySubObject("WorkSheets");
        ws->dynamicCall("Add()");
    } else {
        wb->dynamicCall("Add()");
        if (wb->property("Count").toInt() < 1) {
            delete excel;
            writeToCsv(results);
            return;
        }
        awb = excel->querySubObject("ActiveWorkbook");
        qDebug() << "Workbooks.Count: " << wb->property("Count").toInt();
        qDebug() << "ActiveWorkbook.Name: " << awb->property("Name").toString();

        ws = excel->querySubObject("WorkSheets");
        ws->dynamicCall("Add()");
        valEnd = ws->property("Count").toInt();
    }

    aws = excel->querySubObject("ActiveSheet");
    aws->setProperty("Name", "Meas. Data");

    rprt.append("Channels"); rprt.append("1");
    rprt.append("Samples"); rprt.append(QString("%1").arg(samples));
    rprt.append("Date");
    rprt.append(QString("%1").arg(date.toString("dd.MM.yyyy")));
    rprt.append("Time"); rprt.append(QString("%1").arg(results.at(1)));
    rprt.append(QString("Y_Unit_Label %1").arg("Voltage (V)"));
    rprt.append("");
    rprt.append(QString("X_Dimension %1").arg("Iteration"));
    rprt.append("");
    rprt.append("X0"); rprt.append(QString("%L1").arg(0.0, 0, 'E', 2));
    rprt.append("Delta_X"); rprt.append(QString("%L1").arg(1 * 1000000));
    rprt.append("***End_of_Header***"); rprt.append("");
    rprt.append("X_Value"); rprt.append("");

    valStart = NUM_EXTRA_VALS;
    valEnd   = results.size();

    for (i = valStart; i < valEnd; i++) {
        tmpStr = results.at(i);
        tmpStr.replace('.', sysLocale.decimalPoint());
        tmpDbl = tmpStr.toDouble();
        tmpStr = QString("%1").arg(tmpDbl);
        rprt.append(QString("%L1").arg((i - NUM_EXTRA_VALS) * 1000000));
        rprt.append(tmpStr);
    }

    for (i = 0; i < rprt.size(); i += 2) {
        cell = aws->querySubObject("Cells(int,int)", i / 2 + 1, 1);
        cell->setProperty("Value", rprt.at(i));
        cell = aws->querySubObject("Cells(int,int)", i / 2 + 1, 2);
        cell->setProperty("Value", rprt.at(i + 1));
    }

    tmpStr = QString("%L1").arg(results.at(0).toDouble()
                             / (double)samples / 1000.0);

    cell = aws->querySubObject("Cells(int,int)", 5, 4);  // D5
    cell->setProperty("Value", "Interval [s]");
    cell = aws->querySubObject("Cells(int,int)", 5, 5);  // E5
    cell->setProperty("Value", tmpStr);

    cell = aws->querySubObject("Cells(int,int)", 10, 3);  // C10
    cell->setProperty("Value", "Comment");

    excel->setProperty("Visible", true);

    delete excel;
}
#endif
