#include "core/clustering3d/swarmPoint.h"

namespace corecvs {

double SwarmPoint::distTo(SwarmPoint other)
{
    return (this->point - other.point).l1Metric();
}

} //namespace corecvs
