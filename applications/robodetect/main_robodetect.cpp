/**
 * \file main_robodetect.cpp
 *
 * \date Sep 20, 2013
 **/

#include <string>
#include <iostream>

#include <QtCore/qglobal.h>
#include <QtGui/QtGui>
#include <QApplication>

#include "global.h"
#include "utils.h"

#include "robodetectMainWindow.h"
#include "qtFileLoader.h"




using namespace std;
using namespace corecvs;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(robodetect);
    QTG12Loader::registerMyself();
    QTRGB24Loader::registerMyself();

    setSegVHandler();
    setStdTerminateHandler();

    printf("Starting robodetect...\n");

    QApplication app(argc, argv);
    RobodetectMainWindow mainWindow;

    app.exec();

    printf("Exiting robodetect\n");
    return 0;

}

