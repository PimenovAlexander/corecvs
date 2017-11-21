#include "core/camerafixture/sceneFeaturePoint.h"
#include "core/camerafixture/fixtureScene.h"
#include "core/camerafixture/cameraFixture.h"
#include "core/rectification/multicameraTriangulator.h"
#include "core/utils/visitors/propertyListVisitor.h"


#ifdef WITH_BOOST
#include <boost/math/special_functions/gamma.hpp>
#endif

namespace corecvs {

std::string SceneObservation::getPointName()
{
    return featurePoint ? featurePoint->name : "";
}

Vector2dd SceneObservation::getUndist() const
{
    if (validityFlags & ValidFlags::DIRECTION_VALID) {
        return observDir.xy();
    }

    if (camera != NULL && (validityFlags & ValidFlags::OBSERVATION_VALID))
    {
        // This is a temporary solution. Z value should be computed, not just set to focal
        observDir = Vector3dd(camera->distortion.mapBackward(observation), camera->intrinsics.focal.x());
        validityFlags |= (int)ValidFlags::DIRECTION_VALID;
        return observDir.xy();
    }
    return Vector2dd::Zero(); /* Should I return NaN? */
}

Vector2dd SceneObservation::getDist() const
{
    if (validityFlags & ValidFlags::OBSERVATION_VALID) {
        return observation;
    }

    if (camera != NULL && (validityFlags & ValidFlags::DIRECTION_VALID))
    {
        observation = camera->distortion.mapForward(observDir.xy());
        validityFlags |= (int)ValidFlags::OBSERVATION_VALID;
        return observation;
    }
    return Vector2dd::Zero(); /* Should I return NaN? */
}

void SceneObservation::setUndist(const Vector2dd &undist)
{
    if (camera != NULL)
    {
        // This is a temporary solution. Z value should be computed, not just set to focal
        observDir = Vector3dd(undist, camera->intrinsics.focal.x());
        validityFlags = ValidFlags::DIRECTION_VALID;
    }
    else {
        SYNC_PRINT(("SceneObservation::setUndist(): ignored coord as camera is nul\n"));
    }
}

void SceneObservation::setDist(const Vector2dd &dist)
{
    observation = dist;
    validityFlags = ValidFlags::OBSERVATION_VALID;
}

FixtureCamera *SceneObservation::getCameraById(FixtureCamera::IdType id)
{
    CORE_ASSERT_TRUE_S(featurePoint);
    CORE_ASSERT_TRUE_S(featurePoint->ownerScene);

    return featurePoint->ownerScene->getCameraById(id);
}

SceneObservation *SceneFeaturePoint::getObservation(FixtureCamera *cam)
{
    auto it = observations.find(cam);
    if (it == observations.end()) {
        return nullptr;
    }
    return &((*it).second);
}

void SceneFeaturePoint::removeObservation(SceneObservation *in)
{
    auto it = observations.begin();
    for (; it != observations.end(); it++)
    {
        //FixtureCamera *cam = it->first;
        const SceneObservation &observ = it->second;

        if (in == &observ) {
            break;
        }
    }

    if (it == observations.end()) {
        return;
    }

    observations.erase(it);
}

bool SceneFeaturePoint::checkTriangulationAngle(const corecvs::Vector3dd& point, const corecvs::Vector3dd& camera0, const corecvs::Vector3dd& camera1, double thresholdCos)
{
    const corecvs::Vector3dd& c0 = camera0 - point;
    double c0sumsq = c0.sumAllElementsSq();
    const corecvs::Vector3dd& c1 = camera1 - point;
    double c1sumsq = c1.sumAllElementsSq();
    double dot = ( c0 & c1 ) / thresholdCos;
    return dot * dot < c0sumsq * c1sumsq;
}

bool SceneFeaturePoint::checkTriangulationAngle(const corecvs::Vector3dd& point, const std::vector<corecvs::Vector3dd>& cameras, double thresholdCos)
{
    const size_t n = cameras.size();
    for (size_t i = 0; i < n; i++)
    {
        const corecvs::Vector3dd& c0 = cameras[i] - point;
        double c0sumsq = c0.sumAllElementsSq();
        for (size_t j = i + 1; j < n; j++)
        {
            const corecvs::Vector3dd& c1 = cameras[j] - point;
            double c1sumsq = c1.sumAllElementsSq();
            double dot = (c0 & c1) / thresholdCos;
            if (dot * dot < c0sumsq * c1sumsq)
                return true;
        }
    }

    return false;
}

bool SceneFeaturePoint::checkTriangulationAngle(const corecvs::Vector3dd& pointPosition, bool use__, double thresholdCos)
{
    std::vector<corecvs::Vector3dd> cameras;
    if (use__)
    {
        for (auto& obs0 : observations__)
            cameras.push_back(obs0.first.v->getWorldLocation().shift);
    }
    else
    {
        for (auto& obs0 : observations)
            cameras.push_back(obs0.first->getWorldLocation().shift);
    }

    return SceneFeaturePoint::checkTriangulationAngle(pointPosition, cameras, thresholdCos);
}

Vector3dd SceneFeaturePoint::triangulate(bool use__, std::vector<int> *mask, bool* succeeded, bool trace, bool checkMinimalAngle, double thresholdCos)
{
    MulticameraTriangulator mct;
    mct.trace = trace;
    int id = 0;
    size_t ptr = 0;

    if (use__)
    {
        for (auto& obs: observations__)
        {
            if (!mask || (ptr < mask->size() && (*mask)[ptr] == id))
            {
                Vector2dd undist = obs.second.getUndist();
                mct.addCamera(obs.first.u->getMMatrix(obs.first.v), undist);
                if (mask && ptr + 1 < mask->size())
                    CORE_ASSERT_TRUE_S((*mask)[ptr] < (*mask)[ptr + 1]);
                ++ptr;
            }
            if (mask && ptr == mask->size())
                break;
            ++id;
        }
    }
    else
    {
        for (auto& pos : observations)
        {
            FixtureCamera    *cam = pos.first;
            const SceneObservation &obs = pos.second;

            //cout << "SceneFeaturePoint::triangulate(" << name << ") distorted:" << obs.onDistorted << " " << obs.observation << endl;

            //CORE_ASSERT_TRUE_S(obs.cameraFixture != NULL);
            if ((obs.cameraFixture != NULL) && (!mask || (ptr < mask->size() && (*mask)[ptr] == id)))
            {
                Vector2dd projection = obs.getUndist();     // convert projection 'dist => undist' if need
                FixtureCamera worldCam = cam->cameraFixture->getWorldCamera(cam);
                mct.addCamera(worldCam.getCameraMatrix(), projection);

                //mct.addCamera(obs.cameraFixture->getMMatrix(obs.camera), obs.observation);
                if (mask && ptr + 1 < mask->size())
                    CORE_ASSERT_TRUE_S((*mask)[ptr] < (*mask)[ptr + 1]);
                ptr++;
            }
            if (mask && ptr == mask->size())
                break;
            ++id;
        }
    }

    bool ok = false;
    Vector3dd initial = mct.triangulate(&ok);
#ifdef DEEP_TRACE_702
    if (!ok) {
        SYNC_PRINT(("SceneFeaturePoint::triangulate(%s): initial guess unable to obtain\n", name.c_str()));
    }
#endif

	//if (ok == false)
	//	return Vector3dd(0.0);

    ok = true;
    Vector3dd res = mct.triangulateLM(initial, &ok);

#ifdef DEEP_TRACE_702
    if (!ok) {
        SYNC_PRINT(("SceneFeaturePoint::triangulate(%s): LM guess unable to obtain\n", name.c_str()));
    }
#endif

#ifdef DEEP_TRACE_702
    {
        std::ostringstream ss;
        ss << "dump" << name << ".txt";
        PropertyListWriterVisitor writer(ss.str());
        mct.accept<PropertyListWriterVisitor>(writer);
    }
#endif

    // TODO: in fail case covariance couldn't be estimated, thus we keep "accuracy" field as it is.
    if (ok)
        accuracy = mct.getCovarianceInvEstimation(res); 

    if (checkMinimalAngle && ok)
    {
        ok = checkTriangulationAngle(res, use__, thresholdCos);
        //if (!ok)
        //{
        //    res = Vector3dd(0.0);
        //}
    }

    if (succeeded != nullptr)
        *succeeded = ok;

    //if (ok && ownerScene && ownerScene->coordinateSystemState == FixtureScene::CoordinateSystemState::convertable)
     //   res = ownerScene->localToWorld * res;

    return res;
}

double SceneFeaturePoint::queryPValue(const corecvs::Vector3dd &query) const
{
/*
 * Enjoy the first place of adding boost into corecvs!
 * If you are clever enough (and not too lazy) to compute
 * incomplete gamma function inverse manually - consider at least
 * writing something usefull in #else part.
 *
 * You can start from
 *     A. R. Didonato, A. H. Morris, Computation of the Incomplete Gamma Function Ratios and their Inverse 1986
 *
 * Here we are using covariance estimate to get p-value for multivariate normal distribution.
 * NOTE: however it does not seem to work on non-synthetic data either 'cause crude hessian estimation or
 * because of huge non-normality of the estimate
 */
#ifdef WITH_BOOST
    auto q = query - reprojectedPosition;
    return boost::math::gamma_p(3.0 / 2.0, std::max(0.0, q & (accuracy * q)));
#else
    CORE_UNUSED(query);
    return -1.0;
#endif
}

Vector3dd SceneFeaturePoint::getDrawPosition(bool preferReprojected, bool forceKnown) const
{
    if (preferReprojected)
    {
        if (hasKnownReprojectedPosition || forceKnown)
            return reprojectedPosition;

        if (hasKnownPosition || forceKnown)
            return position;
    }
    else
    {
        if (hasKnownPosition || forceKnown)
            return position;

        if(hasKnownReprojectedPosition || forceKnown)
            return reprojectedPosition;
    }
    return position;
}

SceneFeaturePoint *FixtureSceneGeometry::getPointById(FixtureScenePart::IdType id)
{
    return ownerScene->getPointById(id);
}

std::vector< double > SceneFeaturePoint::estimateReconstructedReprojectionErrorL2() const
{
    std::vector< double > out;
    //if (!hasKnownReprojectedPosition)
    //    return out;

    for (auto& obs : observations)
    { 
        Vector2dd xy;
        if (!obs.first->projectPointFromWorld(reprojectedPosition, &xy))
            continue;

        xy -= obs.second.getUndist();
        out.push_back( xy.l2Metric() );
    }
        
    return out;
}

std::vector< double > SceneFeaturePoint::estimateReprojectionErrorL2() const
{
    std::vector< double > out;
    //if (!hasKnownPosition)
    //    return out;

    for (auto& obs : observations)
    {
        Vector2dd xy;
        if (!obs.first->projectPointFromWorld(position, &xy))
            continue;

        xy -= obs.second.getUndist();
        out.push_back(xy.l2Metric());
    }

    return out;
}

PointPath SceneFeaturePoint::getEpipath(FixtureCamera *camera1, FixtureCamera *camera2, int segments)
{
    PointPath result;

    CameraModel secondCamera = camera2->getWorldCameraModel();
    ConvexPolyhedron secondViewport = secondCamera.getCameraViewport();

    SceneObservation *obs1 = getObservation(camera1);

    if (obs1 == NULL) {
        return result;
    }

    Vector2dd m = obs1->getUndist(); /*We work with geometry, so we take projective undistorted objservation */
    PrinterVisitor printer;
    printer.visit(*obs1, "observation");

    CameraModel model = camera1->getWorldCameraModel();
    Ray3d ray = model.rayFromPixel(m);

    //cout << "Ray" << ray << endl;

    /* We go with ray analysis instead of essential matrix beacause it possibly gives
     * more semanticly valuable info
	 */
    double t1 = 0;
    double t2 = 0;
    bool hasIntersection = secondViewport.intersectWith(ray, t1, t2);
    if (hasIntersection)
	{
        if (t1 < 0.0) t1 = 0.0;

        //cout << "t1 : " << t1 << " t2 : " << t2 << endl;

        FixedVector<double, 4> out1 = (secondCamera.getCameraMatrix() * ray.getProjectivePoint(t1));
        FixedVector<double, 4> out2 = (secondCamera.getCameraMatrix() * ray.getProjectivePoint(t2));

        //cout << "out1: " << out1 << endl;
        //cout << "out2: " << out2 << endl;

        Vector2dd pos1 = Vector2dd( out1[0], out1[1]) / out1[2];
        Vector2dd pos2 = Vector2dd( out2[0], out2[1]) / out2[2];

        //cout << "pos1:" << pos1 << endl;
        //cout << "pos2:" << pos2 << endl;

        /* We should apply distorion here */

        for (int segm = 0; segm <= segments; segm++)
        {
            Vector2dd ssP = lerp(pos1, pos2, segm    , 0, segments);
            Vector2dd ssI = secondCamera.distortion.mapForward(ssP);

            //cout << "Undistortion: " << ssP << " - " << ssI << endl;
            //cout << "Undistortion: " << seP << " - " << seI << endl;

            result.push_back(ssI);
        }

    }

    return result;
}


void SceneFeaturePoint::projectForward(FixtureCamera *camera, CameraFixture *fixture, bool round)
{
    CameraModel worldCam = camera->getWorldCameraModel();

    Vector2dd projection = worldCam.project(position);
    if (!worldCam.isVisible(projection) || !worldCam.isInFront(position))
        return;

    if (round) {
        projection.x() = fround(projection.x());
        projection.y() = fround(projection.y());
    }

    SceneObservation observation(camera, this, projection, fixture);
    observation.setUndist(projection);

#if 0 // DEPRICATED
    if (!round) {
        observation.setObserveDir(worldCam.dirToPoint(position).normalised());  // direct
    }
    else {
        observation.setObserveDir(worldCam.intrinsics.reverse(projection).normalised());  // indirect
    }
#endif
    /*if (direct.notTooFar(indirect, 1e-7))
    {
        SYNC_PRINT(("Ok\n"));
    } else {
        cout << direct << " - " << indirect << "  ";
        SYNC_PRINT(("Fail\n"));
    }*/

    observations[camera] = observation;
    observations__[WPP(fixture, camera)] = observation;
}


/*
SceneFeaturePoint *FixtureSceneGeometry::getPointById(FixtureScenePart::IdType id)
{
    CORE_ASSERT_TRUE_S(featurePoint);
    CORE_ASSERT_TRUE_S(featurePoint->ownerScene);

    return featurePoint->ownerScene->getCameraById(id);
}
*/

} //namespace corecvs
