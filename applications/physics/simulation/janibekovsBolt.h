#ifndef JANIBEKOVSBOLT_H
#define JANIBEKOVSBOLT_H
#include <physMainObject.h>

class JanibekovsBolt : public PhysMainObject
{

public:
    JanibekovsBolt(double frameSize, double mass);
    PhysSphere centralSphere;
    std::vector<PhysSphere> partsOfSystem;
};

#endif // JANIBEKOVSBOLT_H
