#include "sphereReconstructed.h"

#include <QDebug>

namespace corecvs {

const int xCoord = 0;
const int yCoord = 1;
const int zCoord = 2;

SphereReconstructed::SphereReconstructed()
{
	mCenter.x() = 0;
	mCenter.y() = 0;
	mCenter.z() = 0;

	mRadiusVector.x() = 0;
	mRadiusVector.y() = 0;
	mRadiusVector.z() = 0;

	mRadiuses.x() = 0;
	mRadiuses.y() = 0;
	mRadiuses.z() = 0;
}

void SphereReconstructed::reconstruct(Matrix *points)
{

	Vector3dd sum = columnSum(points);

	int size = points->h;

	qDebug() << size;

	Vector3dd mean;
	mean.x() = sum.x() / size;
	mean.y() = sum.y() / size;
	mean.z() = sum.z() / size;

	scale(points, mean);

	fillRadiuses(points);

	qDebug() << "mean" << mean.x() << " " << mean.y() << " " << mean.z();
	qDebug() << "sum" << sum.x() << " " << sum.y() << " " << sum.z();

	Matrix U(points);
	Matrix W(size, 3);
	Matrix V(3, 3);

	Matrix::svd(&U, &W, &V);

	qDebug() << "azaza";

	qDebug() << V.a(0, 0) << " " << V.a(0, 1) << " " << V.a(0, 2);
	qDebug() << V.a(1, 0) << " " << V.a(1, 1) << " " << V.a(1, 2);
	qDebug() << V.a(2, 0) << " " << V.a(2, 1) << " " << V.a(2, 2);

	mCenter.x() = mean.x();
	mCenter.y() = mean.y();
	mCenter.z() = 250;
}

Vector3dd SphereReconstructed::columnSum(Matrix *points)
{
	Vector3dd sum;
	sum.x() = 0;
	sum.y() = 0;
	sum.z() = 0;

	int size = points->h;

	for (int i = 0; i < size; i++) {
		int xx = points->a(i, xCoord);
		int yy = points->a(i, yCoord);
		int zz = points->a(i, zCoord);

		sum.x() = sum.x() + xx;
		sum.y() = sum.y() + yy;
		sum.z() = sum.z() + zz;

		mRadiuses.x() += xx * xx;
		mRadiuses.y() += yy * yy;
		mRadiuses.z() += zz * zz;
	}

	return sum;
}

void SphereReconstructed::fillRadiuses(Matrix *points)
{
	int size = points->h;

	for (int i = 0; i < size; i++) {
		int xx = points->a(i, xCoord);
		int yy = points->a(i, yCoord);
		int zz = points->a(i, zCoord);

		mRadiuses.x() += xx * xx;
		mRadiuses.y() += yy * yy;
		mRadiuses.z() += zz * zz;
	}

	qDebug() << mRadiuses.x() << mRadiuses.y();
	mRadiuses.x() /= size;
	mRadiuses.x() = sqrt(mRadiuses.x());

	mRadiuses.y() /= size;
	mRadiuses.y() = sqrt(mRadiuses.y());

	mRadiuses.z() /= size;
	mRadiuses.z() = sqrt(mRadiuses.z());

	qDebug() << "radiuses " << mRadiuses.x() << " " << mRadiuses.y() << " " << mRadiuses.z();
}

Matrix SphereReconstructed::scale(Matrix *points, Vector3dd coeff)
{
	int size = points->h;

	for (int i = 0; i < size; i++) {
		points->fillLineWithArgs(i
				, points->a(i, xCoord) - coeff.x()
				, points->a(i, yCoord) - coeff.y()
				, points->a(i, zCoord) - coeff.z()
				);
	}
}

}
