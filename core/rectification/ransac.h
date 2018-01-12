#ifndef RANSAC_H_
#define RANSAC_H_
/**
 * \file ransac.h
 * \brief a header that holds generic RANSAC implementation
 *
 * \date Jul 2, 2011
 * \author alexander
 */

#include <random>
#include <vector>
#include <algorithm>

#include "core/utils/global.h"
#include "core/xml/generated/ransacParameters.h"

namespace corecvs {

using std::vector;
using std::find;

/**
 *  This template class is used to implement classic RANSAC algorithm is a generic form
 *
 *
 *
 **/
#if 0
class RansacParameters {
public:
    int iterationsNumber;
    double inliersPercent;
    double inlierThreshold;
};
#endif

template<typename SampleType, typename ModelType>
class Ransac : public RansacParameters {
public:
    vector<SampleType *> *data;
    int dataLen;

    vector<SampleType *> samples;
    int sampleNumber;

    int iteration;
    vector<SampleType *> bestSamples;
    ModelType bestModel;
    int bestInliers;

    bool trace = false;
    //bool useMedian = false;

    /* */
    std::mt19937 rng;

    Ransac(int _sampleNumber, const RansacParameters &params = RansacParameters())
        : RansacParameters(params)
        , sampleNumber(_sampleNumber)
    {
        samples.reserve(sampleNumber);
        rng.seed();
    }

    virtual void randomSelect()
    {        
        if (data->empty())
        {
            SYNC_PRINT(("Ransac::randomSelect(): Somehow we were called with empty data\n"));
            return;
        }
        std::uniform_int_distribution<int> uniform(0, (int)data->size() - 1);
        samples.clear();
        for (int i = 0; i < sampleNumber; i++)
        {
            unsigned index;
            SampleType *element = NULL;
            do {
                index = uniform(rng);
                element = data->at(index);

                if (find(samples.begin(), samples.end(), element) == samples.end())
                    break;
            } while (true);

            samples.push_back(element);
        }
    }

    ModelType getModelRansac()
    {
        bestInliers = 0;
        iteration = 0;
        std::vector<double> costs;
        costs.reserve(data->size());

        double old_median = std::numeric_limits<double>::max();


        if (trace) {
             //SYNC_PRINT(("getModelRansac(): called with threshold %lf\n", ge));
             std::cout << "getModelRansacMultimodel(): called with\n" << *this << std::endl;
        }

        while (true)
        {
            randomSelect();
            vector<ModelType> models = ModelType::getModels(samples);


            for (ModelType &model : models)
            {
                int inliers = 0;
                costs.clear();


                for (size_t i = 0; i < data->size(); i++)
                {
                    if (model.fits(*(data->at(i)), inlierThreshold()))
                        inliers++;
                    costs.push_back(model.getCost(*(data->at(i))));
                }

                auto it = costs.begin() + (costs.size() / 2);
                std::nth_element(costs.begin(), it, costs.end());
                double median = *it;

                if (inliers > bestInliers)
                {
                    bestSamples = samples;
                    bestInliers = inliers;
                    bestModel = model;
                }

                if (useMedian() && median < old_median)
                {
                    bestSamples = samples;
                    bestInliers = inliers;
                    bestModel = model;
                    old_median = median;
                }

                if (trace && !useMedian())
                            SYNC_PRINT(("iteration %d : %d inliers (max so far %d) out of %d (%lf%%)\n",
                                       iteration, inliers, bestInliers, (int)data->size(), (double)100.0 * bestInliers / data->size() ));

                if (trace && useMedian())
                            SYNC_PRINT(("iteration %d : %d inliers (max so far %d) (median %lf %lf) out of %d (%lf%%)\n",
                                   iteration, inliers, bestInliers, median, old_median, (int)data->size(), (double)100.0 * bestInliers / data->size() ));

                if ((bestInliers >  data->size() * inliersPercent() / 100.0 && iteration >= iterationsNumber() * 0.3 )
                    ||
                    iteration >= iterationsNumber() )
                {
                    if (trace) {
                        std::cout << "Fininshing:" << std::endl;
                        std::cout << "BestInliers:" << bestInliers << std::endl;
                    }
                    return bestModel;
                }
            }
            iteration++;
        }
    }

    ~Ransac()
    {}
};

} //namespace corecvs

#endif  //RANSAC_H_
