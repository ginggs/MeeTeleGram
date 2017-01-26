/*
 *  MeeTeleGram, an unofficial MeeGo Telegram client
 *  Copyright (C) 2015  Hedayat Vatankhah <gmail email: hedayatv>
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

#include <stdio.h>
#include <QtGui/QApplication>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QDebug>
#include <QtCore/QLibraryInfo>
#include <QtDeclarative>
#include <QDeclarativeContext>
#include <QDesktopServices>
#include "qmlapplicationviewer.h"
#include "setting.h"
#include "qtelegram.h"
#include "qpeerid.h"

#define APP_NAME "MeeTeleGram"
/*
 * Please note that each telegram client should have its own APP_HASH/APP_ID. So, if you
 * used this source in your own client/fork, please receive your own hash/id from telegram
 * website. Thanks!
 */
#define MEETELEGRAM_APP_HASH "923618563c7c9e07496c4aebb6924bfb"
#define MEETELEGRAM_APP_ID 44097
#define MEETELEGRAM_VERSION "0.0.0"


void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stdout, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QString stdout_log = QDesktopServices::storageLocation(
        QDesktopServices::HomeLocation) + "/MyDocs/meetelegram.log";
    QString stderr_log = QDesktopServices::storageLocation(
        QDesktopServices::HomeLocation) + "/MyDocs/meetelegram.log.stderr";
    freopen(stdout_log.toUtf8().constData(), "w", stdout);
    freopen(stderr_log.toUtf8().constData(), "w", stderr);
    qInstallMsgHandler(myMessageOutput);

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QLocale locale; // system locale

    QTranslator translator;
    if (translator.load("MeeTeleGram." + locale.name(), "l10n/"))
        app->installTranslator(&translator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + locale.name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app->installTranslator(&qtTranslator);
    app->setLayoutDirection(locale.textDirection());

    qmlRegisterType<QPeerId>();

    QmlApplicationViewer viewer;
    Settings settings("MeeTeleGram", "settings.ini");
    viewer.rootContext()->setContextProperty("settings", &settings);

    QString confdir = QDesktopServices::storageLocation(
        QDesktopServices::DataLocation) + "/" APP_NAME "/";

    qDebug() << "Config dir: " << confdir;
    QDir().mkpath(confdir);
    qtelegram qtlg(MEETELEGRAM_APP_ID,
                MEETELEGRAM_APP_HASH, APP_NAME " " MEETELEGRAM_VERSION,
                confdir.toUtf8().constData(),
                "/opt/MeeTeleGram/tg-server.pub");
    viewer.rootContext()->setContextProperty("telegram", &qtlg);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/MeeTeleGram/meetelegram.qml"));
    viewer.showExpanded();

    return app->exec();
}
