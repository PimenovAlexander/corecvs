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
#include "cloudViewDialog.h"
#include "mesh3DScene.h"
#include "core/fileformats/meshLoader.h"
#include "qtFileLoader.h"


int main(int argc, char *argv[])
{
    SET_HANDLERS();

    Q_INIT_RESOURCE(main);
    QTRGB24Loader::registerMyself();

    printf("Starting cloudView...\n");
    QApplication app(argc, argv);
    CloudViewDialog mainWindow(NULL, "Cloud view executable main window");
    MeshLoader loader;

    mainWindow.show();

    for (int i = 1; i < argc; i++) {
        Mesh3DScene *mesh = new Mesh3DScene();

        if (!loader.load(mesh, argv[i]))
        {
            delete_safe(mesh);
            continue;
        }
        mainWindow.addSubObject(argv[i], QSharedPointer<Scene3D>((Scene3D*)mesh));
    }

    app.exec();

    cout << "Exiting Cloudview application  \n";

}

