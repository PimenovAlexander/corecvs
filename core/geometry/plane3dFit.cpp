#include "core/geometry/plane3dFit.h"
#include "core/rectification/ransac.h"

namespace corecvs {

Plane3dFit::Plane3dFit()
{

}

void Plane3dFit::addPoint(const Vector3dd &point)
{
    points.push_back(point);
}

void Plane3dFit::setPointList(const vector<Vector3dd> &pointList)
{
    points.clear();
    points = pointList;
}

void Plane3dFit::setParameters(const Plane3dFitParameters &parameters)
{
    mParameters = parameters;
}

void Plane3dFit::operator ()()
{
    Ransac<PlaneReconstructionModel> estimator(mParameters.sampleNumber);

    estimator.setInlierThreshold  (mParameters.tolerance);
    estimator.setIterationsNumber (1000);
    estimator.setInliersPercent   (1.0);
    estimator.trace = true;


    SYNC_PRINT(("Plane3dFit::operator (): %d points", (int)points.size()));
    if (points.size() < 3) {
        SYNC_PRINT(("Plane3dFit::operator (): Too few points to make fitting"));
        hasError = true;
        return;
    }

    vector<Vector3dd *> ptrArr;
    ptrArr.reserve(points.size());
    for (Vector3dd &v : points)
    {
        ptrArr.push_back(&v);
    }

    estimator.data = &ptrArr;
    ransacResult = estimator.getModelRansac();

    /* Collect info about inliers */
    vector<Vector3dd *> inliers;

    for (Vector3dd &v : points)
    {
        if (ransacResult.fits(v, mParameters.tolerance))
        {
            inliers.push_back(&v);
        }
    }


    result = PlaneReconstructionModel(inliers);
}

Plane3d Plane3dFit::getPlane()
{
    return result.plane;
}

} // namespace corecvs
