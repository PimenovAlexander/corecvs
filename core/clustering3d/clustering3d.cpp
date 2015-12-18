#include "clustering3d.h"

namespace corecvs {

void Clustering3D::findAndMarkUpNewNeigborsDummy(int index)
{
    mIndexes.clear(); // clear old shared index vector
    int i = 0;
    while (index - i >= 0)
    {
        if ((*mCloud)[index - i].cluster == 0 && (*mCloud)[index].distTo((*mCloud)[index - i]) < mRadius)
        {
            (*mCloud)[index - i].cluster = (*mCloud)[index].cluster;
            mIndexes.push_back(index - i);
            mClusters[mClusterNum - 1].push_back((*mCloud)[index - i]);
        }
        i++;
    }

    i = 0;
    while (unsigned(index + i) >= (*mCloud).size())
    {
        if ((*mCloud)[index + i].cluster == 0 && (*mCloud)[index].distTo((*mCloud)[index + i]) < mRadius)
        {
            (*mCloud)[index + i].cluster = (*mCloud)[index].cluster;
            mIndexes.push_back(index + i);
            mClusters[mClusterNum - 1].push_back((*mCloud)[index + i]);
        }
        i++;
    }
}

void Clustering3D::findAndMarkUpNewNeigbors(int index, int direction)
{
    mIndexes.clear(); // clear old shared index vector

    double currentCoord = (*mCloud)[index].point[direction];

    int i = 0;
    double dCurrentCoord = currentCoord - (*mCloud)[index - i].point[direction];
    while (dCurrentCoord < mRadius)
    {
        if ((*mCloud)[index - i].cluster == 0 && (*mCloud)[index].distTo((*mCloud)[index - i]) < mRadius)
        {
            (*mCloud)[index - i].cluster = (*mCloud)[index].cluster;
            mIndexes.push_back(index - i);
            mClusters[mClusterNum - 1].push_back((*mCloud)[index - i]);
        }
        i++;
        if (index - i < 0)
            break;
        dCurrentCoord = currentCoord - (*mCloud)[index - i].point[direction];
    }

    i = 0;
    dCurrentCoord = (*mCloud)[index + i].point[direction] - currentCoord;
    while (dCurrentCoord < mRadius)
    {
        if ((*mCloud)[index + i].cluster == 0 && (*mCloud)[index].distTo((*mCloud)[index + i]) < mRadius)
        {
            (*mCloud)[index + i].cluster = (*mCloud)[index].cluster;
            mIndexes.push_back(index + i);
            mClusters[mClusterNum - 1].push_back((*mCloud)[index + i]);
        }
        i++;
        if (unsigned(index + i) >= (*mCloud).size())
            break;
        dCurrentCoord = (*mCloud)[index + i].point[direction] - currentCoord;
    }
}

void Clustering3D::findNeigborsBehindY(int index)
{
    mIndexes.clear(); // clear old shared index vector

    double currentCoord = (*mCloud)[index].point.y();

    int i = 0;
    double dCurrentCoord = currentCoord - (*mCloud)[index - i].point.y();
    while (dCurrentCoord < mRadius)
    {
        if ( (*mCloud)[index].distTo((*mCloud)[index - i]) < mRadius )
        {
            mIndexes.push_back(index - i);
        }
        i++;
        if (index - i < 0)
            break;
        dCurrentCoord = currentCoord - (*mCloud)[index - i].point.y();
    }
    return;
}

void Clustering3D::clusteringRecursive(double depth, double parentDepth, SortingType sortingType)
{
    vector<int> localIndexes = mIndexes;
    unsigned length = (unsigned)localIndexes.size();
    for (unsigned i = 0; i < length; i++)
    {
        bool isHead = false;
        if ((*mCloud)[localIndexes[i]].cluster == 0) // create new cluster
        {
            mClusterNum++;
            (*mCloud)[localIndexes[i]].cluster = mClusterNum;
            CloudCluster forPush;
            mClusters.push_back(forPush);
            mClusters[mClusterNum - 1].push_back((*mCloud)[localIndexes[i]]);
            isHead = true;
        }

        double delta = 0;
        int direction;
        switch (sortingType)
        {
            default:
            case NONE:
                findAndMarkUpNewNeigborsDummy(localIndexes[i]);
                clusteringRecursive(0, 0, sortingType);
                return;
            case SORT_X:
                direction = 0;
                break;
            case SORT_Y:
                direction = 1;
                break;
            case SORT_Z:
                direction = 2;
                break;
        }

        delta = isHead ? 0 : (*mCloud)[localIndexes[i]].point[direction] - parentDepth;
        if (depth - delta >= 0)
        {
            findAndMarkUpNewNeigbors(localIndexes[i], direction);
            clusteringRecursive(depth - delta, (*mCloud)[localIndexes[i]].point[direction], sortingType);
        }
    }
}


void Clustering3D::clusterStartRecursive(SortingType sortingType)
{
    mClustersCenter.clear();
    mClustersTexCenter.clear();
    mClustersFlow.clear();
    mClusterSize.clear();
    mCluster6DSize.clear();
    mHeadSize.clear();

    PreciseTimer stat = PreciseTimer::currentTime();
    switch (sortingType)
    {
        case SORT_X:
            std::sort((*mCloud).begin(), (*mCloud).end(), SortSwarmPointX());
            break;
        case SORT_Y:
            std::sort((*mCloud).begin(), (*mCloud).end(), SortSwarmPointY());
            break;
        case SORT_Z:
            std::sort((*mCloud).begin(), (*mCloud).end(), SortSwarmPointZ());
            break;
        case NONE:
            break;
    }
    mSortingTime = stat.usecsTo(PreciseTimer::currentTime());

    for (unsigned i = 0; i < mCloud->size(); i++)
    {
        if ((*mCloud)[i].cluster != 0) {
            continue;
        }

        mIndexes.clear();
        mIndexes.push_back((int)i);
        clusteringRecursive(mDepth, 0, sortingType);
        if (mClusters.back().size() > mSize )
        {
            bool isGood = true;
            int sz = (int)mClustersCenter.size();
            mClusters.back().getStat();
            for (int i = 0; i < sz; i++)
            {
                // check if new cluster not down from previous
                if (fabs(mClusters.back().mClusterInfo.point.z() - mClustersCenter[i].z()) < mHeadArea &&
                    fabs(mClusters.back().mClusterInfo.point.x() - mClustersCenter[i].x()) < mHeadArea )
                    isGood = false;
            }
            if (isGood)
            {
                mClustersTexCenter.push_back(mClusters.back().mClusterInfo.texCoor);
                mClustersCenter.push_back(mClusters.back().mClusterInfo.point);
                mClustersFlow.push_back(mClusters.back().mClusterInfo.speed);
                mClusterSize.push_back((int)mClusters.back().size());
                mCluster6DSize.push_back(mClusters.back().m6Dpoints);
                vector<double> tmpVect = mClusters.back().mEllipse.mValues;
                double forMax = 0;
                for (unsigned i = 0; i < tmpVect.size(); i++)
                {
                    forMax = forMax < tmpVect[i] ? tmpVect[i] : forMax;
                }
                mHeadSize.push_back(forMax);
            }
            if (mClustersCenter.size() == mHeadNumber) break;
        }
    }
    mClusteringTime = stat.usecsTo(PreciseTimer::currentTime());
}


/* ========================== TRY TO DO NEW POWERFUL CLUSTERING ========================== */
void Clustering3D::_clusterStarting(Statistics& stat)
{
    mClusters.clear();
    std::sort(mCloud->begin(), mCloud->end(), SortSwarmPointTexY());
    Cloud::iterator first, second;
    first  = mCloud->begin();
    second = mCloud->begin();
    for (unsigned i = 0; i < mCloud->size(); i++)
    {
        if ((*first).texCoor.y() != (*second).texCoor.y())
        {
            std::sort(first, second - 1, SortSwarmPointTexX());
            first = second;
        }
        second++;
    }

    PreciseTimer time = PreciseTimer::currentTime();
    this->_clustering(640, 480);
    stat.setTime("_clustering", time.usecsToNow());

    /* colour for clusters */
    CloudCluster::iterator it;
    mCloud->clear();
    int i = 1;
    vector<CloudCluster *>::iterator it2;
    for (it2 = mpClusters.begin(); it2 < mpClusters.end(); it2++)
    {
        for (it = (*it2)->begin(); it < (*it2)->end(); it++)
        {
            (*it).cluster = i;
            mCloud->push_back(*it);
        }
        i++;
    }
}

void Clustering3D::clusterStarting(int h, int w)
{
    mClusters.clear();
    mClustersCenter.clear();
    mClustersTexCenter.clear();
    mClustersFlow.clear();
    mClusterSize.clear();
    mCluster6DSize.clear();
    mHeadSize.clear();

    this->clustering(w, h, 7, 7, 70);

    vector<CloudCluster *>::iterator itCluster;
    for (itCluster = mpClusters.begin(); itCluster < mpClusters.end(); itCluster++)
    {
        // sort all points in each cluster
        std::sort((*itCluster)->begin(), (*itCluster)->end(), SortSwarmPointY());
    }

    // sort clusters by top y point
    std::sort(mpClusters.begin(), mpClusters.end(), CloudClusterSort());

    // get good clusters
    CloudCluster tmp;
    double height = 0;
    CloudCluster::iterator it;
    for (itCluster = mpClusters.begin(); itCluster < mpClusters.end(); itCluster++)
    {
        tmp.clear();

        height = ((*itCluster)->begin())->point.y();

        for (it = (*itCluster)->begin(); it < (*itCluster)->end(); it++)
        {
            tmp.push_back(*it);
            if ( (*it).point.y() - height > mDepth && tmp.size() > mSize )
            {
                mClusters.push_back(tmp);
                break;
            }
        }
    }

    vector<CloudCluster>::iterator it2;
    for (it2 = mClusters.begin(); it2 < mClusters.end(); it2++)
    {
        it2->getStat();
        mClustersTexCenter.push_back(it2->mClusterInfo.texCoor);
        mClustersCenter.push_back(it2->mClusterInfo.point);
        mClustersFlow.push_back(it2->mClusterInfo.speed);
        mClusterSize.push_back((int)it2->size());
        mCluster6DSize.push_back(it2->m6Dpoints);
        vector<double> tmpVect = it2->mEllipse.mValues;
        double forMax = 0;
        for (unsigned i = 0; i < tmpVect.size(); i++)
        {
            forMax = forMax < tmpVect[i] ? tmpVect[i] : forMax;
        }
        mHeadSize.push_back(forMax);
    }
}

void Clustering3D::clustering(int sizeW, int sizeH, int zoneW, int zoneH, int zoneDepth)
{
    Cloud *cloudTmp = new Cloud(*mCloud);
    Buffer3d map(sizeW, sizeH, cloudTmp);
    mMarkup = new AbstractBuffer<CloudCluster *>(map.h, map.w);
    mpClusters.clear();

    int dx = zoneW;
    int dy = zoneH;
    int x, y;
    double depth;

    for(int i = 0; i < map.h; i++)
    {
        for(int j = 0; j < map.w; j++)
        {
            if (!map.isElementKnown(i,j))
                continue;

            bool addingPending = true;
            for (int k = -dy; k <= 0; k++)
            {
                for (int l = -dx; l <= dx; l++)
                {

                    if (k == 0 && l == 0)
                        break;

                    x = j + l;
                    y = i + k;

                    if (!map.isValidCoord(y, x))
                        continue;

                    if (mMarkup->element(y,x) == NULL)
                        continue;

                    depth = fabs(map.element(i,j)->point.z() - map.element(y,x)->point.z());
                    if ( depth > zoneDepth )
                        continue;

                    if (addingPending)
                    {
                        mMarkup->element(y,x)->push_back(*(map.element(i,j)));
                        mMarkup->element(i,j) = mMarkup->element(y,x);
                        addingPending = false;
                    }
                    else
                    {
                        mMarkup->element(i,j)->markMerge(mMarkup->element(y,x));
                    }
                }
            }

            if (addingPending)
            {
                CloudCluster *newSegment = new CloudCluster();
                newSegment->push_back(*(map.element(i,j)));
                mMarkup->element(i,j) = newSegment;
                mpClusters.push_back(newSegment);
            }
        }
    }

    /*Cycle finished. Now we will need to merge segments and delete segments that are not of the need */
    vector<CloudCluster *>::iterator it;
    for (it = mpClusters.begin(); it < mpClusters.end(); it++)
    {
        if ( (*it) != NULL)
            (*it)->dadify();
    }

    for(int i = 0; i < map.h; i++)
    {
        for(int j = 0; j < map.w; j++)
        {
            if (mMarkup->element(i,j) == NULL)
                continue;

            if (!mMarkup->element(i,j)->isTop())
            {
                mMarkup->element(i,j)->getTopFather()->push_back(*(map.element(i,j)));
            }
        }
    }

    for (it = mpClusters.begin(); it < mpClusters.end(); it++)
    {
        if ( !(*it)->isTop())
            delete_safe(*it);
    }

    /*Delete NULL entries in the vector and shorten the vector*/
    mpClusters.erase(
            remove_if(
                    mpClusters.begin(),
                    mpClusters.end(),
                    bind2nd(equal_to<void *>(), (void *)NULL)
            ),
            mpClusters.end());
}



void Clustering3D::_clustering(int sizeW, int sizeH)
{
    mMarkup = new AbstractBuffer<CloudCluster *>(sizeH, sizeW);
    mpClusters.clear();

    int y = 0;
    int x = 0;
    int k = 0;
    int r = 0;
    bool addingPending;

    for(unsigned i = 0; i < mCloud->size(); i++)
    {
        k = (*mCloud)[i].texCoor.y();
        r = (*mCloud)[i].texCoor.x();

        addingPending = true;

        for (int j = 0; j < 100; j++)
        {
            if ((int)i - j < 0)
                continue;

            if ( (*mCloud)[i].distTo((*mCloud)[i-j]) > mRadius )
                continue;

            y = (*mCloud)[i - j].texCoor.y();
            x = (*mCloud)[i - j].texCoor.x();

            if (mMarkup->element(y, x) == NULL)
                continue;

            if (addingPending)
            {
                mMarkup->element(y,x)->push_back((*mCloud)[i]);
                mMarkup->element(k,r) = mMarkup->element(y,x);
                addingPending = false;
            }
            else
            {
                mMarkup->element(k,r)->markMerge(mMarkup->element(y,x));
            }
        }

        if (addingPending)
        {
            CloudCluster *newSegment = new CloudCluster();
            newSegment->push_back((*mCloud)[i]);
            mMarkup->element(k,r) = newSegment;
            mpClusters.push_back(newSegment);
        }
     }

    /*Cycle finished. Now we will need to merge segments and delete segments that are not of the need */
    for (unsigned i = 0; i < mpClusters.size(); i++)
    {
        if ( mpClusters[i] != NULL)
            mpClusters[i]->dadify();
    }

    for(unsigned i = 0; i < mCloud->size(); i++)
    {
        int y = (*mCloud)[i].texCoor.y();
        int x = (*mCloud)[i].texCoor.x();
        if (!mMarkup->element(y,x)->isTop())
        {
            mMarkup->element(y,x)->getTopFather()->push_back((*mCloud)[i]);
        }
    }

    for (unsigned i = 0; i < mpClusters.size(); i++)
    {
        if ( !mpClusters[i]->isTop())
            delete_safe(mpClusters[i]);
    }

    /*Delete NULL entries in the vector and shorten the vector*/
    mpClusters.erase(
            remove_if(
                    mpClusters.begin(),
                    mpClusters.end(),
                    bind2nd(equal_to<void *>(), (void *)NULL)
            ),
            mpClusters.end());
    return;
}


} //namespace corecvs
