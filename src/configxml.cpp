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

ConfigXml::ConfigXml(Config *cfg)
{
    config = cfg;
}

void ConfigXml::readConfigFile(const QString fileName)
{
    QFile configFile(fileName);
    int intIdx = 0;
    int strIdx = 0;

    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Config file not found!";
    }

    reader.setDevice(&configFile);

    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isStartElement()) {
            if (reader.name() == "config") {
                readConfigElement(&intIdx, &strIdx);
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

void ConfigXml::readConfigElement(int *intIdx, int *strIdx)
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == "entry") {
                readEntryElement(intIdx, strIdx);
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void ConfigXml::readEntryElement(int *intIdx, int *strIdx)
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
                readEntryElement(intIdx, strIdx);
            } else if (reader.name() == "value") {
                readValue(type, intIdx, strIdx);
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void ConfigXml::readValue(const QString type, int *intIdx, int *strIdx)
{
    QString value = reader.readElementText();

    if (reader.isEndElement())
        reader.readNext();

    if (type == "int") {
        config->setID((ConfigIDType) *intIdx, value.toInt());
        (*intIdx)++;
    } else if (type == "str") {
        config->setStr((ConfigStrType) *strIdx, value);
        (*strIdx)++;
    }
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
    int i = 0;
    QString tmpStr;
    QStringList valueNames = QStringList()     << QString("port") <<
        QString("baud")   << QString("flow")   << QString("parity") <<
        QString("data_b") << QString("stop_b") << QString("funct") <<
        QString("integ")  << QString("autoz")  << QString("trigs") <<
        QString("sampl")  << QString("trigr")  << QString("expor");

    writer->writeStartElement("entry");
    writer->writeAttribute("type", "int");
    for (i = 0; i < CFG_ID_SIZE; i++) {
        tmpStr = QString("%1").arg(config->getID((ConfigIDType) i));
        writer->writeStartElement("value");
        writer->writeAttribute("name", valueNames.at(i));
        writer->writeCharacters(tmpStr);
        writer->writeEndElement();
    }
    writer->writeEndElement();

    valueNames = QStringList() << QString("csv_app_dir") <<
        QString("csv_app_path") << QString("xls_dir") << QString("xls_path");

    writer->writeStartElement("entry");
    writer->writeAttribute("type", "str");
    for (i = 0; i < CFG_STR_SIZE; i++) {
        tmpStr = config->getStr((ConfigStrType) i);
        writer->writeStartElement("value");
        writer->writeAttribute("name", valueNames.at(i));
        writer->writeCharacters(tmpStr);
        writer->writeEndElement();
    }
    writer->writeEndElement();
}
