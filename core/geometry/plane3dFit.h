#ifndef PLANE3DFIT_H
#define PLANE3DFIT_H

#include <vector>
#include "math/vector/vector3d.h"
#include "geometry/plane.h"
#include "geometry/ellipticalApproximation.h"

namespace corecvs {

class Plane3dFitParameters {
public:
    double tolerance = 1.0;
    int sampleNumber = 3;

};

class Plane3dFit
{
public:
    class PlaneReconstructionModel {
    public:        
        typedef Vector3dd                SampleType;
        typedef PlaneReconstructionModel ModelType;

        EllipticalApproximation3d approx;
        Plane3d plane;

        PlaneReconstructionModel()
        {
            plane = Plane3d::FromNormal(Vector3dd::OrtX());
        }

        PlaneReconstructionModel (vector<Vector3dd *> &points)
        {
            for (Vector3dd *v: points) {
                approx.addPoint(*v);
            }
            approx.getEllipseParameters();
            plane = Plane3d::FromNormalAndPoint(approx.mAxes[2], approx.getMean());
            plane.normalise();
        }

        static vector<PlaneReconstructionModel> getModels(vector<Vector3dd *> &points, PlaneReconstructionModel * /*context*/ = NULL)
        {
            vector<PlaneReconstructionModel> result;
            result.push_back(PlaneReconstructionModel(points));
            return result;
        }


        double getCost(const Vector3dd &data)
        {
            return plane.distanceTo(data);
        }

        bool fits(const Vector3dd &data, double fitTreshold)
        {
            return (plane.distanceTo(data) < fitTreshold);
        }
    };


public:
    Plane3dFit();

    /* Inputs */

    Plane3dFitParameters mParameters;        
    std::vector<Vector3dd> points;

    void addPoint    (const Vector3dd &point);
    void setPointList(const vector<Vector3dd> &pointList);
    void setParameters(const Plane3dFitParameters& parameters);

    /* Outputs */
    /** Result just after ransac - based on best random sample **/
    PlaneReconstructionModel ransacResult;
    /** Result after rechecking all inliers */
    PlaneReconstructionModel result;

    /* Error flags */
    bool hasError = false;

    void operator ()();

    Plane3d getPlane();

};

} // namespace corecvs

#endif // PLANE3DFIT_H
