#include "buffers/g12Buffer3d.h"
namespace corecvs {

SwarmPoint* G12Buffer3d::p0 = NULL;

G12Buffer3d::~G12Buffer3d()
{
    delete m3d;
    delete mSP;
}


} //namespace corecvs
