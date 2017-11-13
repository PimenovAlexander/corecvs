#include "core/camerafixture/cameraFixture.h"
#include "core/camerafixture/fixtureScene.h"

namespace corecvs {

void CameraFixture::setCameraCount(size_t count)
{
    SYNC_PRINT(("CameraFixture::setCameraCount(%d)\n", (int)count));

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
        FixtureCamera *model  = ownerScene->createCamera();
        ownerScene->addCameraToFixture(model, this);
    }
}

} // namespace corecvs

