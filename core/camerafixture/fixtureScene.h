#ifndef FIXTURE_SCENE_H_
#define FIXTURE_SCENE_H_

#include "core/camerafixture/fixtureCamera.h"
#include "core/camerafixture/cameraFixture.h"
#include "core/camerafixture/sceneFeaturePoint.h"

namespace corecvs {

class CameraFixture;


/* Heap of Calibration related stuff */

class FixtureScene
{
public:
    typedef FixtureCamera     CameraType;
    typedef CameraFixture     FixtureType;
    typedef SceneFeaturePoint PointType;


    FixtureScene();

    /**
     * This field encodes the "silent transform that happens when natural world corrdinate system changes to
     * image related. This covers but is not restritced to the transition between:
     *
     *   Z axis pointing to the sky and Z axis pointing to camera optical axis
     *
     * So far you can't change right handed system to left-handed. This cavity needs to be addressed later.
     *
     *  Camera  World   |  World   Camera
     *    Z       X     |    X        Z
     *    Y      -Z     |    Y       -X
     *    X      -Y     |    Z       -Y
     *
     *  This transform only happens when you use ::positionCameraInStation() method. Thoough we encourage you to do so.
     *
     **/
    Affine3DQ worldFrameToCameraFrame;

    std::string nameId;

    /* This is for future, when all the heap/memory will be completed */
    vector<FixtureScenePart *>    mOwnedObjects;

    vector<CameraFixture *>       fixtures;
    vector<FixtureCamera *>       orphanCameras;
    vector<SceneFeaturePoint *>   points;

    /**
     *  Creates and fills the observations with points. It optionally simulates camera by rounding the projection to nearest pixel
     *
     **/
    void projectForward(SceneFeaturePoint::PointType mask, bool round = false);
    void triangulate   (SceneFeaturePoint * point);



protected:
    template<typename T>
    using umwpp = std::unordered_map<WPP, T>;
    template<typename T>
    using umwppv= umwpp<std::vector<T>>;

    template<typename T>
    void deleteFixtureCameraUMWPP(umwpp<T> &uwpp, FixtureCamera* fc)
    {
        WPP wpp(WPP::UWILDCARD, fc);
        bool contains = false;
        do
        {
            contains = false;
            for (auto it = uwpp.begin(); it != uwpp.end(); ++it)
            {
                if (wpp == it->first)
                {
                    uwpp.erase(it);
                    contains = true;
                    break;
                }
            }
        } while(contains);
    }
    template<typename T>
    void deleteFixtureCameraUMWPP(umwpp<umwpp<T>> &uwpp, FixtureCamera* fc)
    {
        WPP wpp(WPP::UWILDCARD, fc);
        for (auto& it: uwpp)
            deleteFixtureCameraUMWPP(it.second, fc);
    }
    template<typename T>
    void deleteCameraFixtureUMWPP(umwpp<T> &uwpp, CameraFixture* fc)
    {
        WPP wpp(fc, WPP::VWILDCARD);
        bool contains = false;
        do
        {
            contains = false;
            for (auto it = uwpp.begin(); it != uwpp.end(); ++it)
            {
                if (wpp == it->first)
                {
                    uwpp.erase(it);
                    contains = true;
                    break;
                }
            }
        } while(contains);
    }
    template<typename T>
    void deleteCameraFixtureUMWPP(umwpp<umwpp<T>> &uwpp, CameraFixture* fc)
    {
        WPP wpp(fc, WPP::VWILDCARD);
        for (auto& it: uwpp)
            deleteCameraFixtureUMWPP(it.second, fc);
    }
    template<typename T>
    void deletePairUMWPP(umwpp<T> &uwpp, CameraFixture* cf, FixtureCamera* fc)
    {
        uwpp.erase(uwpp.find(WPP(cf, fc)));
    }
    template<typename T>
    void deletePairUMWPP(umwpp<umwpp<T>> &uwpp, CameraFixture* cf, FixtureCamera* fc)
    {
        for (auto& it: uwpp)
            deletePairUMWPP(it.second, cf, fc);
    }
    template<typename T>
    void vectorErase(std::vector<T> &v, const T &t)
    {
        v.erase(std::remove(v.begin(), v.end(), t), v.end());
    }


    virtual FixtureCamera      *fabricateCamera();
    virtual CameraFixture      *fabricateCameraFixture();
    virtual SceneFeaturePoint  *fabricateFeaturePoint();


public:

    /**
     * Manipulation with structures
     **/
    virtual FixtureCamera      *createCamera();
    virtual CameraFixture      *createCameraFixture();
    virtual SceneFeaturePoint  *createFeaturePoint();

    /* These methods  compleatly purge camera from scene */
    virtual void deleteCamera        (FixtureCamera *camera);
    virtual void deleteCameraFixture (CameraFixture *fixture, bool recursive = true);
    virtual void deleteFixturePair   (CameraFixture *fixture, FixtureCamera *camera);
    virtual void deleteFeaturePoint  (SceneFeaturePoint *point);

    virtual void clear();

    /**
     *    Helper method to check data structure integrity
     *
     *    The restritions in this method are strongly recomended but not enforced
     *
     **/
    virtual bool checkIntegrity();

    /**
     *    This function takes top to bottom graph and updates all the backlinks to point correctly
     **/
    virtual bool integrityRelink();

    /**
     *  ATTENTION! METHOD COULD BE UNFINISHED
     *  This method merges another FixtureScene in this one with deep copy. This could also be used to clone.
     *  So far fixtures are not merged.
     **/
    virtual void merge(FixtureScene *other);

    virtual void positionCameraInFixture(CameraFixture *station, FixtureCamera *camera, const Affine3DQ &location);
    virtual void addCameraToFixture     (FixtureCamera *cam, CameraFixture *fixture);

    /**/
    virtual int getObeservationNumber(CameraFixture *fixture);
    virtual int getObeservationNumber(FixtureCamera *cam);


    /* Some debugger helpers */
    virtual void dumpInfo(ostream &out = std::cout);


    size_t totalObservations()
    {
        size_t toReturn = 0;
        for (size_t pointId = 0; pointId < points.size(); pointId++)
        {
            const SceneFeaturePoint *point = points[pointId];
            toReturn += point->observations.size();
        }
        return toReturn;
    }

    /**
     *
     **/
    void setFixtureCount     (size_t count);
    void setOrphanCameraCount(size_t count);
    void setFeaturePointCount(size_t count);

    /* This performs full search. It should not */
    FixtureCamera *getCameraById (FixtureScenePart::IdType id);
    CameraFixture *getFixtureById(FixtureScenePart::IdType id);

    SceneFeaturePoint *getPointByName(const std::string &name);


    template<class VisitorType, class SceneType = FixtureScene>
    void accept(VisitorType &visitor, bool loadCameras = true, bool loadFixtures = true, bool loadPoints = true)
    {
        typedef typename SceneType::CameraType   RealCameraType;
        typedef typename SceneType::FixtureType  RealFixtureType;
        typedef typename SceneType::PointType    RealPointType;


        /* So far compatibilty is on */
        /* Orphan cameras */
        if (loadCameras)
        {
            int ocamSize = (int)orphanCameras.size();
            visitor.visit(ocamSize, 0, "orphancameras.size");

            setOrphanCameraCount(ocamSize);

            for (size_t i = 0; i < (size_t)ocamSize; i++)
            {
                char buffer[100];
                snprintf2buf(buffer, "orphancameras[%d]", i);
                visitor.visit(*static_cast<RealCameraType *>(orphanCameras[i]), buffer);
            }
        }

        /* Fixtures*/
        if (loadFixtures)
        {
            int stationSize = (int)fixtures.size();
            visitor.visit(stationSize, 0, "stations.size");

            setFixtureCount(stationSize);

            for (size_t i = 0; i < (size_t)stationSize; i++)
            {
                char buffer[100];
                snprintf2buf(buffer, "stations[%d]", i);
                visitor.visit(*static_cast<RealFixtureType *>(fixtures[i]), buffer);
            }
        }

        /* Points */
        if (loadPoints)
        {
            int pointsSize = (int)points.size();
            visitor.visit(pointsSize, 0, "points.size");

            setFeaturePointCount(pointsSize);

            for (size_t i = 0; i < (size_t)pointsSize; i++)
            {
                char buffer[100];
                snprintf2buf(buffer, "points[%d]", i);
                visitor.visit(*static_cast<RealPointType *>(points[i]), buffer);
            }
        }
    }

    virtual ~FixtureScene();
};

} // namespace corecvs


#endif // FIXTURE_SCENE_H_
