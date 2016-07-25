/*
 * \file Cloud.cpp
 *
 * \date Feb 26, 2013
 */

#include "cloud.h"

namespace corecvs {

Cloud* Cloud::filterByAABB(const AxisAlignedBox3d &box)
{
    Cloud* result = new Cloud();
    for (Cloud::iterator it = this->begin(); it != this->end(); ++it)
    {
        if (box.contains((*it).point))
        {
            result->push_back(*it);
        }
    }
    return result;
}

void Cloud::andToMesh(Mesh3D &mesh)
{
    for (Cloud::iterator it = this->begin(); it != this->end(); ++it)
    {
        mesh.addPoint((*it).point);
    }
}

} //namespace corecvs
