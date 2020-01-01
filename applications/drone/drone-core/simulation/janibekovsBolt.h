#ifndef JANIBEKOVSBOLT_H
#define JANIBEKOVSBOLT_H
#include <simulation/physMainObject.h>
#include "geometry/mesh3d.h"
#include <copterInputs.h>
#include <vector>

#include <cameracalibration/cameraModel.h>
#include "cameracalibration/calibrationDrawHelpers.h"
#include "math/affine.h"
#include "math/vector/vector3d.h"

#include "simulation/physSphere.h"

class DzhanibekovBolt : public PhysMainObject
{

public:
    DzhanibekovBolt(double arm = 0.01, double mass = 6);
    PhysSphere centralSphere;
    std::vector<PhysSphere> partsOfSystem;
    void drawMyself(Mesh3D &mesh);
    Affine3DQ getTransform();
    void physicsTick(double deltaT);
    virtual void tick(double deltaT) override;
    void drawMyself(Mesh3DDecorated &mesh);

    Mesh3DDecorated *worldMesh = NULL;
    void drawForces(Mesh3D &mesh);
};

#endif // JANIBEKOVSBOLT_H
