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

#include "matrix33.h"
#include "correspondenceList.h"
namespace corecvs {





class RansacEstimator
{
public:
    unsigned trySize;
    unsigned maxIterations;
    double treshold;


    RansacEstimator(
            unsigned  _trySize = 9,
            unsigned _maxIterations = 1000,
            double _treshold = 1) :
        trySize(_trySize),
        maxIterations(_maxIterations),
        treshold(_treshold)
    {}

    Matrix33 getFundamentalRansac1(CorrespondenceList *list);
    Matrix33 getEssentialRansac1  (CorrespondenceList *list);

    Matrix33 getFundamentalRansac(vector<Correspondence *> *data);
    Matrix33 getEssentialRansac  (vector<Correspondence *> *data);
};



} //namespace corecvs
#endif  //RANSACESTIMATOR_H_



