/**
 * \file main_qt_copter.cpp
 * \brief Entry point for the copter application
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
#include "copterDialog.h"
#include "mainWindow.h"
#include "configManager.h"
#include "qtFileLoader.h"


using namespace std;

int main(int argc, char *argv[])
{
    setSegVHandler();
    setStdTerminateHandler();

    Q_INIT_RESOURCE(main);


    QTG12Loader::registerMyself();
    QTRGB24Loader::registerMyself();



    QString source;
    if (argc < 2)
    {
        //source = string("prec:/media/media/storage/v3i/SafeTurn/image%04d_c%d.pgm");
        //source = string("prec:/media/media/storage/v3i/raws2/1024x512_12h_image_%d_%d.raw");
//        source = QString("prec:/media/media/storage/v3i/framegrabbed1/1024x512_16h_image_%03d_c%d.raw");

        //source = string("prec:/media/disk/home/alexander/frames/framegrabber/frame%06d_c%d.rawg");
        //source = string("v4l2:/dev/video0,/dev/video1:1/10");

    } else {
        source = QString(argv[1]);
    }

    printf("Starting copter...\n");

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

    MainWindow mainWindow(new CopterDialog(), source, params, true);

    app.exec();

    cout << "Exiting Host application  \n";

}

