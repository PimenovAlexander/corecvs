/**
 * \file main_testSuperResolution.cpp
 *
 * \date  Nov 28, 2013
 **/

#include <string>
#include <iostream>

#include <QtGui/QtGui>
#include <QtGui/QApplication>

#include "global.h"
#include "utils.h"

#include "testSuperResolutionMainWindow.h"
#include "qtFileLoader.h"



using namespace std;
using namespace corecvs;

int main(int argc, char *argv[])
{
    QTFileLoader::registerMyself();
    setSegVHandler();
    setStdTerminateHandler();

    printf("Starting testSuperResolution...\n");

    QApplication app(argc, argv);
    TestSuperResolutionMainWindow mainWindow;

    app.exec();

    printf("Exiting testSuperResolution\n");
    return 0;

}


