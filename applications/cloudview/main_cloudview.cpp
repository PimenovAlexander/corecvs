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
#include <billboardCaption3DScene.h>

#include <QtGui/QtGui>
#include <QApplication>
#include <core/fileformats/xyzListLoader.h>

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


        if (corecvs::HelperUtils::endsWith(path, ".list"))
        {
            BillboardCaption3DScene *billBoard = new BillboardCaption3DScene;
            XYZListLoader loader;
            std::ifstream in(path, std::ios_base::in);
            loader.loadLabelList(in, billBoard->mLabels);
            cout << "Loaded labels " << billBoard->mLabels.size() << endl;

            mainWindow.addSubObject(QString::fromStdString(path), QSharedPointer<Scene3D>((Scene3D*)billBoard));
            continue;
        }

        if (oldStyle)
        {
            Mesh3DScene *scene = new Mesh3DScene();
            Mesh3D *mesh = new Mesh3D();
            scene->setMesh(mesh);

            if (!loader.load(scene->owned, path))
            {
                delete_safe(scene);
                continue;
            }            
            mainWindow.addSubObject(QString::fromStdString(path), QSharedPointer<Scene3D>((Scene3D*)scene));
        } else {            
            SceneShaded *shaded = new SceneShaded();
            Mesh3DDecorated *mesh = new Mesh3DDecorated();

            if (corecvs::HelperUtils::endsWith(path, ".obj"))
            {
                OBJLoader objLoader;

                /** Load Materials **/
                std::string mtlFile = path.substr(0, path.length() - 4) + ".mtl";
                std::ifstream materialFile;

                cout << "Loading material from <"  << mtlFile << ">" << endl;
                materialFile.open(mtlFile, std::ios::in);
                if (materialFile.good())
                {
                    objLoader.loadMaterials(materialFile, mesh->materials, corecvs::HelperUtils::getDirectory(mtlFile));

                    cout << "Loaded materials: " << mesh->materials.size() << std::endl;
                } else {
                    cout << "Unable to load material" << std::endl;
                }
                materialFile.close();

                /** Load actual data **/
                cout << "Loading geometry from <"  << path <<  ">" << endl;

                std::ifstream file;
                file.open(path, std::ios::in);
                if (file.fail())
                {
                    SYNC_PRINT(("main(): Can't open mesh file <%s> for reading\n", path.c_str()));
                    return false;
                }

                objLoader.loadOBJ(file, *mesh);
                file.close();
            } else {
                if (!loader.load(mesh, path))
                {
                    delete_safe(mesh);
                    delete_safe(shaded);
                    continue;
                }
            }

            mesh->recomputeMeanNormals();
            shaded->setMesh(mesh);
            shaded->prepareMesh(&mainWindow);
            mainWindow.addSubObject(QString::fromStdString(path), QSharedPointer<Scene3D>(shaded));
        }
    }

    app.exec();

    cout << "Exiting Cloudview application  \n";

}

