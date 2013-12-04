#ifndef SPHERERECONSTRUCTED_H
#define SPHERERECONSTRUCTED_H

#include <QImage>

#include "matrix.h"
#include "vector3d.h"

namespace corecvs {

class SphereReconstructed
{
public:
	SphereReconstructed();

	void reconstruct(Matrix *points);

	Vector3dd center() const { return mCenter; }
	Vector3dd radiuses() const { return mRadiuses; }

protected:
	Matrix scale(Matrix *points, Vector3dd coeff);

	Vector3dd columnSum(Matrix *points);

	void fillRadiuses(Matrix *points);

private:
	Vector3dd mCenter;

	Vector3dd mRadiusVector;
	Vector3dd mRadiuses;
};

}

#endif // SPHERERECONSTRUCTED_H
