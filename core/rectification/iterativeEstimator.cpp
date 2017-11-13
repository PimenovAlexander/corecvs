/**
 * \file iterativeEstimator.cpp
 * \brief Add Comment Here
 *
 * \date Oct 26, 2011
 * \author alexander
 */

#include "core/utils/global.h"
#include "core/utils/log.h"

#include "core/rectification/iterativeEstimator.h"
namespace corecvs {


EssentialMatrix IterativeEstimator::getEssential (const vector<Correspondence *> &samples)
{
    EssentialMatrix model;
    int iteration;
    vector<Correspondence *> workingSamples = samples;

    EssentialEstimator::CostFunction7to1 initialCost(&samples);

    L_INFO_P("Starting iterative rectification...");

    double sigma = initialSigma;
    for (iteration = 0; iteration < maxIterations; iteration++)
    {
        EssentialEstimator estimator;
        if (workingSamples.size() < 9)
        {
            cout << "Too few inputs to continue:" << workingSamples.size() << endl;
            break;
        }
        model = estimator.getEssential(workingSamples, method);
        EssentialEstimator::CostFunction7to1 cost(&workingSamples);

        int passed = 0;
        int rejected = 0;

        vector<Correspondence *> passedSamples;
        for (unsigned i = 0; i < workingSamples.size(); i++)
        {
            Correspondence *corr = workingSamples[i];
            double value = model.epipolarDistance(*corr);
            if (value > sigma)
            {
                rejected++;
                corr->value = iteration;
            } else {
                passed++;
                passedSamples.push_back(corr);
            }
        }

        double errorPerPoint = sqrt(cost.getCost(model) / workingSamples.size());
        double errorPerPointInit = sqrt(initialCost.getCost(model) / samples.size());

#ifdef TRACE
        cout << "Iteration: " << iteration << endl;
        cout << "Error:" << cost.getCost(model) << endl;
        cout << "Sq per point:" << (cost.getCost(model) / workingSamples.size()) << endl;
        cout << "Error per point:" << errorPerPoint << endl;

        cout << " Total:" << workingSamples.size() << endl;
        cout << "   Passed  :" << passed   << endl;
        cout << "   Rejected:" << rejected << endl;
#endif
        //printf("Iteration %4d: %6d %6d %6d %lf %lf \n", iteration, passed + rejected, passed, rejected, errorPerPoint, errorPerPointInit);
        // Make real callback
        L_INFO_P("Iteration %4d: %6d %6d %6d %lf %lf"
                 , iteration, passed + rejected, passed, rejected, errorPerPoint, errorPerPointInit);

        workingSamples = passedSamples;
        sigma *= sigmaFactor;
    }

    for (unsigned i = 0; i < samples.size(); i++)
    {
        samples[i]->flags |= Correspondence::FLAG_FAILER;
    }

    for (unsigned i = 0; i < workingSamples.size(); i++)
    {
        workingSamples[i]->flags &= ~Correspondence::FLAG_FAILER;
        workingSamples[i]->flags |=  Correspondence::FLAG_PASSER;
        workingSamples[i]->value = iteration;
    }

    L_INFO_P("Iterative rectification finished");

    return model;
}


IterativeEstimator::~IterativeEstimator()
{
    // TODO Auto-generated destructor stub
}


} //namespace corecvs

