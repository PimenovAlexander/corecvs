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

#include "core/utils/global.h"

#include "core/utils/utils.h"
#include "qtFileLoader.h"
#include "imageViewMainWindow.h"

int main(int argc, char *argv[])
{
    SET_HANDLERS();

    Q_INIT_RESOURCE(main);

    SYNC_PRINT(("Starting ImageView...\n"));
    BufferFactory::printCaps();

    QApplication app(argc, argv);
    ImageViewMainWindow mainWindow;

    QTG12Loader::registerMyself();
    QTRGB24Loader::registerMyself();

    if (argc > 1)
    {
        qDebug("Main: %s", argv[1]);
        mainWindow.loadImage(QString(argv[1]));
    }

    mainWindow.show();
    app.exec();
    SYNC_PRINT(("Exiting ImageView application...\n"));
}
