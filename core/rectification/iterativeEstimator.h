#ifndef ITERATIVEESTIMATOR_H_
#define ITERATIVEESTIMATOR_H_
/**
 * \file iterativeEstimator.h
 * \brief Add Comment Here
 *
 * \date Oct 26, 2011
 * \author alexander
 */

#include "essentialEstimator.h"
namespace corecvs {

class IterativeEstimator
{
public:
    int maxIterations;
    double initialSigma;
    double sigmaFactor;
    EssentialEstimator::OptimisationMethod method;

    IterativeEstimator(
            int _maxIterations,
            double _initialSigma,
            double _sigmaFactor,
            EssentialEstimator::OptimisationMethod _method = EssentialEstimator::METHOD_DEFAULT
    ) :
        maxIterations(_maxIterations),
        initialSigma(_initialSigma),
        sigmaFactor(_sigmaFactor),
        method(_method)
    {}

    EssentialMatrix getEssential   (const vector<Correspondance *> &samples);

    virtual ~IterativeEstimator();
};


} //namespace corecvs
#endif /* ITERATIVEESTIMATOR_H_ */

