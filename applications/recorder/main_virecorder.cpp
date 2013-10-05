/**
 * \file main_qt_recorder.cpp
 * \brief Entry point for the recorder application
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include <string>
#include <iostream>

#include <QtGui/QtGui>
#include <QtGui/QApplication>
#include <QtCore/qobjectdefs.h>

#include "global.h"

#include "utils.h"
#include "recorderDialog.h"
#include "mainWindow.h"


using namespace std;

int main(int argc, char *argv[])
{
    setSegVHandler();
    setStdTerminateHandler();

    QString source;
    if (argc != 2)
    {
        //source = string("prec:/media/media/storage/v3i/SafeTurn/image%04d_c%d.pgm");
        //source = string("prec:/media/media/storage/v3i/raws2/1024x512_12h_image_%d_%d.raw");
//        source = QString("prec:/media/media/storage/v3i/framegrabbed1/1024x512_16h_image_%03d_c%d.raw");

        //source = string("prec:/media/disk/home/alexander/frames/framegrabber/frame%06d_c%d.rawg");
        //source = string("v4l2:/dev/video0,/dev/video1:1/10");

    } else {
        source = QString(argv[1]);
    }

    printf("Starting recorder...\n");

    QApplication app(argc, argv);
    MainWindowParameters params;
    MainWindow mainWindow(new RecorderDialog(), source, params);

    app.exec();

    cout << "Exiting Host application  \n";

}

