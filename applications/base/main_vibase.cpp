/**
 * \file main_qt_host_stub.cpp
 * \brief Includes the main procedure for a host application stub
 *
 * \date Aug 27, 2010
 * \author Sergey Levi
 */

#include <string>
#include <iostream>

//#include <QtCore/QObjectDefs>
#include <QtGui/QtGui>
#include <QApplication>

#include "global.h"

#include "utils.h"
#include "baseHostDialog.h"
#include "mainWindow.h"

int main(int argc, char *argv[])
{
    setSegVHandler();
    setStdTerminateHandler();

    QString source;
    if (argc != 2)
    {
        source = QString("prec:data/pair/image0001%0.0s_c%d.pgm");
        //source = QString("prec:/media/media/storage/v3i/SafeTurn/image%04d_c%d.pgm");
        //source = QString("prec:/media/media/storage/v3i/raws2/1024x512_12h_image_%d_%d.raw");
        //source = QString("prec:/media/media/storage/v3i/framegrabbed1/1024x512_16h_image_%03d_c%d.raw");
        //source = QString("prec:/media/disk/home/alexander/frames/framegrabber/frame%06d_c%d.rawg");
        //source = QString("v4l2:/dev/video0,/dev/video1:1/10");
    } else {
        source = QString(argv[1]);
    }

    QApplication app(argc, argv);
    MainWindowParameters params;
    MainWindow mainWindow(new BaseHostDialog(), source, params);
    mainWindow.show();

    app.exec();

    std::cout << "Exiting Host application  \n";
    return 0;
}

