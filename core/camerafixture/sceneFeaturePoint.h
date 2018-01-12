#ifndef SCENE_FEATURE_POINT_H
#define SCENE_FEATURE_POINT_H

#include <string>
#include <unordered_map>

#include "core/camerafixture/fixtureCamera.h"
#include "core/features2d/imageKeyPoints.h"
#include "core/camerafixture/wildcardablePointerPair.h"

/* Presentation related */
#include "core/buffers/rgb24/rgbColor.h"

namespace corecvs
{
    template<typename U, typename V>
    class WildcardablePointerPair;
}


namespace corecvs {

class SceneFeaturePoint;

static const double triangulatorCosAngleThreshold = std::cos(degToRad(2.5));

/**
 *
 *   Observation is a class that decribes the result of point beening visble from a particular camera
 *   As with other FixtureScene parts it's contence is governed by guidelines not strict riles.
 *
 *   Working algorithms using this structure actually describe it's own rules. This class is mostly a container
 *
 **/
class SceneObservation
{
public:
    SceneObservation(
              FixtureCamera     *cam = nullptr
            , SceneFeaturePoint *sfp = nullptr
            , Vector2dd          obs = Vector2dd::Zero()
            , CameraFixture     *fix = nullptr)
        : camera(cam)
        , cameraFixture(fix)
        , featurePoint(sfp)
        , accuracy(0.0)
        , observation(obs)
        , observDir(0.0)
        , validityFlags(ValidFlags::OBSERVATION_VALID)
    {}

    /**
     *   Id of the camera that observese the point
     **/
    FixtureCamera      *camera;

    /**
     *   Id of the camera fixture that observese the point
     **/
    CameraFixture      *cameraFixture;

    /**
     *   Point that is observed
     **/
    SceneFeaturePoint  *featurePoint;
    Vector2dd           accuracy;
    enum ValidFlags {
        OBSERVATION_VALID = 0x1,
        DIRECTION_VALID   = 0x2
    };

private:
    mutable Vector2dd   observation;                /**< distorted position at the image */
    mutable Vector3dd   observDir;                  /**< Ray to point from camera origin - this is helpful when camera is not projective */
    mutable int         validityFlags;

public:
    KeyPointArea        keyPointArea;

    /* \depicated Using this is discoraged */
    double              &x() { return observation.x(); }
    double              &y() { return observation.y(); }

    /* \depicated Using this is discoraged */
    void                setObserveDir(const Vector3dd &dir) { observDir = dir; }

    std::string         getPointName();

    /* */
    Vector2dd           getUndist() const;
    Vector2dd           getDist  () const;

    void                setUndist(const Vector2dd &undist);
    void                setDist  (const Vector2dd &dist);

private:
    FixtureCamera      *getCameraById(FixtureCamera::IdType id);

public:
    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(observDir    , Vector3dd(0.0) , "observDir");
        visitor.visit(observation  , Vector2dd(0.0) , "observation");
        visitor.visit(accuracy     , Vector2dd(0.0) , "accuracy");
        visitor.visit(validityFlags, 0              , "validityFlags");

        keyPointArea.accept<VisitorType>(visitor);

        FixtureCamera::IdType id = 0;
        if (camera != NULL) {
            id = camera->getObjectId();
        }
        visitor.visit(id, (uint64_t)0, "camera");

        if (visitor.isLoader())
        {
            camera = getCameraById(id);
            if (camera != NULL) {
                cameraFixture = camera->cameraFixture;
            }
        }

        /* This is a compatibility block. Remove this when all data would be converted */
#if 1
        if (visitor.isLoader())
        {
            if (validityFlags == 0) /* We expect that only legacy scenes would have this */
            {
                bool onDistorted;
                visitor.visit(onDistorted, false, "onDistorted");
                Vector2dd obs = observation;
                if (onDistorted) {
                    setDist(obs);
                } else {
                    setUndist(obs);
                }
            }
        }
#endif
    }
};


typedef WildcardablePointerPair<CameraFixture, FixtureCamera> WPP;


class SceneFeaturePoint : public FixtureScenePart
{
public:
    std::string                 name;

    /** This is a primary position of the FeaturePoint. The one that is know by direct measurement */
    Vector3dd                   position;
    bool                        hasKnownPosition;

    /*
     * Here we'll store some estimation for inverse of covariance matrix
     * So for delta v we'll get v'Av = Mahlanobis distance (which has
     * chi-squared distribution with 3 dof)
     */
    Matrix33                    accuracy;

    // Gets p-value using covariance estimation using one-sided test
    double                      queryPValue(const corecvs::Vector3dd &q) const;

    /** This is a position that is achived by reconstruction */
    Vector3dd                   reprojectedPosition;
    bool                        hasKnownReprojectedPosition;

    enum PointType {
        POINT_UNKNOWN               = 0x00,
        POINT_USER_DEFINED          = 0x01,  /**< Point that comes from a file */
        POINT_RECONSTRUCTED         = 0x02,
        POINT_TEMPORARY             = 0x04,
        POINT_TRIANGULATE           = 0x08,
        POINT_RAW_DETECTED          = 0x10,
        POINT_RAW_DETECTED_MATCHED  = 0x20,
        POINT_ALL                   = 0xFF
    };
    PointType                   type;

    static inline const char *getTypeName(const PointType &value)
    {
        switch (value)
        {
            case POINT_UNKNOWN       : return "POINT_UNKNOWN";
            case POINT_USER_DEFINED  : return "USER_DEFINED" ;
            case POINT_RECONSTRUCTED : return "RECONSTRUCTED";
            case POINT_TEMPORARY     : return "TEMPORARY"    ;
            case POINT_TRIANGULATE   : return "TRIANGULATE"  ;
            case POINT_RAW_DETECTED  : return "RAW_DETECTED" ;
            default                  : return "Not in range" ;
        }
    }

    void setPosition(const Vector3dd &position, PointType type = POINT_USER_DEFINED)
    {
        this->position = position;
        this->hasKnownPosition = true;
        this->type = type;
    }

    Vector3dd getDrawPosition(bool preferReprojected = false, bool forceKnown = false) const;

    /**
     * \brief	This method triangulates a point based on its observations
     *
     * \param use__ 			- should we use observations from observations or observations__
     * \param mask 				- to select observations to use
     * \param succeeded 		- if non-null, returns success or fail (Vector3dd == 0,0,0)
     * \param checkMinimalAngle - 'succeeded' will be false if maximum trabgulation angle less than threshold (2 degree)
     * \param thresholdCos      - if 'checkMinimalAngle' is enabled, this field represents maximum cosine threshold for triangulation angle
     *
     **/
    Vector3dd triangulate(bool use__ = false, std::vector<int> *mask = nullptr, bool* succeeded = nullptr, bool trace = false, bool checkMinimalAngle = false, double thresholdCos = triangulatorCosAngleThreshold);

    /** Observation related block */
    typedef std::unordered_map<FixtureCamera *, SceneObservation> ObservContainer;

    ObservContainer observations;

    std::unordered_map<WildcardablePointerPair<CameraFixture, FixtureCamera>, SceneObservation> observations__;

    /* This is a presentation related block we should move it to derived class */
    RGBColor        color;

    /**/

    SceneFeaturePoint(FixtureScene * owner = NULL) :
        FixtureScenePart(owner),
        position(0.0),
        hasKnownPosition(false),
        reprojectedPosition(0.0),
        hasKnownReprojectedPosition(false),
        type(POINT_UNKNOWN),
        color(RGBColor::White())
    {}

    SceneFeaturePoint(Vector3dd _position, const std::string &_name = std::string(), FixtureScene * owner = NULL) :
        FixtureScenePart(owner),
        name(_name),
        position(_position),
        hasKnownPosition(true),
        reprojectedPosition(0.0),
        hasKnownReprojectedPosition(false),
        type(POINT_UNKNOWN),
        color(RGBColor::White())
    {}

    void transform(const Matrix33 &matrix, const Vector3dd &translate)
    {
        position = matrix * position + translate;
    }

    bool hasObservation(FixtureCamera *cam) { return getObservation(cam) != nullptr; }

    SceneObservation *getObservation(FixtureCamera *cam);

    void removeObservation(SceneObservation *);

    /* Let it be so far like this */
    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        IdType id = getObjectId();
        visitor.visit(id, IdType(0) , "id");
        setObjectId(id);

        visitor.visit(name                       , std::string("")   , "name");
        visitor.visit(position                   , Vector3dd(0.0)    , "position");
        visitor.visit(hasKnownPosition           , false             , "hasKnownPosition");
        visitor.visit(reprojectedPosition        , Vector3dd(0.0)    , "reprojectedPosition");
        visitor.visit(hasKnownReprojectedPosition, false             , "hasKnownReprojectedPosition");
        visitor.visit((int &)type                , (int)POINT_UNKNOWN     , "type");
        visitor.visit(color                      , RGBColor::Black() , "color");

        int observeSize = (int)observations.size();
        visitor.visit(observeSize, 0, "observations.size");

        if (!visitor.isLoader())
        {
            /* We don't load observations here*/

            /* We resort it to make compare easier. We should find a way to make it more stable */
            vector<FixtureCamera *> toSort;
            for (auto &it : observations) {
                toSort.push_back(it.first);
            }

            std::sort(toSort.begin(), toSort.end(), [](const FixtureCamera *first, const FixtureCamera *second) {
                return first->nameId < second->nameId;
            });

            int i = 0;
            for (auto &it : toSort)
            {
                SceneObservation &observ = observations[it];
                char buffer[100];
                snprintf2buf(buffer, "obsrv[%d]", i++);
                visitor.visit(observ, observ, buffer);
            }
        }
        else
        {
            observations.clear();
            SceneObservation observ0;
            for (int i = 0; i < observeSize; i++)
            {
                char buffer[100];
                snprintf2buf(buffer, "obsrv[%d]", i);
                SceneObservation observ;
                observ.featurePoint = this;         // we need to set it before visit()
                visitor.visit(observ, observ0, buffer);

                if (observ.camera == NULL)          // when we load the old format file with another field name
                {
                    snprintf2buf(buffer, "obsereve[%d]", i);
                    visitor.visit(observ, observ0, buffer);
                }

                observations[observ.camera] = observ;
                observations__[WPP(observ.cameraFixture, observ.camera)] = observ;
            }
        }
    }

    /* Helper functions */
    PointPath getEpipath(FixtureCamera *camera1, FixtureCamera *camera2, int segments = 10);

    /* Project this point to a given camera*/
    void projectForward(FixtureCamera *camera, CameraFixture *fixture, bool round);

    /* L2 reprojection error per observation */
    std::vector<double> estimateReconstructedReprojectionErrorL2() const;
    std::vector<double> estimateReprojectionErrorL2() const;

    static bool checkTriangulationAngle(const corecvs::Vector3dd& point, const corecvs::Vector3dd& camera0, const corecvs::Vector3dd& camera1, double thresholdCos = triangulatorCosAngleThreshold);
    static bool checkTriangulationAngle(const corecvs::Vector3dd& point, const std::vector<corecvs::Vector3dd>& cameras, double thresholdCos = triangulatorCosAngleThreshold);

 private:
     bool checkTriangulationAngle(const corecvs::Vector3dd& pointPosition, bool use__ = false, double thresholdCos = triangulatorCosAngleThreshold);
};


class FixtureSceneGeometry : public FixtureScenePart, public FlatPolygon
{
public:
    FixtureSceneGeometry(FixtureScene * owner = NULL) :
        FixtureScenePart(owner)
    {}

    RGBColor color;

    /** Related points container */
    typedef std::vector<SceneFeaturePoint *> RelatedPointsContainer;

    RelatedPointsContainer relatedPoints;


    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        FlatPolygon::accept(visitor);
        visitor.visit(color,   RGBColor::Red(),    "color");

        int relatedSize = (int)relatedPoints.size();
        visitor.visit(relatedSize, 0, "related.size");
        relatedPoints.resize(relatedSize);

        for (int i = 0; i < relatedSize; i++)
        {
            char buffer[100];
            snprintf2buf(buffer, "pointId[%d]", i);

            if (visitor.isLoader())
            {
                SceneFeaturePoint::IdType id;
                visitor.visit(id, IdType(0) , buffer);
                relatedPoints[i] = getPointById(id);
            } else {
                SceneFeaturePoint::IdType id = relatedPoints[i]->getObjectId();
                visitor.visit(id, IdType(0) , buffer);
            }
        }

    }

private:
    SceneFeaturePoint *getPointById  (FixtureScenePart::IdType id);

};

} // namespace corecvs

#endif // SCENE_FEATURE_POINT_H
