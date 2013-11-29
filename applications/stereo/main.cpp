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

	DepthBuffer *buffer = new DepthBuffer(50, 50, false);
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 50; j++) {
			buffer->element(i, j) = i + ((j - i) / 2);
		}
	}

	QImage im("/home/rayman/Downloads/depth.jpg");

	Mesh3DScene *mesh = new Mesh3DScene;
	mesh->addSphere(Vector3dd(0, 0, 0), 10, 1000);

	RectificationResult result;

	QSharedPointer<Scene3D> sc = QSharedPointer<Scene3D>( new StereoReconstructedScene(im) );

    qDebug() << "try";

	w.setNewScenePointer(sc, 9);
    qDebug() << "fin";

    w.show();
    
	return a.exec();
}
