#include <algorithm>

#include "affine.h"

#include "multicameraTriangulator.h"
#include "cameraFixture.h"
#include "fixtureScene.h"

using namespace corecvs;

/**
 *  Camera  World   |  World   Camera
 *    X      -Y     |    X        Z
 *    Y      -Z     |    Y       -X
 *    Z       X     |    Z       -Y
 **/
FixtureScene::FixtureScene() :
     //worldFrameToCameraFrame(Affine3DQ::RotationZ(degToRad(90.0)) * Affine3DQ::RotationX(degToRad(90.0)))
    worldFrameToCameraFrame(Affine3DQ(Quaternion::FromMatrix(
        Matrix33( 0, -1,  0,
                  0,  0, -1,
                  1,  0,  0
        ))))
{
}

void FixtureScene::projectForward(SceneFeaturePoint::PointType mask, bool round)
{
    //SYNC_PRINT(("FixtureScene::projectForward(0x%0X, %s):called\n", mask, round ? "true" : "false"));

    //SYNC_PRINT(("FixtureScene::projectForward(): points %u\n", points.size()));

    for (size_t pointId = 0; pointId < points.size(); pointId++)
    {
        SceneFeaturePoint *point = points[pointId];

        //cout << "Projecting point:" << point->name << " (" << point->position << ")"<< endl;

        if ( (point->type & mask) == 0) {
            //printf("Skipping (type = %x, mask = %x)\n", point->type, mask);
            continue;
        }

        //cout << "Projecting" << endl;

        for (size_t fixtureId = 0; fixtureId < fixtures.size(); fixtureId++)
        {
            CameraFixture &fixture = *fixtures[fixtureId];
            for (size_t camId = 0; camId < fixture.cameras.size(); camId++)
            {
                FixtureCamera *camera = fixture.cameras[camId];
                CameraModel worldCam = fixture.getWorldCamera(camera);


                Vector2dd projection = worldCam.project(point->position);
                if (!worldCam.isVisible(projection) || !worldCam.isInFront(point->position))
                    continue;

                if (round) {
                    projection.x() = fround(projection.x());
                    projection.y() = fround(projection.y());
                }

                SceneObservation observation;
                observation.accuracy     = Vector2dd(0.0, 0.0);
                observation.camera       = camera;
                observation.featurePoint = point;
                observation.isKnown      = true;
                observation.observation  = projection;

                Vector3dd direct = worldCam.dirToPoint(point->position).normalised();
                Vector3dd indirect = worldCam.intrinsics.reverse(projection).normalised();

                if (!round) {
                    observation.observDir = direct;
                } else {
                    observation.observDir = indirect;
                }

                /*if (direct.notTooFar(indirect, 1e-7))
                {
                    SYNC_PRINT(("Ok\n"));
                } else {
                    cout << direct << " - " << indirect << "  ";
                    SYNC_PRINT(("Fail\n"));
                }*/

                point->observations[camera] = observation;
                point->observations__[WPP(fixtures[fixtureId], camera)] = observation;
                //cout << "Camera:" << camera->fileName << " = " << projection << endl;
            }
        }
    }
}

void FixtureScene::triangulate(SceneFeaturePoint *point)
{
   MulticameraTriangulator triangulator;

  /* for (size_t stationId = 0; stationId < fixtures.size(); stationId++)
   {
       CameraFixture &station = *fixtures[stationId];
       for (size_t camId = 0; camId < station.cameras.size(); camId++)
       {
           FixtureCamera *camera = station.cameras[camId];
           CameraModel worldCam = station.getWorldCamera(camera);



       }
   }*/

   triangulator.trace = true;

   if (point->observations.size() < 2)
   {
       SYNC_PRINT(("FixtureScene::triangulate(): Too few observations"));
       return;
   }

   for (auto it = point->observations.begin(); it != point->observations.end(); it++)
   {
       FixtureCamera *cam = it->first;
       const SceneObservation &observ = it->second;

       if (cam->cameraFixture == NULL) {
           continue;
       }

       FixtureCamera worldCam = cam->cameraFixture->getWorldCamera(cam);
       triangulator.addCamera(worldCam.getCameraMatrix(), observ.observation);
   }

   Vector3dd point3d;
   bool ok;

   point3d = triangulator.triangulate(&ok);

   if (!ok) {
       SYNC_PRINT(("FixtureScene::triangulate(): MulticameraTriangulator returned false"));
       return;
   }
   cout << "FixtureScene::triangulate(): Triangulated to" << point3d << std::endl;


   point->position = point3d;
}

FixtureCamera *FixtureScene::createCamera()
{
    FixtureCamera *camera = fabricateCamera();
    mOwnedObjects.push_back(camera);
    orphanCameras.push_back(camera);
    return camera;
}

CameraFixture *FixtureScene::createCameraFixture()
{
    CameraFixture *fixture = fabricateCameraFixture();
    mOwnedObjects.push_back(fixture);
    fixtures.push_back(fixture);
    fixture->sequenceNumber = fixtures.size() - 1;
    return fixture;
}

SceneFeaturePoint *FixtureScene::createFeaturePoint()
{
    SceneFeaturePoint *point = fabricateFeaturePoint();
    mOwnedObjects.push_back(point);
    points.push_back(point);
    return point;
}

/* This method assumes the scene is well formed */
void FixtureScene::deleteCamera(FixtureCamera *camera)
{
    vectorErase(orphanCameras, camera);

    for(size_t i = 0; i < fixtures.size(); i++)
    {
        CameraFixture *station = fixtures[i];
        if (station == NULL) continue;
        vectorErase(station->cameras, camera);
    }

    for(size_t i = 0; i < points.size(); i++)
    {
        SceneFeaturePoint *point = points[i];
        if (point == NULL) continue;

        auto it = point->observations.find(camera);
        if ( it != point->observations.end() ) {
            point->observations.erase(it);
        }

        deleteFixtureCameraUMWPP(point->observations__, camera);
    }


    delete_safe(camera);

}

void FixtureScene::deleteCameraFixture(CameraFixture *fixture, bool recursive)
{
    for(size_t i = 0; i < points.size(); i++)
    {
        SceneFeaturePoint *point = points[i];
        if (point == NULL) continue;

        deleteCameraFixtureUMWPP(point->observations__, fixture);
    }
    if (recursive)
    {
        while (!fixture->cameras.empty()) {
            deleteCamera(fixture->cameras.back());
        }
    } else {
        orphanCameras.insert(orphanCameras.end(), fixture->cameras.begin(), fixture->cameras.end());
    }

    vectorErase(fixtures, fixture);
}

void FixtureScene::deleteFeaturePoint(SceneFeaturePoint *point)
{
    vectorErase(points, point);
}

void FixtureScene::clear()
{
    for(size_t i = 0; i < mOwnedObjects.size(); i++)
    {
        delete_safe(mOwnedObjects[i]);
    }

    fixtures.clear();
    orphanCameras.clear();
    points.clear();

}

void FixtureScene::deleteFixturePair(CameraFixture *fixture, FixtureCamera *camera)
{
    for (SceneFeaturePoint *p : points)
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

    for(size_t i = 0; i < orphanCameras.size(); i++)
    {
        FixtureCamera *cam = orphanCameras[i];
        if (cam == NULL) {
             ok = false; SYNC_PRINT(("Orphan Camera is NULL: scene:<%s> pos <%d>\n", this->nameId.c_str(), (int)i));
        }
        if (cam->ownerScene != this) {
             ok = false; SYNC_PRINT(("Orphan Camera form other scene: cam:<%s> scene:<%s>\n", cam->nameId.c_str(), this->nameId.c_str()));
        }
        if (cam->cameraFixture != NULL) {
             ok = false; SYNC_PRINT(("Orphan Camera pretends to have station: cam:<%s>cam->station:<%s> scene:<%s>\n", cam->nameId.c_str(), cam->cameraFixture->name.c_str(), this->nameId.c_str()));
        }
    }

    for(size_t i = 0; i < fixtures.size(); i++)
    {
        CameraFixture *fixture = fixtures[i];
        if (fixture == NULL) {
            ok = false; SYNC_PRINT(("Station is NULL: scene:<%s> pos <%d>\n", this->nameId.c_str(), (int)i));
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

    for(size_t i = 0; i < points.size(); i++)
    {
        SceneFeaturePoint *point = points[i];
        if (point == NULL) {
            ok = false; SYNC_PRINT(("Point is NULL: scene:<%s> pos <%d>\n", this->nameId.c_str(), (int)i));
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


    return ok;
}

bool FixtureScene::integrityRelink()
{
    vectorErase(orphanCameras, (FixtureCamera *)NULL);

    for(size_t i = 0; i < orphanCameras.size(); i++)
    {
        FixtureCamera *cam = orphanCameras[i];
        cam->ownerScene = this;
        cam->cameraFixture = NULL;
    }

    vectorErase(fixtures, (CameraFixture *)NULL);

    for(size_t i = 0; i < fixtures.size(); i++)
    {
        CameraFixture *fixture = fixtures[i];
        fixture->ownerScene = this;

        vectorErase(fixture->cameras, (FixtureCamera *)NULL);

        for(size_t j = 0; j < fixture->cameras.size(); j++)
        {
            FixtureCamera *cam = fixture->cameras[j];
            cam->ownerScene = this;
            cam->cameraFixture = fixture;
        }
    }

    vectorErase(points, (SceneFeaturePoint *)NULL);

    for(size_t i = 0; i < points.size(); i++)
    {
        SceneFeaturePoint *point = points[i];
        point->ownerScene = this;


        /* TODO check for NULL */
        for (auto it = point->observations.begin(); it != point->observations.end(); ++it)
        {
            FixtureCamera *cam = it->first;
            SceneObservation &observ = it->second;

            observ.camera = cam;
            observ.cameraFixture = cam->cameraFixture;
            observ.featurePoint = point;
        }
    }

    return true;
}

void FixtureScene::merge(FixtureScene *other)
{
    //int oldOrphanNumber = orphanCameras.size();

    for(size_t i = 0; i < other->orphanCameras.size(); i++)
    {
        FixtureCamera *cam = createCamera();
        *static_cast<CameraModel *>(cam) = *(other->orphanCameras[i]);
    }


    int oldFixtureNumber = fixtures.size();

    for(size_t i = 0; i < other->fixtures.size(); i++)
    {
        CameraFixture *otherFixture = other->fixtures[i];
        CameraFixture *newFixture = createCameraFixture();

        newFixture->location = otherFixture->location;
        newFixture->name     = otherFixture->name;

        for(size_t j = 0; j < otherFixture->cameras.size(); j++)
        {
            FixtureCamera *cam = createCamera();
            *static_cast<CameraModel *>(cam) = *(otherFixture->cameras[j]);
            addCameraToFixture(cam, newFixture);
        }
    }

    for(size_t i = 0; i < other->points.size(); i++)
    {
        SceneFeaturePoint *otherPoint = other->points[i];
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

            CameraFixture *thisFixture = fixtures[oldFixtureNumber + otherFixture->sequenceNumber];
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

    auto it = std::find(orphanCameras.begin(), orphanCameras.end(), cam);
    if (it != orphanCameras.end()) {
        orphanCameras.erase(it);
    }
    cam->cameraFixture = fixture;
    fixture->cameras.push_back(cam);
    cam->sequenceNumber = fixture->cameras.size() - 1;

}

int FixtureScene::getObeservationNumber(CameraFixture *fixture)
{
    int count = 0;
    for(size_t i = 0; i < points.size(); i++)
    {
        SceneFeaturePoint *point = points[i];
        for (auto it = point->observations.begin(); it != point->observations.end(); ++it)
        {
            FixtureCamera *cam = it->first;
            if (cam->cameraFixture == fixture)
                 count++;
        }
    }
    return count;
}

int FixtureScene::getObeservationNumber(FixtureCamera *cam)
{
    int count = 0;
    for(size_t i = 0; i < points.size(); i++)
    {
        SceneFeaturePoint *point = points[i];
        if (point->observations.find( cam ) != point->observations.end())
            count++;
    }
    return count;
}

void FixtureScene::dumpInfo(ostream &out)
{
    out << "FixtureScene::dumpInfo():" << endl;
    out << "Owned objects: " <<  mOwnedObjects.size() << endl;

    out << "Orphan Cameras: " <<  orphanCameras.size() << endl;
    for(size_t j = 0; j < orphanCameras.size(); j++)
    {
        FixtureCamera *cam = orphanCameras[j];
        out << "     " << "Camera <" << cam->nameId << "> "  << endl;
    }
    out << "Fixtures: " <<  fixtures.size() << endl;
    for(size_t i = 0; i < fixtures.size(); i++)
    {
        CameraFixture *fixture = fixtures[i];
        out << "  " << "Fixture <" << fixture->name << "> " << fixture->cameras.size() << endl;
        for(size_t j = 0; j < fixture->cameras.size(); j++)
        {
            FixtureCamera *cam = fixture->cameras[j];
            out << "     " << "Camera <" << cam->nameId << "> "  << endl;
            out << "        " << "Size [" << cam->intrinsics.w() << " x " << cam->intrinsics.h() << "] "  << endl;

        }
    }

    out << "Points: " <<  points.size() << endl;
    out << "   Observations: " <<  totalObservations() << endl;

}

void FixtureScene::setFixtureCount(size_t count)
{
    //SYNC_PRINT(("FixtureScene::setFixtureCount(%d):  called\n", count));

    while  (fixtures.size() > count) {
        CameraFixture *fixture = fixtures.back();
        fixtures.pop_back(); /* delete camera will generally do it, but only in owner scene.*/
        deleteCameraFixture(fixture);
    }

    while  (fixtures.size() < count) {
        createCameraFixture();
    }
}

void FixtureScene::setOrphanCameraCount(size_t count)
{
    //SYNC_PRINT(("FixtureScene::setOrphanCameraCount(%d):  called\n", count));

    while  (orphanCameras.size() > count) {
        FixtureCamera *model = orphanCameras.back();
        orphanCameras.pop_back(); /* delete camera will generally do it, but only in owner scene.*/
        deleteCamera(model);
    }

    while  (orphanCameras.size() < count) {
        createCamera();
    }
}

//  vector<SceneFeaturePoint *>   points;
void FixtureScene::setFeaturePointCount(size_t count)
{
    //SYNC_PRINT(("FixtureScene::setFeaturePointCount(%d):  called\n", (int)count));

    while  (points.size() > count) {
        SceneFeaturePoint *point = points.back();
        points.pop_back();
        deleteFeaturePoint(point);
    }

    while  (points.size() < count) {
        createFeaturePoint();
    }
}

FixtureCamera *FixtureScene::getCameraById(FixtureScenePart::IdType id)
{
    for (FixtureCamera *cam: orphanCameras) {
        if (cam->getObjectId() == id) {
            return cam;
        }
    }

    for (CameraFixture *station: fixtures) {
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
    for (CameraFixture *station: fixtures) {
        if (station->getObjectId() == id) {
            return station;
        }
    }
    return NULL;
}

SceneFeaturePoint *FixtureScene::getPointByName(const std::string &name)
{
    for (SceneFeaturePoint *point: points) {
        if (point->name == name) {
            return point;
        }
    }
    return NULL;
}



FixtureScene::~FixtureScene()
{
    clear();
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
