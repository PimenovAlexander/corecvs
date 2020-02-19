#pragma once
/*
 * \file cloud.h
 *
 * \date Feb 20, 2013
 */

#include "utils/global.h"
#include "clustering3d/swarmPoint.h"
#include "geometry/mesh/mesh3d.h"

namespace corecvs {

typedef std::vector<SwarmPoint> CloudBase;


class Cloud : public CloudBase
{
public:
    /*Cloud filtering*/
    Cloud* filterByAABB(const AxisAlignedBox3d &box);
    void andToMesh(Mesh3D &mesh);
};

} /* namespace corecvs */


/* EOF */
