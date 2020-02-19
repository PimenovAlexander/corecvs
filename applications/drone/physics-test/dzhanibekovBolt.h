#ifndef DZANIBEKOVS_BOLT_H
#define DZANIBEKOVS_BOLT_H

#include <vector>

#include "geometry/mesh/mesh3d.h"
#include "cameracalibration/cameraModel.h"
#include "cameracalibration/calibrationDrawHelpers.h"
#include "math/affine.h"
#include "math/vector/vector3d.h"

#include "simulation/physicsSphere.h"
#include "simulation/physicsCompoundObject.h"
#include "copterInputs.h"

class DzhanibekovBolt : public PhysicsMainObject
{

public:
    DzhanibekovBolt(double arm = 0.01, double mass = 13);

    /* Owned */
    PhysicsSphere *centralSphere;
    std::vector<PhysicsSphere *> systemElements;

    Affine3DQ getTransform();
    void physicsTick (double deltaT);
    virtual void tick(double deltaT) override;

    void drawMyself(Mesh3D &mesh);
    void drawMyself(Mesh3DDecorated &mesh);
    void drawForces(Mesh3D &mesh);

    bool testMode = false;
    Mesh3DDecorated *worldMesh = NULL;
    double mw;

    virtual ~DzhanibekovBolt() override;
};

#endif // DZANIBEKOVS_BOLT_H
