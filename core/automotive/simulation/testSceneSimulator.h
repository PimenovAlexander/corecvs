#ifndef TEST_SCENE_SIMULATOR_H_
#define TEST_SCENE_SIMULATOR_H_

/**
 * \file flowSimulator.h
 * \brief This is a header file to simulate flow caused by still standing objects and a moving
 * vehicle.
 *
 * \date Feb 22, 2011
 * \author alexander
 */

#include <vector>

#include "core/utils/global.h"

#include "core/math/vector/vector3d.h"
#include "core/math/matrix/matrix44.h"
#include "core/buffers/flow/flowVector.h"
#include "core/cameracalibration/cameraModel.h"
#include "core/math/eulerAngles.h"

namespace corecvs {


class TestSceneSimulator
{
public:
    TestSceneSimulator();
    virtual ~TestSceneSimulator();

    static std::vector<Vector3dd> *generateMockScene();

    //static std::vector<Vector3dd> *unitCube(unsigned gradations);

    static void unitCube(unsigned gradations, std::vector<Vector3dd> &output);

    static std::vector<Vector3dd> *applyTransform(std::vector<Vector3dd> * pointsA, const Matrix44 &A);

    static std::vector<FloatFlowVector> *simulateFlow(
            const PinholeCameraIntrinsics   &camIntrinsics,
            const ShiftRotateTransformation &cameraExtrinsics,
            const ShiftRotateTransformation &carMovement,
            double maxZ = 7000.0,
            double minLength = 5.0
            );

};


} //namespace corecvs

#endif /* TEST_SCENE_SIMULATOR_H_ */
