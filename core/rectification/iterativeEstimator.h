#ifndef ITERATIVEESTIMATOR_H_
#define ITERATIVEESTIMATOR_H_
/**
 * \file iterativeEstimator.h
 * \brief Add Comment Here
 *
 * \date Oct 26, 2011
 * \author alexander
 */

#include "core/rectification/essentialEstimator.h"
#include "core/xml/generated/iterativeEstimateParameters.h"

#include "core/camerafixture/fixtureScene.h"

namespace corecvs {

class IterativeEstimator
{
public:

    IterativeEstimateParameters params;
    EssentialEstimator::OptimisationMethod method;

    EssentialMatrix initalGuess;

    IterativeEstimator(EssentialEstimator::OptimisationMethod _method = EssentialEstimator::METHOD_DEFAULT) : method(_method)
    {}

    IterativeEstimator(
            int _maxIterations,
            double _initialSigma,
            double _sigmaFactor,
            EssentialEstimator::OptimisationMethod _method = EssentialEstimator::METHOD_DEFAULT
    ) :
        method(_method)
    {
        params.setIterationsNumber(_maxIterations);
        params.setInitialSigma(_initialSigma);
        params.setSigmaFactor(_sigmaFactor);
        params.setUseInitial(false);

    }

    EssentialMatrix getEssential   (const vector<Correspondence *> &samples);

    virtual ~IterativeEstimator();
};


class IterativeEstimatorScene
{
public:
    IterativeEstimateParameters params; /**< For  convinence threshold is in px - relative to focal**/
    bool trace = false;

    EssentialDecomposition getEssentialIterative(FixtureScene *scene, FixtureCamera *cam1, FixtureCamera *cam2);
    //EssentialDecomposition getEssentialRansac(FixtureScene *scene, FixtureCamera *cam1, FixtureCamera *cam2);

};



} //namespace corecvs
#endif /* ITERATIVEESTIMATOR_H_ */

