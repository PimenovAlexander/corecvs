#include "stereoReconstructedScene.h"

#include <QDebug>

#include <QColor>

StereoReconstructedScene::StereoReconstructedScene(DepthBuffer* buffer,
		const int& hScale, const int& wScale, const int& dScale)
{
	qDebug() << "tryfill";

	int counter = 0;

	for (int i = 0; i < buffer->h; i++) {
		for (int j = 0; j < buffer->w; j++) {

//				qDebug() << buffer->element(i, j);
			Vector3dd vertex;
			vertex.x() = j * wScale;
			vertex.y() = i * hScale;
			vertex.z() = buffer->element(i, j) * dScale;

			vertexes.push_back(vertex);

			if (j != (buffer->w - 1)) {
				if (i != (buffer->h - 1)) {
					faces.push_back(Vector3d32(counter, counter + 1, counter + wScale + 1));
					faces.push_back(Vector3d32(counter, counter + wScale, counter + wScale + 1));
				}
			}

			counter++;
		}
	}
}

StereoReconstructedScene::StereoReconstructedScene(QImage image, const int &hScale, const int &wScale, const int &dScale)
{
	qDebug() << "create depthmap scene by image";

	int counter = 0;

	for (int i = 0; i < image.height(); i++) {
		for (int j = 0; j < image.width(); j++) {

//				qDebug() << buffer->element(i, j);
			Vector3dd vertex;
			vertex.x() = j * wScale;
			vertex.y() = i * hScale;
			vertex.z() = qRed(image.pixel(i, j));

			vertexes.push_back(vertex);

			if (j != (image.width() - 1)) {
				if (i != (image.height() - 1)) {
					faces.push_back(Vector3d32(counter, counter + 1, counter + wScale + 1));
					faces.push_back(Vector3d32(counter, counter + wScale, counter + wScale + 1));
				}
			}

			counter++;
		}
	}
}
