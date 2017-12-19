/**
 * \file main_qt_recorder.cpp
 * \brief Entry point for the recorder application
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include <string>
#include <iostream>
#include <sceneShaded.h>

#include <QtGui/QtGui>
#include <QApplication>

#include "core/fileformats/objLoader.h"
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
        bool oldStyle = true;
        std::string path = argv[i];

        if (corecvs::HelperUtils::startsWith(path, "n:"))
        {
            path = path.substr(strlen("n:"));
            oldStyle = false;
        }

        if (oldStyle)
        {
            Mesh3DScene *mesh = new Mesh3DScene();

            if (!loader.load(mesh, path))
            {
                delete_safe(mesh);
                continue;
            }
            mainWindow.addSubObject(QString::fromStdString(path), QSharedPointer<Scene3D>((Scene3D*)mesh));
        } else {
            SceneShaded *shaded = new SceneShaded();
            Mesh3DDecorated *mesh = new Mesh3DDecorated();
            OBJLoader objLoader;

            std::ifstream file;
            file.open(path, std::ios::in);
            objLoader.loadOBJ(file, *mesh);
            file.close();

            shaded->mMesh = mesh;
            shaded->mMesh->recomputeMeanNormals();
            shaded->prepareMesh(&mainWindow);
            mainWindow.addSubObject(QString::fromStdString(path), QSharedPointer<Scene3D>(shaded));
        }
    }

    app.exec();

    cout << "Exiting Cloudview application  \n";

}

