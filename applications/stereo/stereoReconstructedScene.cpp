#include "stereoReconstructedScene.h"

#include <QDebug>

#include <QColor>

#include "sphereReconstructed.h"

const int maxDepth = 5;

StereoReconstructedScene::StereoReconstructedScene(DepthBuffer* buffer,
		const double &hScale, const double &wScale, const double &dScale)
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

StereoReconstructedScene::StereoReconstructedScene(QImage image, const double &hScale, const double &wScale, const double &dScale)
{
	qDebug() << "create depthmap scene by image";

	int counter = 0;
	int pointsCounter = 0;

	for (int i = 0; i < image.height() - 1; i = i + 2) {
		for (int j = 0; j < image.width() - 1; j = j + 2) {

			int depth = (qRed(image.pixel(j, i))
					+ qRed(image.pixel(j + 1, i))
					+ qRed(image.pixel(j, i + 1))
					+ qRed(image.pixel(j + 1, i + 1))
					)
					/ 4;

//			qDebug() << depth;

			if (depth > 2) {
				pointsCounter++;
			}


			Vector3dd vertex;
			vertex.x() = j * wScale;
			vertex.y() = i * hScale;
			vertex.z() = (255 - depth) * dScale;

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

	qDebug() << pointsCounter;

	Matrix points(pointsCounter, 3);
	int matrixCounter = 0;

	for (int i = 0; i < vertexes.size(); i++) {

		if (vertexes.at(i)[2] < (255 - 2) * dScale) {
			Vector3dd point = vertexes.at(i);

			points.fillLineWithArgs(matrixCounter, point.x(), point.y(), point.z());
			matrixCounter++;
		}


	}

	qDebug() << matrixCounter;

	SphereReconstructed rec;
	rec.reconstruct(&points);

	radius = rec.radiuses();
	radius.x() /= 2;
	radius.y() /= 2;
	center = rec.center();
	center.z() = 255 * dScale;

	qDebug() << "fin";
}
