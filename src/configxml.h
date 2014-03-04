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

#ifndef CONFIGXML_H
#define CONFIGXML_H

#include <QtXml/QtXml>
#include <QString>
#include "config.h"

#define CFG_FILE_DEFAULT PROG_NAME ".xml"

class ConfigXml
{
public:
    ConfigXml(Config *cfg);
    void readConfigFile  (const QString fileName);
    bool writeConfigFile (const QString fileNames);

private:
    Config *config;
    QXmlStreamReader reader;

    void readConfigElement (int *intIdx, int *strIdx);
    void readEntryElement  (int *intIdx, int *strIdx);
    void readValue         (const QString type, int *intIdx, int *strIdx);
    void skipUnknownElement(void);
    void writeEntries      (QXmlStreamWriter *writer);
};

#endif // CONFIGXML_H
