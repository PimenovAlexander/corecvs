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
#include "correspondanceList.h"
namespace corecvs {





class RansacEstimator
{
public:
    unsigned trySize;
    unsigned maxIterations;
    double treshold;


    RansacEstimator(
            unsigned  _trySize,
            unsigned _maxIterations,
            double _treshold ) :
        trySize(_trySize),
        maxIterations(_maxIterations),
        treshold(_treshold)
    {};

    Matrix33 getFundamentalRansac1(CorrespondanceList *list);
    Matrix33 getEssentialRansac1  (CorrespondanceList *list);

    Matrix33 getFundamentalRansac(vector<Correspondance *> *data);
    Matrix33 getEssentialRansac  (vector<Correspondance *> *data);
};



} //namespace corecvs
#endif  //RANSACESTIMATOR_H_



