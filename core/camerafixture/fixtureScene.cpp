#include "core/camerafixture/fixtureScene.h"
#include "core/buffers/bufferFactory.h"
#include "core/math/affine.h"
#include "core/utils/utils.h"
#include "core/camerafixture/cameraFixture.h"
#include "core/utils/log.h"
#include "core/filesystem/folderScanner.h"

namespace corecvs {

const char * FixtureSceneFactory::DEFAULT_NAME = "FixtureScene";
Affine3DQ FixtureScene::DEFAULT_WORLD_TO_CAMERA = Affine3DQ(Quaternion::FromMatrix(
                                                                Matrix33( 0, -1,  0,
                                                                          0,  0, -1,
                                                                          1,  0,  0
                                                                )));




/**
 *  Camera  World   |  World   Camera
 *    X      -Y     |    X        Z
 *    Y      -Z     |    Y       -X
 *    Z       X     |    Z       -Y
 **/
FixtureScene::FixtureScene() :
     //worldFrameToCameraFrame(Affine3DQ::RotationZ(degToRad(90.0)) * Affine3DQ::RotationX(degToRad(90.0)))
    worldFrameToCameraFrame(DEFAULT_WORLD_TO_CAMERA)
{
}

void FixtureScene::projectForward(SceneFeaturePoint::PointType mask, bool round)
{
    //SYNC_PRINT(("FixtureScene::projectForward(0x%0X, %s):called\n", mask, round ? "true" : "false"));
    //SYNC_PRINT(("FixtureScene::projectForward(): points %u\n", mSceneFeaturePoints.size()));

    for (SceneFeaturePoint* point : mSceneFeaturePoints)
    {
        //cout << "Projecting point:" << point->name << " (" << point->position << ")"<< endl;
        if ((point->type & mask) == 0) {
            //printf("Skipping (type = %x, mask = %x)\n", point->type, mask);
            continue;
        }
        for (CameraFixture * fixture : mFixtures)
        {
            for (FixtureCamera * camera : fixture->cameras)
            {
                point->projectForward(camera, fixture, round);
            }
        }
    }
}

bool FixtureScene::triangulate(SceneFeaturePoint *point, bool trace, bool checkMinimalAngle)
{
    size_t observationNum = point->observations.size();
    if (observationNum < 2)
    {
        L_WARNING_P("point <%s> has too few observations (%d)", point->name.c_str(), (int)observationNum);
        return false;
    }

    bool ok = true;
    Vector3dd point3d = point->triangulate(false, nullptr, &ok, trace, checkMinimalAngle);

    if (fixtures().size() == 2 && !ok) // case of failed angle check for two cameras
    {
        L_WARNING_P("point <%s> has not passed trangulation angle check, but is triangulated anyway", point->name.c_str());
    }
    else if (!ok) {
        L_WARNING_P("point <%s> MulticameraTriangulator returned false", point->name.c_str());
        return false;
    }

    if (trace)
        cout << "FixtureScene::triangulate(): triangulated to " << point3d << std::endl;

#if 0
    if (point->hasKnownPosition)
        point->reprojectedPosition = point3d;   // store at the reprojectedPosition as "position" is untouchable
    else
        point->position = point3d;              //TODO: why it's stored into the position field instead of reprojectedPosition?
#else
    point->reprojectedPosition = point3d;
#endif

    return true;
}

CameraPrototype *FixtureScene::createCameraPrototype()
{
    CameraPrototype *cameraPrototype = fabricateCameraPrototype();
#ifdef SCENE_OWN_ALLOCATOR_DRAFT
    mOwnedObjects.push_back(cameraPrototype);
#endif
    mCameraPrototypes.push_back(cameraPrototype);
    return cameraPrototype;
}

FixtureCamera *FixtureScene::createCamera()
{
    FixtureCamera *camera = fabricateCamera();
#ifdef SCENE_OWN_ALLOCATOR_DRAFT
    mOwnedObjects.push_back(camera);
#endif
    mOrphanCameras.push_back(camera);
    return camera;
}

CameraFixture *FixtureScene::createCameraFixture()
{
    CameraFixture *fixture = fabricateCameraFixture();
#ifdef SCENE_OWN_ALLOCATOR_DRAFT
    mOwnedObjects.push_back(fixture);
#endif
    mFixtures.push_back(fixture);
    fixture->sequenceNumber = (int)mFixtures.size() - 1;
    return fixture;
}

SceneFeaturePoint *FixtureScene::createFeaturePoint()
{
    SceneFeaturePoint *point = fabricateFeaturePoint();
#ifdef SCENE_OWN_ALLOCATOR_DRAFT
    mOwnedObjects.push_back(point);
#endif
    mSceneFeaturePoints.push_back(point);
    return point;
}

FixtureSceneGeometry *FixtureScene::createSceneGeometry()
{
    FixtureSceneGeometry *geometry = fabricateSceneGeometry();
#ifdef SCENE_OWN_ALLOCATOR_DRAFT
    mOwnedObjects.push_back(geometry);
#endif
    mGeomtery.push_back(geometry);
    return geometry;
}


ImageRelatedData *FixtureScene::createImageData()
{
    ImageRelatedData *image = fabricateImageData();
#ifdef SCENE_OWN_ALLOCATOR_DRAFT
    mOwnedObjects.push_back(image);
#endif
    mImages.push_back(image);
    return image;
}

void FixtureScene::destroyObject(FixtureScenePart *condemned)
{
#ifdef SCENE_OWN_ALLOCATOR_DRAFT
    vectorErase(mOwnedObjects, condemned);
#endif
    delete_safe(condemned);
}

/* This method assumes the scene is well formed */
void FixtureScene::deleteCamera(FixtureCamera *camera)
{
    vectorErase(mOrphanCameras, camera);

    for (size_t i = 0; i < mFixtures.size(); i++)
    {
        CameraFixture *station = mFixtures[i];
        if (station == NULL)
            continue;
        //SYNC_PRINT(("Detaching camera <%s> from station <%s>\n", camera->nameId.c_str(), station->name.c_str()));
        vectorErase(station->cameras, camera);
    }

    for (size_t i = 0; i < mSceneFeaturePoints.size(); i++)
    {
        SceneFeaturePoint *point = mSceneFeaturePoints[i];
        if (point == NULL)
            continue;

        auto it = point->observations.find(camera);
        if (it != point->observations.end()) {
            point->observations.erase(it);
        }

        deleteFixtureCameraUMWPP(point->observations__, camera);
    }

    for (size_t i = 0; i < camera->mImages.size(); i++)
    {
        ImageRelatedData *image = static_cast<ImageRelatedData *>(camera->mImages[i]);
        /* Detach image manually */
        image->camera = NULL;
        deleteImage(image);
    }

    delete_safe(camera);
}

void FixtureScene::deleteCameraPrototype(CameraPrototype *cameraPrototype)
{
     for (FixtureCamera *cam: mOrphanCameras)
     {
         if (cam->cameraPrototype == cameraPrototype)
             cam->cameraPrototype = NULL;
     }

     for (size_t i = 0; i < mFixtures.size(); i++)
     {
        CameraFixture *fixture = mFixtures[i];
        if (fixture == NULL)
            continue;

        for (FixtureCamera *cam: fixture->cameras)
        {
            if (cam->cameraPrototype == cameraPrototype)
                cam->cameraPrototype = NULL;
        }
     }

     vectorErase(mCameraPrototypes, cameraPrototype);
     delete_safe(cameraPrototype);
}

void FixtureScene::deleteCameraFixture(CameraFixture *fixture, bool recursive)
{
    SYNC_PRINT(("FixtureScene::deleteCameraFixture(CameraFixture(%s), %s)\n", fixture == NULL ? "NULL" : fixture->name.c_str(), recursive ? "true" : "false" ));

//     SYNC_PRINT(("FixtureScene::deleteCameraFixture(): purging point references\n"));

    for (size_t i = 0; i < mSceneFeaturePoints.size(); i++)
    {
        SceneFeaturePoint *point = mSceneFeaturePoints[i];
        if (point == NULL) continue;

        deleteCameraFixtureUMWPP(point->observations__, fixture);
    }


//    SYNC_PRINT(("FixtureScene::deleteCameraFixture():checking for recursiveness\n"));
    if (recursive)
    {
        while (!fixture->cameras.empty()) {
            deleteCamera(fixture->cameras.back());
        }
    }
    else
    {
        for (FixtureCamera *cam : fixture->cameras)
        {
            cam->cameraFixture = NULL;
        }
        mOrphanCameras.insert(mOrphanCameras.end(), fixture->cameras.begin(), fixture->cameras.end());
    }

    SYNC_PRINT(("FixtureScene::deleteCameraFixture():actually removing from scene\n"));
    vectorErase(mFixtures, fixture);
    destroyObject(fixture);
}

void FixtureScene::deleteFeaturePoint(SceneFeaturePoint *point)
{
    for (size_t i = 0; i < mGeomtery.size(); i++)
    {
        FixtureSceneGeometry *geometry = mGeomtery[i];
        vectorErase(geometry->relatedPoints, point);
    }
    if (mSceneFeaturePoints.back() == point) {
        mSceneFeaturePoints.pop_back();
    } else {
        vectorErase(mSceneFeaturePoints, point);
    }
    delete_safe(point);

}

void FixtureScene::deleteSceneGeometry(FixtureSceneGeometry *geometry)
{
    vectorErase(mGeomtery, geometry);
    delete_safe(geometry);
}

void FixtureScene::deleteImage(ImageRelatedData *image)
{
    FixtureCamera  *camera  = image->camera;
    if (camera != NULL) {
        vectorErase(camera->mImages, image);
    }

    vectorErase(mImages, image);
    delete_safe(image);
}

void FixtureScene::clear()
{
    SYNC_PRINT(("FixtureScene::clear(): called\n"));

    PreciseTimer dest = PreciseTimer::currentTime();

#ifdef SCENE_OWN_ALLOCATOR_DRAFT

    /** Just purge all heap **/
    for (size_t i = 0; i < mOwnedObjects.size(); i++)
    {
        delete_safe(mOwnedObjects[i]);
    }
    mOwnedObjects.clear();

    mCameraPrototypes.clear();
    mFixtures.clear();
    mOrphanCameras.clear();
    mSceneFeaturePoints.clear();
    mGeomtery.clear();
#else
    
    while (!mGeomtery.empty()) {
        deleteSceneGeometry(mGeomtery.back());
    }

    while (!mImages.empty()) {
        deleteImage(mImages.back());
    }

    while (!mFixtures.empty())
    {
       deleteCameraFixture(mFixtures.back(), true);      
    }

    while (!mOrphanCameras.empty()) {
        deleteCamera(mOrphanCameras.back());
    }

    while (!mCameraPrototypes.empty()) {
        deleteCameraPrototype(mCameraPrototypes.back());
    }

    while (!mSceneFeaturePoints.empty()) {
        deleteFeaturePoint(mSceneFeaturePoints.back());
    }
#endif

    SYNC_PRINT(("FixtureScene::clear(): destruction took %2.2lf ms\n", dest.usecsToNow() / 1000.0));
}

void FixtureScene::deleteFixturePair(CameraFixture *fixture, FixtureCamera *camera)
{
    for (SceneFeaturePoint *p : mSceneFeaturePoints)
    {
        deletePairUMWPP(p->observations__, fixture, camera);
    }
}

/**
 *  We are checking all parent links to be sure that we have a DAG.
 *  All double links will also be found
 *
 **/
bool FixtureScene::checkIntegrity()
{
    bool ok = true;

    for (size_t i = 0; i < mCameraPrototypes.size(); i++)
    {
        CameraPrototype *proto = mCameraPrototypes[i];
        if (proto == NULL) {
             ok = false; SYNC_PRINT(("Camera Prototype is NULL: scene:<%s> pos <%" PRISIZE_T ">\n", this->nameId.c_str(), i));
        }
        if (proto->ownerScene != this) {
             ok = false; SYNC_PRINT(("Camera Prototype  form other scene: cam:<%s> scene:<%s>\n", proto->nameId.c_str(), this->nameId.c_str()));
        }
    }

    for (size_t i = 0; i < mOrphanCameras.size(); i++)
    {
        FixtureCamera *cam = mOrphanCameras[i];
        if (cam == NULL) {
             ok = false; SYNC_PRINT(("Orphan Camera is NULL: scene:<%s> pos <%" PRISIZE_T ">\n", this->nameId.c_str(), i));
        }
        if (cam->ownerScene != this) {
             ok = false; SYNC_PRINT(("Orphan Camera form other scene: cam:<%s> scene:<%s>\n", cam->nameId.c_str(), this->nameId.c_str()));
        }
        if (cam->cameraFixture != NULL) {
             ok = false; SYNC_PRINT(("Orphan Camera pretends to have station: cam:<%s>cam->station:<%s> scene:<%s>\n", cam->nameId.c_str(), cam->cameraFixture->name.c_str(), this->nameId.c_str()));
        }

        for (size_t c = 0; c < cam->mImages.size(); c++)
        {
            ImageRelatedData *image = cam->mImages[c];
            if (image == NULL) {
                 ok = false; SYNC_PRINT(("Image is NULL: scene:<%s> pos <%" PRISIZE_T ">\n", this->nameId.c_str(), i));
            }

        }

    }

    for (size_t i = 0; i < mFixtures.size(); i++)
    {
        CameraFixture *fixture = mFixtures[i];
        if (fixture == NULL) {
            ok = false; SYNC_PRINT(("Station is NULL: scene:<%s> pos <%" PRISIZE_T ">\n", this->nameId.c_str(), i));
        }
        if (fixture->ownerScene != this) {
            ok = false; SYNC_PRINT(("Station form other scene: station:<%s> scene:<%s>\n", fixture->name.c_str(), this->nameId.c_str()));
        }

        for(size_t j = 0; j < fixture->cameras.size(); j++)
        {
            FixtureCamera *cam = fixture->cameras[j];
            if (cam == NULL) {
                ok = false; SYNC_PRINT(("Station Camera is NULL: scene:<%s> station:<%s> pos <%d>\n", this->nameId.c_str(), fixture->name.c_str(), (int)j));
            }
            if (cam->ownerScene != this) {
                ok = false; SYNC_PRINT(("Station Camera form other scene: cam:<%s> station:<%s> scene:<%s>\n", cam->nameId.c_str(), fixture->name.c_str(), this->nameId.c_str()));
            }
#if 0
            if (cam->cameraFixture != fixture) {
                if (cam->cameraFixture) {
                    ok = false; SYNC_PRINT(("Station Camera has NULL station: cam:<%s> station:<%s> scene:<%s>\n", cam->nameId.c_str(), fixture->name.c_str(), this->nameId.c_str()));
                } else {
                    ok = false;SYNC_PRINT(("Station Camera form other station: cam:<%s> station:<%s> cam->station:<%s> scene:<%s>\n", cam->nameId.c_str(), fixture->name.c_str(), cam->cameraFixture->name.c_str(), this->nameId.c_str()));
                }
            }
#endif
        }
    }

    for (size_t i = 0; i < mSceneFeaturePoints.size(); i++)
    {
        SceneFeaturePoint *point = mSceneFeaturePoints[i];
        if (point == NULL) {
            ok = false; SYNC_PRINT(("Point is NULL: scene:<%s> pos <%" PRISIZE_T ">\n", this->nameId.c_str(), i));
        }
        if (point->ownerScene != this) {
            ok = false; SYNC_PRINT(("Point form other scene: point:<%s> scene:<%s>\n", point->name.c_str(), this->nameId.c_str()));
        }

        for (auto it = point->observations.begin(); it != point->observations.end(); ++it)
        {
            CameraModel *cam = it->first;
            const SceneObservation &observ = it->second;
            if (observ.camera == NULL)
            {
                ok = false; SYNC_PRINT(("observation has null camera"));
            }
            if (cam == NULL)
            {
                ok = false; SYNC_PRINT(("there is a null entry in observation"));
            }
            if (observ.camera != cam) {
                ok = false; SYNC_PRINT(("Point observation list malformed"));
            }

            if (observ.featurePoint != point) {
                ok = false; SYNC_PRINT(("Point observation has wrong point pointer"));
            }
        }
        for (auto& it: point->observations__)
        {
            FixtureCamera* fixtureCamera = it.first.v;
            CameraFixture* cameraFixture = it.first.u;
            const SceneObservation &observ = it.second;

            if (observ.camera == NULL)
            {
                ok = false; SYNC_PRINT(("observation__ has null camera"));
            }
            if (observ.cameraFixture == NULL)
            {
                ok = false; SYNC_PRINT(("observation__ has null camera fixture"));
            }
            if (fixtureCamera == WPP::VWILDCARD)
            {
                ok = false; SYNC_PRINT(("there is a wild-card entry in observation__"));
            }
            if (cameraFixture == WPP::UWILDCARD)
            {
                ok = false; SYNC_PRINT(("there is a wild-card entry in observation__"));
            }
            if (observ.camera != fixtureCamera)
            {
                ok = false; SYNC_PRINT(("Point observation__ list malformed"));
            }
            if (observ.cameraFixture != cameraFixture)
            {
                ok = false; SYNC_PRINT(("Point observation__ list malformed"));
            }
            if (observ.featurePoint != point)
            {
                ok = false; SYNC_PRINT(("Point observation__ has wrong point pointer"));
            }

        }
    }

    for (size_t i = 0; i < mGeomtery.size(); i++)
    {
        FixtureSceneGeometry *geometry = mGeomtery[i];
        if (geometry == NULL) {
            ok = false; SYNC_PRINT(("Geometry is NULL: scene:<%s> pos <%" PRISIZE_T ">\n", this->nameId.c_str(), i));
        } else if (geometry->ownerScene != this) {
            ok = false; SYNC_PRINT(("Geometry form other scene: geometry:<%" PRISIZE_T "> scene:<%s>\n", i, this->nameId.c_str()));
        }

        for (auto it = geometry->relatedPoints.begin(); it != geometry->relatedPoints.end(); ++it)
        {
            SceneFeaturePoint *point = *it;
            if (point == NULL) {
                ok = false; SYNC_PRINT(("Related point is NULL: scene:<%s> geometry <%" PRISIZE_T ">\n", this->nameId.c_str(), i));
            } else if (point->ownerScene != this) {
                ok = false; SYNC_PRINT(("Related Point form other scene: point:<%s> scene:<%s>\n", point->name.c_str(), this->nameId.c_str()));
            }
        }
    }

    return ok;
}

bool FixtureScene::integrityRelink()
{
    vectorErase(mImages, (ImageRelatedData *)NULL);

    vectorErase(mOrphanCameras, (FixtureCamera *)NULL);

    for (size_t i = 0; i < mOrphanCameras.size(); i++)
    {
        FixtureCamera *cam = mOrphanCameras[i];
        cam->ownerScene = this;
        cam->cameraFixture = NULL;
        cam->sequenceNumber = (int)i;
        for (size_t c = 0; c < cam->mImages.size(); c++)
        {
            ImageRelatedData *image = cam->mImages[c];
            image->camera = cam;
        }
    }

    vectorErase(mFixtures, (CameraFixture *)NULL);

    for (size_t i = 0; i < mFixtures.size(); i++)
    {
        CameraFixture *fixture = mFixtures[i];
        fixture->ownerScene = this;
        fixture->sequenceNumber = (int)i;

        vectorErase(fixture->cameras, (FixtureCamera *)NULL);

        for(size_t j = 0; j < fixture->cameras.size(); j++)
        {
            FixtureCamera *cam = fixture->cameras[j];
            cam->ownerScene = this;
            cam->cameraFixture = fixture;
            cam->sequenceNumber = (int)j;
            for (size_t c = 0; c < cam->mImages.size(); c++)
            {
                ImageRelatedData *image = cam->mImages[c];
                image->camera = cam;
            }
        }
    }

    vectorErase(mSceneFeaturePoints, (SceneFeaturePoint *)NULL);

    for (size_t i = 0; i < mSceneFeaturePoints.size(); i++)
    {
        SceneFeaturePoint *point = mSceneFeaturePoints[i];
        point->ownerScene = this;

        /* TODO check for NULL */
        for (auto it = point->observations.begin(); it != point->observations.end(); ++it)
        {
            FixtureCamera *cam = it->first;
            SceneObservation &observ = it->second;

            observ.camera = cam;
            if (cam != NULL) {
                observ.cameraFixture = cam->cameraFixture;
            }
            observ.featurePoint = point;
        }
    }

    vectorErase(mGeomtery, (FixtureSceneGeometry *)NULL);

    for (size_t i = 0; i < mGeomtery.size(); i++)
    {
        FixtureSceneGeometry *geometry = mGeomtery[i];
        geometry->ownerScene = this;
    }


    return true;
}

void FixtureScene::merge(FixtureScene *other)
{
    //int oldOrphanNumber = mOrphanCameras.size();

    for(size_t i = 0; i < other->mOrphanCameras.size(); i++)
    {
        FixtureCamera *cam = createCamera();
        cam->copyModelFrom(*other->mOrphanCameras[i]);
    }

    int oldFixtureNumber = (int)mFixtures.size();

    for(size_t i = 0; i < other->mFixtures.size(); i++)
    {
        CameraFixture *otherFixture = other->mFixtures[i];
        CameraFixture *newFixture = createCameraFixture();

        newFixture->location = otherFixture->location;
        newFixture->name     = otherFixture->name;

        for(size_t j = 0; j < otherFixture->cameras.size(); j++)
        {
            FixtureCamera *cam = createCamera();
            cam->copyModelFrom(*otherFixture->cameras[j]);
            addCameraToFixture(cam, newFixture);
        }
    }

    for(size_t i = 0; i < other->mSceneFeaturePoints.size(); i++)
    {
        SceneFeaturePoint *otherPoint = other->mSceneFeaturePoints[i];
        SceneFeaturePoint *newPoint = createFeaturePoint();

        /*This need to be moved in point itself*/

        newPoint->name                        = otherPoint->name;
        newPoint->position                    = otherPoint->position;
        newPoint->hasKnownPosition            = otherPoint->hasKnownPosition;
        newPoint->accuracy                    = otherPoint->accuracy;
        newPoint->reprojectedPosition         = otherPoint->reprojectedPosition;
        newPoint->hasKnownReprojectedPosition = otherPoint->hasKnownReprojectedPosition;
        newPoint->type                        = otherPoint->type;

        for (auto it = otherPoint->observations.begin(); it != otherPoint->observations.end(); ++it)
        {
            FixtureCamera *otherCam = it->first;
            CameraFixture *otherFixture = otherCam->cameraFixture;
            SceneObservation &otherObserv = it->second;

            CameraFixture *thisFixture = mFixtures[oldFixtureNumber + otherFixture->sequenceNumber];
            FixtureCamera *thisCam     = thisFixture->cameras[otherCam->sequenceNumber];

            /*This need to be moved in Observation itself*/
            SceneObservation newObserv = otherObserv;
            newObserv.featurePoint = newPoint;
            newObserv.camera = thisCam;
            newObserv.cameraFixture = thisFixture;

            newPoint->observations.insert(std::pair<FixtureCamera *, SceneObservation>(thisCam, newObserv));
        }
    }
}


void FixtureScene::positionCameraInFixture(CameraFixture * /*fixture */, FixtureCamera *camera, const Affine3DQ &location)
{
//    cout << "FixtureScene::positionCameraInStation()" << std::endl;
//    cout << "  World Transform():" << std::endl;
//    worldFrameToCameraFrame.prettyPrint1();
//    cout << "  Input:" << std::endl;
//    location.prettyPrint1();
    camera->extrinsics = CameraLocationData(location * worldFrameToCameraFrame.inverted());
//    cout << "  In camera form";
//    camera->extrinsics.prettyPrint1();
}

void FixtureScene::addCameraToFixture(FixtureCamera *cam, CameraFixture *fixture)
{
    if (fixture == NULL)
        return;
    auto it = std::find(mOrphanCameras.begin(), mOrphanCameras.end(), cam);
    if (it != mOrphanCameras.end()) {
        mOrphanCameras.erase(it);
    }
    cam->cameraFixture = fixture;
    fixture->cameras.push_back(cam);
    cam->sequenceNumber = (int)fixture->cameras.size() - 1;
}

ImageRelatedData * FixtureScene::addImageToCamera(FixtureCamera *cam, std::string path)
{
    if (cam == NULL)
        return NULL;
    ImageRelatedData *image = createImageData();
    image->mImagePath = path;
    cam->addImageToCamera(image);
    return image;
}

int FixtureScene::getObservationNumber(CameraFixture *fixture)
{
    int count = 0;
    for (size_t i = 0; i < mSceneFeaturePoints.size(); i++)
    {
        SceneFeaturePoint *point = mSceneFeaturePoints[i];
        for (auto it = point->observations.begin(); it != point->observations.end(); ++it)
        {
            FixtureCamera *cam = it->first;
            if (cam->cameraFixture == fixture)
                 count++;
        }
    }
    return count;
}

int FixtureScene::getObservationNumber(FixtureCamera *cam)
{
    int count = 0;
    for (size_t i = 0; i < mSceneFeaturePoints.size(); i++)
    {
        SceneFeaturePoint *point = mSceneFeaturePoints[i];
        if (point->observations.find( cam ) != point->observations.end())
            count++;
    }
    return count;
}

void FixtureScene::dumpInfo(ostream &out, bool brief)
{
    out << "FixtureScene::dumpInfo():" << endl;
#ifdef SCENE_OWN_ALLOCATOR_DRAFT
    out << "Owned objects: " <<  mOwnedObjects.size() << endl;
#endif
    out << "name: " << nameId << "\trelPath: " << relativeImageDataPath << "\tcoordSysState: " << (int)coordinateSystemState << endl;

    out << "Camera Prototypes: " << mCameraPrototypes.size() << endl;
    if (!brief)
        for (size_t j = 0; j < mCameraPrototypes.size(); j++)
        {
            CameraPrototype *proto = mCameraPrototypes[j];
            out << "     " << "CameraPrototype <" << proto->nameId << "> "  << endl;
        }

    out << "Orphan Cameras: " << mOrphanCameras.size() << endl;
    if (!brief)
        for (size_t j = 0; j < mOrphanCameras.size(); j++)
        {
            FixtureCamera *cam = mOrphanCameras[j];
            out << "     " << "Camera <" << cam->nameId << "> " << endl;
        }
    out << "Fixtures: " << mFixtures.size() << endl;
    if (!brief)
        for (size_t i = 0; i < mFixtures.size(); i++)
        {
            CameraFixture *fixture = mFixtures[i];
            out << "  " << "Fixture <" << fixture->name << "> " << fixture->cameras.size() << endl;
            for(size_t j = 0; j < fixture->cameras.size(); j++)
            {
                FixtureCamera *cam = fixture->cameras[j];
                out << "     " << "Camera <" << cam->nameId << "> "  << endl;
                out << "        " << "Size [" << cam->intrinsics->w() << " x " << cam->intrinsics->h() << "] "  << endl;
                out << "        " << "Images "  <<   cam->mImages.size() << endl;

            }
        }

    out << "Points: " << mSceneFeaturePoints.size() << endl;
    out << "   Observations: " <<  totalObservations() << endl;
    out << "Geometries: " << mGeomtery.size() << endl;
    out << "Images: "     << mImages.size() << endl;
}

void FixtureScene::setFixtureCount(size_t count)
{
    //SYNC_PRINT(("FixtureScene::setFixtureCount(%d):  called\n", count));

    while (mFixtures.size() > count)
    {
        CameraFixture *fixture = mFixtures.back();
        deleteCameraFixture(fixture);
        // mFixtures.pop_back(); /* delete camera fixture will generally do it, but only in owner scene.*/
    }

    while (mFixtures.size() < count) {
        createCameraFixture();
    }
}

void FixtureScene::setPrototypeCount(size_t count)
{
    while (mCameraPrototypes.size() > count) {
        CameraPrototype *proto = mCameraPrototypes.back();
        mCameraPrototypes.pop_back();
        deleteCameraPrototype(proto);
    }

    while (mCameraPrototypes.size() < count) {
        createCameraPrototype();
    }
}

void FixtureScene::setOrphanCameraCount(size_t count)
{
    //SYNC_PRINT(("FixtureScene::setOrphanCameraCount(%d):  called\n", count));

    while (mOrphanCameras.size() > count) {
        FixtureCamera *model = mOrphanCameras.back();
        mOrphanCameras.pop_back(); /* delete camera will generally do it, but only in owner scene.*/
        deleteCamera(model);
    }

    while (mOrphanCameras.size() < count) {
        createCamera();
    }
}

//  vector<SceneFeaturePoint *>   points;
void FixtureScene::setFeaturePointCount(size_t count)
{
    //SYNC_PRINT(("FixtureScene::setFeaturePointCount(%" PRISIZE_T "):  called\n", count));

    while (mSceneFeaturePoints.size() > count) {
        SceneFeaturePoint *point = mSceneFeaturePoints.back();
        mSceneFeaturePoints.pop_back();
        deleteFeaturePoint(point);
    }

    while (mSceneFeaturePoints.size() < count) {
        createFeaturePoint();
    }
}

void FixtureScene::setGeometryCount(size_t count)
{
    while (mGeomtery.size() > count) {
        FixtureSceneGeometry *geometry = mGeomtery.back();
        mGeomtery.pop_back();
        deleteSceneGeometry(geometry);
    }

    while (mGeomtery.size() < count) {
        createSceneGeometry();
    }
}



FixtureCamera *FixtureScene::getCameraById(FixtureScenePart::IdType id)
{
    for (FixtureCamera *cam : mOrphanCameras) {
        if (cam->getObjectId() == id) {
            return cam;
        }
    }

    for (CameraFixture *station : mFixtures) {
        for (FixtureCamera *cam: station->cameras) {
            if (cam->getObjectId() == id) {
                return cam;
            }
        }
    }

    return NULL;
}

CameraFixture *FixtureScene::getFixtureById(FixtureScenePart::IdType id)
{
    for (CameraFixture *station : mFixtures) {
        if (station->getObjectId() == id) {
            return station;
        }
    }
    return NULL;
}

SceneFeaturePoint *FixtureScene::getPointById(FixtureScenePart::IdType id)
{
    for (SceneFeaturePoint *point : mSceneFeaturePoints) {
        if (point->getObjectId() == id) {
            return point;
        }
    }
    return NULL;
}

SceneFeaturePoint *FixtureScene::getPointByName(const std::string &name)
{
    for (SceneFeaturePoint *point: mSceneFeaturePoints) {
        if (point->name == name) {
            return point;
        }
    }
    return NULL;
}

CameraFixture *FixtureScene::getFixtureByName(const string& name)
{
    for (size_t stationId = 0; stationId < fixtures().size(); stationId++)
    {
        if (fixtures()[stationId]->name == name) {
            return static_cast<CameraFixture *>(fixtures()[stationId]);
        }
    }
    return NULL;
}

FixtureCamera *FixtureScene::getCameraByName(const string& fixtureName, const string& name)
{
    CameraFixture *station = getFixtureByName(fixtureName);
    if (station == NULL)
        return NULL;

    for (auto cam : station->cameras)
    {
        if (cam->nameId == name)
            return static_cast<FixtureCamera *>(cam);
    }
    return NULL;
}


FixtureCamera *FixtureScene::getCameraByNumber(int fixtureNumber, int cameraNumber)
{
    if (fixtureNumber == -1)
    {
        if (cameraNumber < (int)orphanCameras().size())
            return orphanCameras()[cameraNumber];
        return NULL;
    }

    if (fixtureNumber < (int)fixtures().size())
    {
        if (cameraNumber < (int)fixtures()[fixtureNumber]->cameras.size())
            return fixtures()[fixtureNumber]->cameras[cameraNumber];
    }
    return NULL;
}


FixtureScene::~FixtureScene()
{
    clear();
}


CameraPrototype *FixtureScene::fabricateCameraPrototype()
{
    //SYNC_PRINT(("FixtureScene::fabricateCameraPrototype(): called\n"));
    return new CameraPrototype(this);
}

FixtureCamera *FixtureScene::fabricateCamera()
{
    //SYNC_PRINT(("FixtureScene::fabricateCamera(): called\n"));
    return new FixtureCamera(this);
}

CameraFixture *FixtureScene::fabricateCameraFixture()
{
    //SYNC_PRINT(("FixtureScene::fabricateCameraFixture(): called\n"));
    return new CameraFixture(this);
}

SceneFeaturePoint *FixtureScene::fabricateFeaturePoint()
{
    //SYNC_PRINT(("FixtureScene::fabricateFeaturePoint(): called\n"));
    return new SceneFeaturePoint(this);
}

FixtureSceneGeometry *FixtureScene::fabricateSceneGeometry()
{
    //SYNC_PRINT(("FixtureScene::fabricateSceneGeometry(): called\n"));
    return new FixtureSceneGeometry(this);
}

ImageRelatedData *FixtureScene::fabricateImageData()
{
    //SYNC_PRINT(("FixtureScene::fabricateImageData(): called\n"));
    return new ImageRelatedData(this);
}

void corecvs::FixtureScene::transform(const corecvs::Affine3DQ &transformation, const double scale)
{
    for (auto& pt: mSceneFeaturePoints)
    {
        pt->position = scale * (transformation * pt->position);
        pt->reprojectedPosition = scale * (transformation * pt->reprojectedPosition);
    }

    for (FixtureCamera* fc: mOrphanCameras)
    {
        fc->extrinsics.transform(transformation, scale);
    }

    for (CameraFixture* cf: mFixtures)
    {
        cf->location.shift = scale * (transformation * cf->location.shift);
        cf->location.rotor = transformation.rotor ^ cf->location.rotor;
        for (FixtureCamera *fc : cf->cameras)
        {
            fc->extrinsics.transform(Affine3DQ::Identity(), scale);
        }
    }

    Matrix44 T = Matrix44::Scale(scale) * (Matrix44)transformation;

    for (FixtureSceneGeometry *g: mGeomtery)
    {
        g->transform<Matrix44>(T);
    }
}

std::unique_ptr<FixtureSceneFactory> FixtureSceneFactory::instance;

FixtureSceneFactory *FixtureSceneFactory::getInstance()
{
    if (!instance) {
        instance.reset(new FixtureSceneFactory());
    }

    FixtureSceneCreateFunctor lambda = [](){return new FixtureScene;};
    std::pair<std::string, FixtureSceneCreateFunctor>  entry(std::string(DEFAULT_NAME), lambda);

    instance->creators.insert(entry);

    return instance.get();
}

FixtureScene *FixtureSceneFactory::sceneFactory(const std::string &name)
{
    FixtureScene *toReturn = NULL;
    auto it = creators.find(name);
    if (it == creators.end())
    {
        return toReturn;
    }

    toReturn = (*it).second();
    return toReturn;
}

void FixtureSceneFactory::print()
{
    cout << "Known fixture Scenes:" << endl;
    for (auto &it : creators)
    {
        cout << " " << it.first << endl;
    }
}

std::string ImageRelatedData::getImageScenePath() const
{
    if (ownerScene == NULL || FolderScanner::isAbsolutePath(mImagePath))
    {
        return mImagePath;
    }
    return FolderScanner::concatPath(ownerScene->getImageSearchPath(), mImagePath);
}

RGB24Buffer *ImageRelatedData::getRGB24BufferPtr()
{
    RGB24Buffer* toReturn = BufferFactory::getInstance()->loadRGB24Bitmap(mImagePath);
    return toReturn;
}

RGB24Buffer *ImageRelatedData::getUndistRGB24BufferPtr()
{
    DisplacementBuffer transform = camera->transform(DistortionApplicationParameters());
    corecvs::RGB24Buffer* buffer = getImage()->doReverseDeformationBlTyped<corecvs::DisplacementBuffer>(
                &transform,
                camera->intrinsics->size().y(),
                camera->intrinsics->size().x());
    return buffer;
}

std::shared_ptr<RGB24Buffer> ImageRelatedData::getImage(bool detach, bool forceReload)
{
    if (forceReload || mCache == NULL)
    {
        mCache = std::shared_ptr<RGB24Buffer>(BufferFactory::getInstance()->loadRGB24Bitmap(getImageScenePath()));
    }

    if (!detach)
        return mCache;

    if (mCache == NULL)
        return NULL;

    return std::shared_ptr<RGB24Buffer>(new RGB24Buffer(mCache.get()));
}

G12Buffer *ImageRelatedData::getG12BufferPtr()
{
    CORE_ASSERT_TRUE_P(getImage(false).get(), ("Invalid image"));
    return getImage(false).get()->toG12Buffer();
}

G8Buffer *ImageRelatedData::getG8BufferPtr()
{
    CORE_ASSERT_TRUE_P(getImage(false).get(), ("Invalid image"));
    return getImage(false).get()->getChannelG8(ImageChannel::LUMA);
}

void ImageRelatedData::cleanCache()
{
    mCache.reset();
}


} // namespace
