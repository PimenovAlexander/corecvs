#ifndef TESTPHYSICSOBJECT_H
#define TESTPHYSICSOBJECT_H

#include "core/geometry/mesh/mesh3d.h"
#include <copterInputs.h>
#include <vector>

#include <cameracalibration/cameraModel.h>
#include "cameracalibration/calibrationDrawHelpers.h"
#include "math/affine.h"
#include "math/vector/vector3d.h"

#include "simulation/physicsSphere.h"
#include "simulation/physicsCompoundObject.h"

class TestPhysicsObject : public PhysicsMainObject
{

public:
    TestPhysicsObject(double arm = 0.1, double mass = 4);
    PhysicsSphere *centralSphere;
    std::vector<PhysicsSphere*> systemElements;

    void drawMyself(Mesh3D &mesh);
    Affine3DQ getTransform();
    void physicsTick(double deltaT);
    virtual void tick(double deltaT) override;
    void drawMyself(Mesh3DDecorated &mesh);
    bool testMode = false;
    Mesh3DDecorated *worldMesh = NULL;
    void drawForces(Mesh3D &mesh);
    double mw;


};

#endif // TESTPHYSICSOBJECT_H
