#include "core/camerafixture/cameraFixture.h"
#include "core/camerafixture/fixtureScene.h"

namespace corecvs {

void CameraFixture::setCameraCount(size_t count)
{
    //SYNC_PRINT(("CameraFixture::setCameraCount(%d)\n", (int)count));

    if (ownerScene == NULL)
    {
        SYNC_PRINT(("CameraFixture(%p)::setCameraCount(): ownerScene is NULL ", static_cast<void*>(this)));
        return;
    }

    while (cameras.size() > count)
    {
        FixtureCamera *model = cameras.back();
        cameras.pop_back(); /* delete camera will generally do it, but only in owner scene.*/
        model->ownerScene->deleteCamera(model);
    }

    while (cameras.size() < count)
    {
        FixtureCamera *model = ownerScene->createCamera();
        ownerScene->addCameraToFixture(model, this);
    }
}

void CameraFixture::transformLocation(const Matrix44& coordinatesTransform)
{
	location = getTransformedLocation(coordinatesTransform);
}

Affine3DQ CameraFixture::getTransformedLocation(const Matrix44& coordinatesTransform) const
{
    return getTransformedLocation(coordinatesTransform, location);
}

Affine3DQ CameraFixture::getTransformedLocation(const Matrix44& coordinatesTransform, const Affine3DQ &location)
{
	static const Vector3dd x0 = { 1, 0, 0 };
	static const Vector3dd y0 = { 0, 1, 0 };
	static const Vector3dd z0 = { 0, 0, 1 };
	static const Vector3dd zero0 = { 0, 0, 0 };
	Matrix44 m = (Matrix44)location;
	m = coordinatesTransform * m;
	Vector3dd x = m * x0;
	Vector3dd y = m * y0;
	Vector3dd z = m * z0;
	Vector3dd shift = m * zero0;

	x -= shift;
	y -= shift;
	z -= shift;

	x.normalise();
	y.normalise();
	z.normalise();

	Matrix33 basisRotation(x, y, z);
	basisRotation.transpose();
	return Affine3DQ(Quaternion::FromMatrix(basisRotation), shift);
}

} // namespace corecvs
