/**
 * \file ransacEstimator.cpp
 * \brief Add Comment Here
 *
 * \date Nov 14, 2011
 * \author alexander
 */

#include <vector>

#include "global.h"

#include "ransacEstimator.h"
#include "essentialMatrix.h"
#include "essentialEstimator.h"
#include "ransac.h"
#include "correspondanceList.h"
namespace corecvs {




class Model8Point {
public:
    EssentialMatrix model;

    Model8Point()
        : model(Matrix33(1.0))
    {

    }

    Model8Point(const vector<Correspondance *> &samples)
        : model(Matrix33(1.0))
    {
        EssentialEstimator estimator;
        model = estimator.getEssential(samples, EssentialEstimator::METHOD_SVD_LSE);
    }

    bool fits(const Correspondance &data, double fitTreshold)
    {
        return (model.epipolarDistance(data) < fitTreshold);
    }
};

class ModelFundamental8Point : public Model8Point
{
public:
    ModelFundamental8Point() : Model8Point() {};
    ModelFundamental8Point(const vector<Correspondance *> &samples) :
                Model8Point(samples)
    {
        model.assertRank2();
    };
};

class ModelEssential8Point : public Model8Point
{
public:
    ModelEssential8Point() : Model8Point() {};
    ModelEssential8Point(const vector<Correspondance *> &samples) :
                Model8Point(samples)
    {
        model.projectToEssential();
    };
};


Matrix33 RansacEstimator::getFundamentalRansac1(CorrespondanceList *list)
{
    vector<Correspondance *> data;
    data.reserve(list->size());
    for(unsigned i = 0; i < list->size(); i++)
        data.push_back(&(list->at(i)));

    return getFundamentalRansac(&data);
}

Matrix33 RansacEstimator::getFundamentalRansac(vector<Correspondance *> *data)
{
    Ransac<Correspondance, ModelFundamental8Point>
        ransac(trySize);

    ransac.data = data;
    ransac.inlierThreshold = treshold;
    ransac.inliersPercent = 1.0;
    ransac.iterationsNumber = maxIterations;

    ModelFundamental8Point result = ransac.getModelRansac();
    for (unsigned i = 0; i < data->size(); i++)
    {
        bool fits = result.fits(*(data->operator[](i)), treshold);
        data->operator[](i)->flags |= (fits ? Correspondance::FLAG_PASSER : Correspondance::FLAG_FAILER);
    }
    for (unsigned i = 0; i < ransac.bestSamples.size(); i++)
    {
        ransac.bestSamples[i]->flags  |= Correspondance::FLAG_IS_BASED_ON;
    }

    return result.model;
}



Matrix33 RansacEstimator::getEssentialRansac1(CorrespondanceList *list)
{

    vector<Correspondance *> data;
    data.reserve(list->size());
    for(unsigned i = 0; i < list->size(); i++)
        data.push_back(&(list->at(i)));

    return getEssentialRansac(&data);
}

Matrix33 RansacEstimator::getEssentialRansac(vector<Correspondance *> *data)
{
    Ransac<Correspondance, ModelEssential8Point>
        ransac(trySize);

    ransac.data = data;

    ransac.inlierThreshold = treshold;
    ransac.inliersPercent = 1.0;
    ransac.iterationsNumber = maxIterations;

    ModelEssential8Point result = ransac.getModelRansac();
    for (unsigned i = 0; i < data->size(); i++)
    {
        bool fits = result.fits(*(data->operator[](i)), treshold);
        data->operator[](i)->flags |= (fits ? Correspondance::FLAG_PASSER : Correspondance::FLAG_FAILER);
    }

    for (unsigned i = 0; i < ransac.bestSamples.size(); i++)
    {
        ransac.bestSamples[i]->flags  |= Correspondance::FLAG_IS_BASED_ON;
    }

    return result.model;
}


} //namespace corecvs

