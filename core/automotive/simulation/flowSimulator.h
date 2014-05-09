#ifndef FLOWSIMULATOR_H_
#define FLOWSIMULATOR_H_

/**
 * \file flowSimulator.h
 * \brief This is a header file to simulate flow caused by still standing objects and a moving
 * vehicle.
 *
 * \date Feb 22, 2011
 * \author alexander
 */

#include <vector>

#include "global.h"

#include "vector3d.h"
#include "matrix44.h"
#include "flowVector.h"
#include "cameraParameters.h"
#include "eulerAngles.h"

namespace corecvs {


class FlowSimulator
{
public:
    FlowSimulator();
    virtual ~FlowSimulator();

    static std::vector<Vector3dd> *generateMockScene();

    static std::vector<Vector3dd> *unitCube(unsigned gradations);

    static std::vector<Vector3dd> *applyTransform(std::vector<Vector3dd> * pointsA, const Matrix44 &A);

    static std::vector<FloatFlowVector> *simulateFlow(
            const CameraIntrinsics &camIntrinsics,
            const ShiftRotateTransformation &cameraExtrinsics,
            const ShiftRotateTransformation &carMovement,
            double maxZ = 7000.0,
            double minLength = 5.0
            );

};


} //namespace corecvs

#endif /* FLOWSIMULATOR_H_ */
