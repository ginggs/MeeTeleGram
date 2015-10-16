/*****************************************************************
*  Copyright (C) 2012  Marco Bavagnoli - lil.deimos@gmail.com    *
******************************************************************/
/*
 *  Prayers, Islamic prayer time application
 *  Copyright (C) 2013  Hedayat Vatankhah <gmail email: hedayatv>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "setting.h"

#include <QDesktopServices>
#include <QCoreApplication>
#include <QDebug>


Settings::Settings(QString appName, QString fileName, QObject *parent) : QObject(parent)
{
    // Initialize the settings path
    m_confFile = QDesktopServices::storageLocation( QDesktopServices::DataLocation ) + "/" + appName + "/" + fileName;
}

QString Settings::filePath() const
{
    return m_confFile;
}

void Settings::setFilePath(const QString &data)
{
    m_confFile.clear();
    m_confFile.append(data);
}

void Settings::setValue(const QString & key, const QVariant & value)
{
    QSettings settings(m_confFile, QSettings::IniFormat);

    settings.setValue(key, value);
}

QVariant Settings::getValue( const QString & key, const QVariant & defaultValue) const
{
    QSettings settings(m_confFile, QSettings::IniFormat);

    return settings.value(key, defaultValue);
}

// first index=0
void Settings::removeArrayEntry( const QString & key , int index)
{
    QVariantList array;
    array = getArray(key);
    array.removeAt(index);

    removeArray(key);

    foreach (QVariant entry, array)
        appendToArray(key, entry.toMap());
}

void Settings::removeArray( const QString & key )
{
    QSettings settings(m_confFile, QSettings::IniFormat);

    settings.remove(key);
}

void Settings::setArrayValue( const QString & key, int index,
                                const QMap<QString, QVariant> &values)
{
    QSettings settings(m_confFile, QSettings::IniFormat);
    QMapIterator<QString, QVariant> i(values);

    settings.beginWriteArray(key);
    settings.setArrayIndex(index);
    while (i.hasNext()) {
         i.next();
         settings.setValue(i.key(), i.value() );
     }
     settings.endArray();
}

void Settings::appendToArray( const QString & key, const QMap<QString, QVariant> &values)
{
    QSettings settings(m_confFile, QSettings::IniFormat);

    // get the current size of this array
    int size = settings.beginReadArray(key);
    settings.endArray();

    setArrayValue(key, size, values);
}


QString Settings::getArrayJson( const QString & key)
{
    QSettings settings(m_confFile, QSettings::IniFormat);
    QString list;

    list = "[ { ";
    int size = settings.beginReadArray(key);

     for (int i = 0; i < size; ++i) {
         settings.setArrayIndex(i);
         QVariantMap entry;
         QStringList keys;
         keys = settings.allKeys();
         foreach (QString key, keys) {
             entry.insert( key, settings.value(key) );
             list += "\""+key+"\" : \""+settings.value(key).toString()+"\", ";
         }

         list.remove( list.length()-2, 2 );
         list += "},{";
     }
     list.remove( list.length()-2, 2 );
     list += " ]";
     list = list.replace("\n","<br>").replace("\r","").replace("\t"," ");
//     qDebug() << "LIST: " << list;

     settings.endArray();
     return list;
}

QString Settings::getArrayXml( const QString & key)
{
    QSettings settings(m_confFile, QSettings::IniFormat);
    QString list;

    int size = settings.beginReadArray(key);

    list = "<xml>";
     for (int i = 0; i < size; ++i) {
         list += "<item>";
         settings.setArrayIndex(i);
         QVariantMap entry;
         QStringList keys;
         keys = settings.allKeys();
         foreach (QString key, keys) {
             entry.insert( key, settings.value(key) );
             list += "<"+key+">"+settings.value(key).toString()+"</"+key+">";
         }
         list += "</item>";
     }
     list = list.replace("\n"," ").replace("\r"," ").replace("\t"," ").replace("<br>"," ");
     list += "</xml>";
//     qDebug() << "LIST: " << list;

     settings.endArray();
     return list;
}

bool Settings::checkValueArray( const QString & key, const QString & arrayKey , const QString & value )
{
    QSettings settings(m_confFile, QSettings::IniFormat);

    int size = settings.beginReadArray(key);

     for (int i = 0; i < size; ++i) {
         settings.setArrayIndex(i);
         if ( settings.value(arrayKey).toString().compare(value) == 0 ) return true;
     }

     return false;
}

int Settings::getIndexOfValueInArray( const QString & key, const QString & arrayKey , const QString & value )
{
    QSettings settings(m_confFile, QSettings::IniFormat);

    int size = settings.beginReadArray(key);

     for (int i = 0; i < size; ++i) {
         settings.setArrayIndex(i);
         if ( settings.value(arrayKey).toString().compare(value) == 0 ) return i;
     }
     return -1;
}

QVariantList Settings::getArray(const QString &key)
{
    QSettings settings(m_confFile, QSettings::IniFormat);
    QVariantList list;

    int size = settings.beginReadArray(key);

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QVariantMap entry;
        QStringList keys;
        keys = settings.allKeys();
        foreach (QString key, keys) {
            entry.insert( key, settings.value(key) );
        }
        list.append(entry);
     }
     settings.endArray();

     return list;
}

bool locatoinLessThan(const QVariant &s1, const QVariant &s2)
{
    QVariantMap s1map = s1.toMap();
    QVariantMap s2map = s2.toMap();
    return s1map["title"].toString() < s2map["title"].toString();
}

void Settings::sortArray(const QString &key)
{
    QVariantList array = getArray(key);

    qSort(array.begin(), array.end(), locatoinLessThan);

    removeArray(key);

    foreach (QVariant entry, array)
        appendToArray(key, entry.toMap());
}

// https://bugreports.qt-project.org/browse/QTBUG-71
int Settings::getTimeZone()
{
    // determine how far off of UTC we are
    QDateTime now = QDateTime::currentDateTime();

    // check if we need to account for UTC being tomorrow relative to us
    int dayOffset = 0;
    if( now.date() < now.toUTC().date() )
    {
        dayOffset = 24;
    }

    int timezone = now.time().hour() - ( now.toUTC().time().hour() + dayOffset );
//    qDebug() << "TIMEZONE" << timezone;
    return timezone;
}


// convert a string with format "2012-02-06T06:00:00" ( the java Date() )to QDateTime
QDateTime Settings::stringToDate(QString s)
{
    QDateTime t;

    t = QDateTime::fromString(s, "yyyy-MM-ddThh:mm:ss");

    return t;
}
