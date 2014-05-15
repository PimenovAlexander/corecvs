/**
 * \file main_testbed.cpp
 *
 * \date Sep 20, 2013
 **/

#include <string>
#include <iostream>

#include <QtGui/QtGui>
#include <QtGui/QApplication>

#include "global.h"
#include "utils.h"

#include "testDistortionMainWindow.h"
#include "qtFileLoader.h"



using namespace std;
using namespace corecvs;

int main(int argc, char *argv[])
{
    QTFileLoader::registerMyself();
    setSegVHandler();
    setStdTerminateHandler();

    printf("Starting testbed...\n");

    QApplication app(argc, argv);
    TestbedMainWindow mainWindow;

    app.exec();

    printf("Exiting testbed\n");
    return 0;

}

