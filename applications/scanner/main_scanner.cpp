/**
 * \file main_qt_scanner.cpp
 * \brief Entry point for the scanner application
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include <string>
#include <iostream>

#include <QtGui/QtGui>
#include <QApplication>
#include <QtCore/qobjectdefs.h>

#include "core/utils/global.h"

#include "core/utils/utils.h"
#include "scannerDialog.h"
#include "mainWindow.h"
#include "configManager.h"
#include "qtFileLoader.h"


using namespace std;

int main(int argc, char *argv[])
{
    QTRGB24Loader::registerMyself();
    QTG12Loader  ::registerMyself();

    setSegVHandler();
    setStdTerminateHandler();

    Q_INIT_RESOURCE(main);


    /*
    RGB24Buffer *test = BufferFactory::getInstance()->loadRGB24Bitmap("test.png");
    if (test == NULL) {
        printf ("Failed\n");

        return 0;
    } else {
        cout << test->getSize();
    }*/

    QString source;
    source = QString(argv[1]);

    printf("Starting scanner...\n");

    ConfigManager::setConfigName("cvs.conf");
    ConfigManager::setCamConfigName("cvs-camera.conf");


    QApplication app(argc, argv);
    MainWindowParameters params;

    for (int i = 2; i < argc; i++)
    {
        QString val = QString(argv[i]);

        if (val == "auto")
        {
            printf("Autoplay on\n");
            params.autoPlay = true;
        }
    }

    ScannerDialog *scannerDialog = new ScannerDialog();

    if ((argc == 3) && (argv[2] != "auto")) scannerDialog->addScannerPath(QString(argv[2]));

    MainWindow mainWindow(scannerDialog, source, params, true);
    app.exec();

    cout << "Exiting Host application  \n";

}

