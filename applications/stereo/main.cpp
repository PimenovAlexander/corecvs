#include <QtGui/QApplication>

#include "cloudViewDialog.h"
#include "triangulator.h"
#include "mesh3DScene.h"

#include "stereoReconstructedScene.h"

#include <QImage>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CloudViewDialog w;

	QImage im("/home/rayman/Downloads/depthMap3.png");

//	StereoReconstructedScene *scene = new StereoReconstructedScene(im, 1, 1, 0.5);

	Mesh3DScene *mesh = new Mesh3DScene;
	StereoReconstructedScene *sc = new StereoReconstructedScene(im, 1, 1, 0.5);

	QSharedPointer<Scene3D> depth = QSharedPointer<Scene3D>( sc );

	mesh->addEllipsoid(sc->center, sc->radius, 100);

	QSharedPointer<Scene3D> build = QSharedPointer<Scene3D>( mesh);

    qDebug() << "try";

	w.setNewScenePointer(depth, 8);
	w.setNewScenePointer(build, 7);
    qDebug() << "fin";

    w.show();
    
	return a.exec();
}
