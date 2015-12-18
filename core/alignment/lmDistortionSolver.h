#ifndef LMDISTORTIONSOLVER_H
#define LMDISTORTIONSOLVER_H

#include "radialCorrection.h"
#include "selectableGeometryFeatures.h"
#include "lineDistortionEstimatorParameters.h"
#include "ellipticalApproximation.h"

namespace corecvs {

class LMDistortionSolver
{
public:
    LMDistortionSolver();

    ObservationList *list;
    //LineDistortionEstimatorParameters parameters;
    Vector2dd initialCenter;

    RadialCorrection solve();
};


class LMLinesDistortionSolver
{
public:
    LMLinesDistortionSolver();

    SelectableGeometryFeatures *lineList;
    LineDistortionEstimatorParameters parameters;
    Vector2dd initialCenter;

    RadialCorrection solve();

    void computeCosts(const RadialCorrection &correction, bool updatePoints = true);
    EllipticalApproximation1d costs[LineDistortionEstimatorCost::LINE_DISTORTION_ESTIMATOR_COST_LAST];

};


}  // namespace corecvs

#endif // LMDISTORTIONSOLVER_H
