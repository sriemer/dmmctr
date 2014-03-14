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

#include <QtXml/QtXml>
#include <QFile>
#include <QDebug>
#include "configxml.h"

ConfigXml::ConfigXml(Settings *settings)
{
    sets = settings;
}

void ConfigXml::readConfigFile(const QString fileName)
{
    QFile configFile(fileName);

    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Config file not found!";
    }

    reader.setDevice(&configFile);

    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isStartElement()) {
            if (reader.name() == "config") {
                readConfigElement();
            } else {
                reader.raiseError(QObject::tr("Not a config file"));
            }
        } else {
            reader.readNext();
        }
    }
    configFile.close();
    if (reader.hasError()) {
        qDebug() << "Error: Failed to parse config file!";
    } else if (configFile.error() != QFile::NoError) {
        qDebug() << "Error: Can not read config file!";
    }
}

void ConfigXml::readConfigElement(void)
{
    int id = 0;

    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == "entry") {
                readEntryElement(&id);
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void ConfigXml::readEntryElement(int *id)
{
    QString type = reader.attributes().value("type").toString();

    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == "entry") {
                readEntryElement(id);
            } else if (reader.name() == "value") {
                readValue(id, type);
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void ConfigXml::readValue(int *id, const QString type)
{
    QString name = reader.attributes().value("name").toString();
    QString value = reader.readElementText();

    if (reader.isEndElement())
        reader.readNext();

    if (type == "int")
        *id = sets->setCfgID(*id, name, value.toInt());
    else if (type == "str")
        *id = sets->setCfgStr(*id, name, value);
}

void ConfigXml::skipUnknownElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            skipUnknownElement();
        } else {
            reader.readNext();
        }
    }
}

bool ConfigXml::writeConfigFile(const QString fileName)
{
    QXmlStreamWriter writer;
    QFile configFile(fileName);

    if (!configFile.open(QFile::Truncate | QFile::WriteOnly | QFile::Text)) {
        qDebug() << "Error: Cannot write file " << qPrintable(fileName) <<
                    ": " << qPrintable(configFile.errorString());
        return false;
    }

    writer.setDevice(&configFile);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("config");

    writeEntries(&writer);

    writer.writeEndDocument();
    configFile.close();
    if (configFile.error()) {
        qDebug() << "Error: Cannot write file " << qPrintable(fileName) <<
                    ": " << qPrintable(configFile.errorString());
        return false;
    }
    return true;
}

void ConfigXml::writeEntries(QXmlStreamWriter *writer)
{
    int i;
    SetIDType id;
    QString tmpStr;

    for (i = 0; i < SETTINGS_SIZE; i++) {
        id = (SetIDType) i;
        writer->writeStartElement("entry");
        switch (sets->getCfgType(id)) {
        case CFG_INT:
        case CFG_ID:
            writer->writeAttribute("type", "int");
            tmpStr = QString("%1").arg(sets->getCfgID(id));
            break;
        case CFG_STRING:
            writer->writeAttribute("type", "str");
            tmpStr = sets->getCfgStr(id);
            break;
        default:
            break;
        }
        writer->writeStartElement("value");
        writer->writeAttribute("name", sets->getName(id));
        writer->writeCharacters(tmpStr);
        writer->writeEndElement();
        writer->writeEndElement();
    }
}
