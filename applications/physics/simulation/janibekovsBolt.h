#ifndef JANIBEKOVSBOLT_H
#define JANIBEKOVSBOLT_H
#include <physMainObject.h>
#include "core/geometry/mesh3d.h"
#include <copterInputs.h>
#include <vector>

#include <core/cameracalibration/cameraModel.h>
#include "core/cameracalibration/calibrationDrawHelpers.h"
#include "core/math/affine.h"
#include "core/math/vector/vector3d.h"

#include "physSphere.h"

class JanibekovsBolt : public PhysMainObject
{

public:
    JanibekovsBolt(double arm = 0.01, double mass = 0.21);
    PhysSphere centralSphere;
    std::vector<PhysSphere> partsOfSystem;
    void drawMyself(Mesh3D &mesh);
    Affine3DQ getTransform();
    void physicsTick(double deltaT);
    virtual void tick(double deltaT) override;
    void drawMyself(Mesh3DDecorated &mesh);

    Mesh3DDecorated *worldMesh = NULL;
};

#endif // JANIBEKOVSBOLT_H
