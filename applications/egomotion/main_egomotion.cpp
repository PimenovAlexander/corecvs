/**
 * \file main_qt_egomotion.cpp
 * \brief Entry point for the egomotion application
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include <string>
#include <iostream>

#include <QtGui/QtGui>
#include <QApplication>
#include <QtCore/qobjectdefs.h>

#include "global.h"

#include "utils.h"
#include "egomotionDialog.h"
#include "mainWindow.h"
#include "configManager.h"


using namespace std;

int main(int argc, char *argv[])
{
    setSegVHandler();
    setStdTerminateHandler();

    Q_INIT_RESOURCE(main);

    QString source;
    if (argc != 2)
    {
        //source = string("prec:/media/media/storage/v3i/SafeTurn/image%04d_c%d.pgm");
        //source = string("prec:/media/media/storage/v3i/raws2/1024x512_12h_image_%d_%d.raw");
//        source = QString("prec:/media/media/storage/v3i/framegrabbed1/1024x512_16h_image_%03d_c%d.raw");

        source = QString("prec:data/SafeTurn-left/image%04d_c0.pgm");
        //source = string("v4l2:/dev/video0,/dev/video1:1/10");

    } else {
        source = QString(argv[1]);
    }

    printf("Starting egomotion...\n");

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

    MainWindow mainWindow(new EgomotionDialog(), source, params, false);

    app.exec();

    cout << "Exiting Host application  \n";

}

