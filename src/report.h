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

#ifndef REPORT_H
#define REPORT_H

#include <QStringList>

class Report
{
public:
    Report();
    void writeToCsv(QStringList results, const QString csvApp);
#ifdef Q_OS_WIN
    void exportToExcel(QStringList results, QString templatePath);
#endif
private:
};

#endif
