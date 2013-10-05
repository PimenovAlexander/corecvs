#pragma once
/*
 * \file clustering3d.h
 *
 * \date Mar 15, 2013
 */

#include "global.h"
#include "cloud.h"
#include "cloudCluster.h"
#include "preciseTimer.h"
#include "buffer3d.h"

#include "calculationStats.h"

namespace corecvs {
using std::vector;

class Clustering3D
{
public:
    Clustering3D(Cloud *cloud, int radius, int size, double depth, double headArea, int headNumber) :
         mMarkup(NULL)
       , mCloud(cloud)
       , mRadius(radius)
       , mSize(size)
       , mDepth(depth)
       , mHeadArea(headArea)
       , mHeadNumber(headNumber)
    {
        mMarkup         = NULL;
        mClusterNum     = 0;
        mSortingTime    = 0;
        mClusteringTime = 0;
        mHeadSize.reserve(4);
        mIndexes.reserve(50);
        mClusters.reserve(50);
        mpClusters.reserve(50);
    }

    ~Clustering3D()
    {
        vector<CloudCluster *>::iterator it;
        for (it = mpClusters.begin(); it < mpClusters.end(); it++) delete_safe(*(it));
        delete_safe(mMarkup);
    }

    enum SortingType {
        SORT_X,
        SORT_Y,
        SORT_Z,
        NONE
    };

    uint64_t             mSortingTime;
    uint64_t             mClusteringTime;

    // output data
    vector<Vector3dd> mClustersCenter;
    vector<Vector2dd> mClustersTexCenter;
    vector<Vector3dd> mClustersFlow;
    vector<int>       mClusterSize;
    vector<int>       mCluster6DSize;
    vector<double>    mHeadSize;

    // begin clustering
    void clusterStartRecursive(SortingType sortingType);
    void clusterStarting(int h, int w);
    void _clusterStarting(Statistics &stat);

private:
    // several methods for finding nearest cloud points
    void findAndMarkUpNewNeigbors(int index, int direction);
    void findAndMarkUpNewNeigborsDummy(int index);
    void findNeigborsBehindY(int index);

    // recursive clustering
    void clusteringRecursive(double depth, double parentDepth, SortingType sortingType);
    void clustering(int sizeW, int sizeH, int zoneW, int zoneH, int zoneDepth);
    void _clustering(int sizeW, int sizeH);

    // internal data
    vector<CloudCluster>            mClusters;
    vector<CloudCluster *>          mpClusters;
    AbstractBuffer<CloudCluster *> *mMarkup;
    vector<int>                     mIndexes;
    int                             mClusterNum;


    // input for clustering
    Cloud    *mCloud;
    int       mRadius;
    unsigned  mSize;
    double    mDepth;
    double    mHeadArea;
    unsigned  mHeadNumber;
};

} /* namespace corecvs */

/* EOF */
