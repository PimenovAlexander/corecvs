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
#include <QApplication>


#include "global.h"
#include "utils.h"
#include "advancedImageWidget.h"
#include "qtFileLoader.h"

#include "imageViewMainWindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    setSegVHandler();
    setStdTerminateHandler();

    Q_INIT_RESOURCE(main);

    SYNC_PRINT(("Starting cloudView...\n"));
    QApplication app(argc, argv);
    ImageViewMainWindow mainWindow;

    QTG12Loader::registerMyself();
    QTRGB24Loader::registerMyself();

    if (argc > 1)
    {
        qDebug("Main: %s", argv[1]);
        mainWindow.loadImage(QString(argv[1]));

#if 0
        RGB48Buffer *buffer = PPMLoader().rgb48BufferCreateFromPPM(str);
        if (buffer == NULL)
        {
            qDebug("Can't' open file: %s", str.c_str());
        } else {
            mainWindow.input = buffer;
        }
#endif
    }

    mainWindow.show();
    app.exec();
    SYNC_PRINT(("Exiting ImageView application...\n"));
}
