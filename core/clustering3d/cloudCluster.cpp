#include "core/clustering3d/cloudCluster.h"

namespace corecvs {

void CloudCluster::getMean()
{
    Vector3dd sum = Vector3dd(0, 0, 0);
    for (CloudCluster::iterator it = this->begin(); it != this->end(); ++it)
    {
        sum = sum + (*it).point;
    }
    mMean = sum / this->size();
}

void CloudCluster::getStat()
{
    m6Dpoints = 0;
    mClusterInfo.point   = Vector3dd(0);
    mClusterInfo.texCoor = Vector2dd(0);
    mClusterInfo.speed   = Vector3dd(0);

    getMean();

    for (CloudCluster::iterator it = this->begin(); it != this->end(); ++it)
    {
        mClusterInfo.point   += (*it).point;   /* find 3D center of cluster */
        mClusterInfo.texCoor += (*it).texCoor; /* find 2D center of cluster */

        if ((*it).is6D)
        {
            mClusterInfo.speed += (*it).speed; /* find mean flow of cluster */
            m6Dpoints++;
        }

        mEllipse.addPoint((*it).point - mMean);        /* find ellipse approximation */
    }
    mClusterInfo.point   /= (double)this->size();
    mClusterInfo.texCoor /= (double)this->size();

    if (m6Dpoints != 0)
        mClusterInfo.speed /= m6Dpoints;

    mEllipse.getEllipseParameters();
}

} //namespace corecvs
