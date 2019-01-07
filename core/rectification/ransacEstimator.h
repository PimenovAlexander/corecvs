/**
 * \file ransacEstimator.h
 * \brief a header for ransacEstimator.c
 *
 * \date Nov 14, 2011
 * \author alexander
 */
#ifndef RANSACESTIMATOR_H_
#define RANSACESTIMATOR_H_

#include <vector>

#include "core/math/matrix/matrix33.h"
#include "core/buffers/correspondenceList.h"
#include "core/rectification/ransac.h"
#include "core/camerafixture/fixtureScene.h"

namespace corecvs {

class RansacEstimator
{
public:
    int algorithm = 0;
    bool trace = false;
    int trySize;
    RansacParameters ransacParams;

    /*Addintional outputs*/
    double fitPercent = 0.0;

    RansacEstimator(
            int  _trySize, /* Negative values force default try size */
            unsigned _maxIterations,
            double _treshold ) :
        trySize(_trySize)
    {
        ransacParams.setIterationsNumber(_maxIterations);
        ransacParams.setInlierThreshold(_treshold);
    }



    Matrix33 getFundamentalRansac1(CorrespondenceList *list);
    Matrix33 getEssentialRansac1  (CorrespondenceList *list);

    Matrix33 getFundamentalRansac(vector<Correspondence *> *data);

    template <class Subestimator>
    Matrix33 getEssentialRansacS (vector<Correspondence *> *data);

    Matrix33 getEssentialRansac  (vector<Correspondence *> *data);    
};

class RansacEstimatorScene
{
public:
    RansacParameters params; /**< For  convinence threshold is in px - relative to focal**/
    bool trace = true;

    EssentialDecomposition getEssentialRansac(FixtureScene *scene, FixtureCamera *cam1, FixtureCamera *cam2);
    //EssentialDecomposition getEssentialRansac(FixtureScene *scene, FixtureCamera *cam1, FixtureCamera *cam2);

};



} //namespace corecvs
#endif  //RANSACESTIMATOR_H_



