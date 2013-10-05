/**
 * \file vjPattern.cpp
 * \brief This file hold implementation of function for Viola-Jones optimised patterns.
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#include "global.h"

#include "vjPattern.h"
#include "tbbWrapper.h"

namespace corecvs {

VJPattern::VJPattern()
{
}

VJPattern::~VJPattern()
{
}


class ParallelGetBestClassifier
{
    const AdaBoostInput<VJInputImage> &inputs;
    VJSimpleClassifierGenerator *gen;

public:

    ParallelGetBestClassifier (
            const AdaBoostInput<VJInputImage> &_inputs,
            VJSimpleClassifierGenerator *_gen) :
        inputs(_inputs),
        gen(_gen)
    {}


    void operator()( const BlockedRange<unsigned>& r ) const
    {
        for(unsigned iteration = r.begin(); iteration  < r.end(); iteration++)
        {
            vector<VJResult> results;
            results.reserve(inputs.size());

            /*Choosing random VJ pattern form one of the generators*/
            VJPattern *currentPattern = gen->generators[rand() % gen->generators.size()]->pattern();

            results.clear();
            for (unsigned long i = 0; i < (unsigned long)inputs.size(); i++)
            {
                const AdaBoostInputData<VJInputImage> *data = &inputs[i];
                G12IntegralBuffer *integral = data->x.integral;
                int value = currentPattern->applyToPoint(integral,0,0);
                VJResult result = VJResult(value, data);
                results.push_back(result);
            }

            sort(results.begin(), results.end());

            /**
             *  Now we should find the best threshold start with the one lower
             *  then the lowest result.
             *
             **/

            int threshold = results[0].value - 1;

            /**
             * Calculate the epsilon of the error assuming all the
             * results are predicted as faces
             **/
            double startEpsilon = 0;
            for (unsigned i = 0; i < results.size(); i++)
            {

                if (! results[i].inputData->y)
                    startEpsilon += results[i].inputData->weight;
            }

            double minEpsilon = startEpsilon;
            double currentEpsilon = startEpsilon;

            /**
             * Now move threshold over the results from the list and
             * see how epsilon changes
             **/
            for (unsigned i = 0; i < results.size(); i++)
            {
                VJResult *result = &(results[i]);

                /**
                 * If we step over the real face we should be punished
                 * otherwise rewarded
                 **/
                if (result->inputData->y)
                    currentEpsilon += result->inputData->weight;
                else
                    currentEpsilon -= result->inputData->weight;

                if (currentEpsilon < minEpsilon)
                {
                    minEpsilon = currentEpsilon;
                    if (i < (results.size() - 1))
                    {
                        // threshold = results[i].result;
                        threshold = (results[i].value + results[i + 1].value) / 2;
                    }
                    else
                    {
                        threshold = results[i].value;
                    }
                }
            }
            /* TODO: Replace this with reduce operation */
#ifdef WITH_TBB
            spin_mutex::scoped_lock lock;
            lock.acquire(gen->VJSimpleClassifierGeneratorMutex);
            {
#endif
                if (minEpsilon < gen->minEpsilonGlobal)
                {
                    gen->minEpsilonGlobal = minEpsilon;
                    VJPattern *tmp = gen->bestPattern;
                    gen->bestPattern = currentPattern;
                    currentPattern = tmp;
                    gen->bestZ = threshold;
                }

                gen->globalCount++;
                if (gen->globalCount % 500 == 0)
                {
                    printf("Pattern attempt %d\n", gen->globalCount);
                    fflush(stdout);
                }
#ifdef WITH_TBB
            }
            lock.release();
#endif

            if (currentPattern != NULL)
                delete currentPattern;
        }
    }
};

VJSimpleClassifier *VJSimpleClassifierGenerator::getBestClassifier(const AdaBoostInput<VJInputImage> &inputs, double *cost)
{
    minEpsilonGlobal = numeric_limits<double>::max();
    bestPattern = NULL;
    bestZ = 0;

    /**
     * This counter is incremented by 1 synchronously
     *
     */
    globalCount = 0;

    parallelable_for<unsigned, ParallelGetBestClassifier>(0,patternNumber, ParallelGetBestClassifier(inputs, this));
    *cost = minEpsilonGlobal;
    return new VJSimpleClassifier(bestPattern, bestZ, 0, 0, h, w);
}

} //namespace corecvs

