/**********************************************************************************************
   Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/


#include "setup/IAppSetup.h"
#include "setup/CLogHandler.h"
#include "CCommandProcessor.h"

#include "setup/CAppSetupMac.h"
#include "setup/CAppSetupLinux.h"
#include "setup/CAppSetupWin.h"


#include <gdal.h>


IAppSetup* instance = nullptr;

IAppSetup* IAppSetup::getPlattformInstance()
{
    if(nullptr == instance)
    {
#ifdef Q_OS_MAC
        instance = new CAppSetupMac();
#endif
#ifdef Q_OS_LINUX
        instance = new CAppSetupLinux();
#endif
#ifdef Q_OS_WIN32
        instance = new CAppSetupWin();
#endif
    }
    return instance;
}


void IAppSetup::prepareGdal(QString gdalDir, QString projDir)
{
    qputenv("GDAL_DATA", gdalDir.toUtf8());
    qputenv("PROJ_LIB", projDir.toUtf8());

    qDebug() << "GDAL_DATA directory set to " + gdalDir;
    qDebug() << "PROJ_LIB directory set to " + projDir;
    GDALAllRegister();
}


QString IAppSetup::path(QString path, QString subdir, bool mkdir, QString debugName)
{
    QDir pathDir(path);

    if(subdir != 0)
    {
        pathDir = QDir(pathDir.absoluteFilePath(subdir));
    }
    if(mkdir && !pathDir.exists())
    {
        pathDir.mkpath(pathDir.absolutePath());
        qDebug() << debugName << "path created" << pathDir.absolutePath();
    }
    else if (debugName != 0)
    {
        qDebug() << debugName << "path" << pathDir.absolutePath();
    }
    return pathDir.absolutePath();
}


void IAppSetup::prepareTranslator(QString translationPath, QString translationPrefix)
{
    QString locale = QLocale::system().name();
    QDir dir(translationPath);
    if(!QFile::exists(dir.absoluteFilePath(translationPrefix + locale)))
    {
        locale = locale.left(2);
    }
    qDebug() << "locale" << locale;

    QApplication* app =  (QApplication*) QCoreApplication::instance();
    QTranslator *qtTranslator = new QTranslator(app);
    if (qtTranslator->load(translationPrefix + locale, translationPath))
    {
        app->installTranslator(qtTranslator);
        qDebug() << "using file '"+ translationPath + "/" + translationPrefix + locale + ".qm' for translations.";
    }
    else
    {
        qWarning() << "no file found for translations '"+ translationPath + "/" + translationPrefix + locale + "' (using default).";
    }
}


void IAppSetup::initLogHandler()
{
    CLogHandler::initLogHandler(logDir(), qlOpts->logfile, qlOpts->debug);
}


CAppOpts* IAppSetup::processArguments()
{
    CCommandProcessor cmdParse;
    return cmdParse.processOptions(QCoreApplication::instance()->arguments());
}