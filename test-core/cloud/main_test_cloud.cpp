/**
 * \file main_test_cloud.cpp
 * \brief This is the main file for the test cloud 
 *
 * \date февр. 20, 2013
 * \author alex
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "cloud.h"
#include "preciseTimer.h"
#include "ellipticalApproximation.h"
#include "clustering3d.h"
#include "buffer3d.h"

TEST(CloudTest, main)
{
//    srand(int64_t(0));

   // vector<CloudCluster> mClusters;

   // mClusters.push_back(CloudCluster());
   // mClusters.clear();

    CloudCluster *pCloud = new CloudCluster();
    SwarmPoint point;
    point.point = Vector3dd(0, 4, 0);
    pCloud->push_back(point);

    point.point = Vector3dd(0, 12, 0);
    pCloud->push_back(point);
//    point.point = Vector3dd(0, 1, 1);
//    pCloud->push_back(point);
//    point.point = Vector3dd(1, 1, 0);
//    pCloud->push_back(point);

//    point.texCoor = Vector2dd(10, 2);
//    pCloud->push_back(point);
//    point.texCoor = Vector2dd(11, 2);
//    pCloud->push_back(point);

//    point.texCoor = Vector2dd(8, 4);
//    pCloud->push_back(point);
//    point.texCoor = Vector2dd(7, 5);
//    pCloud->push_back(point);
//    point.texCoor = Vector2dd(8, 5);
//    pCloud->push_back(point);
//    point.texCoor = Vector2dd(9, 5);
//    pCloud->push_back(point);
//    point.texCoor = Vector2dd(8, 6);
//    pCloud->push_back(point);

    pCloud->getStat();

    for (vector<Vector3dd>::iterator it = pCloud->mEllipse.mAxes.begin(); it != pCloud->mEllipse.mAxes.end(); it++)
        cout << (*it) << endl;

    for (vector<double>::iterator it = pCloud->mEllipse.mValues.begin(); it != pCloud->mEllipse.mValues.end(); it++)

        cout << (*it) << endl;


//    Clustering3D clusterer(pCloud, 1, 1, 1, 1);
//    clusterer._clusterStarting();

//        int i = 0;
//        typename vector<CloudCluster *>::iterator it;
//        typename CloudCluster::iterator it2;
//        for (it = clusterer.mpClusters.begin(); it < clusterer.mpClusters.end(); it++)
//        {
//            for (it2 = (*it)->begin(); it2 < (*it)->end(); it2++)
//            {
//                cout << "Cluster " << i << " Point" << (*it2).texCoor << endl;
//            }
//            i++;
//        }


//    Buffer3d buffer(10, 10, pCloud);
//    buffer.element(5, 5)->speed = Vector3dd(0, 0, 0);

//    cout << pCloud->back().speed << endl;
    delete pCloud;
}
