#pragma once
/*
 * \file cloudCluster.h
 *
 * \date Mar 15, 2013
 */

#include "global.h"
#include "cloud.h"
#include "segmentator.h"
#include "ellipticalApproximation.h"

namespace corecvs {

class CloudCluster : public CloudBase, public BaseSegment<CloudCluster>
{
public:
//    CloudCluster() : mEllipse(), m6Dpoints(){}

    void getMean();

    void getStat();

    /* Information about cluster */
    EllipticalApproximationUnified<Vector3dd> mEllipse;
    SwarmPoint                                mClusterInfo;
    int                                       m6Dpoints;
    Vector3dd                                 mMean;


};

struct CloudClusterSort
{
    bool operator()(CloudCluster* const & one, CloudCluster* const & another) const
    {
        return (*(one->begin())).point.y() < (*(another->begin())).point.y();
    }
};

} /* namespace corecvs */

/* EOF */
