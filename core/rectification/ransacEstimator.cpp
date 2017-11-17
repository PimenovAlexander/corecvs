/**
 * \file ransacEstimator.cpp
 * \brief Add Comment Here
 *
 * \date Nov 14, 2011
 * \author alexander
 */

#include <vector>

#include "core/utils/global.h"

#include "core/rectification/ransacEstimator.h"
#include "core/rectification/essentialMatrix.h"
#include "core/rectification/essentialEstimator.h"
#include "core/rectification/ransac.h"
#include "core/rectification/correspondenceList.h"
namespace corecvs {




class Model8Point {
public:
    EssentialMatrix model;

    Model8Point()
        : model(Matrix33(1.0))
    {

    }

    Model8Point(const vector<Correspondence *> &samples)
        : model(Matrix33(1.0))
    {
        EssentialEstimator estimator;
        model = estimator.getEssential(samples, EssentialEstimator::METHOD_SVD_LSE);
    }

    double getCost(const Correspondence &data)
    {
        return (model.epipolarDistance(data));
    }

    bool fits(const Correspondence &data, double fitTreshold)
    {
        return (model.epipolarDistance(data) < fitTreshold);
    }
};

class Model5Point {
public:
    EssentialMatrix model;

    Model5Point(const EssentialMatrix &m) :
        model(m)
    {
    }

    Model5Point()
    {
        model = Matrix33(1.0);
    }

    static vector<Model5Point> getModels(const vector<Correspondence *> &samples)
    {
        EssentialEstimator estimator;
        vector<EssentialMatrix> ms = estimator.getEssential5point(samples);
        vector<Model5Point> result;
        result.reserve(ms.size());
        for ( EssentialMatrix &m: ms)
        {
            result.push_back(m);
        }
        return result;

    }

    double getCost(const Correspondence &data)
    {
        return (model.epipolarDistance(data));
    }

    bool fits(const Correspondence &data, double fitTreshold)
    {
        return (model.epipolarDistance(data) < fitTreshold);
    }
};


class ModelFundamental8Point : public Model8Point
{
public:
    ModelFundamental8Point() : Model8Point() {}
    ModelFundamental8Point(const vector<Correspondence *> &samples) :
                Model8Point(samples)
    {
        model.assertRank2();
    }
};

class ModelEssential8Point : public Model8Point
{
public:
    ModelEssential8Point() : Model8Point() {}
    ModelEssential8Point(const vector<Correspondence *> &samples) :
                Model8Point(samples)
    {
        model.projectToEssential();
    }
};


Matrix33 RansacEstimator::getFundamentalRansac1(CorrespondenceList *list)
{
    vector<Correspondence *> data;
    data.reserve(list->size());
    for(unsigned i = 0; i < list->size(); i++)
        data.push_back(&(list->at(i)));

    return getFundamentalRansac(&data);
}

Matrix33 RansacEstimator::getFundamentalRansac(vector<Correspondence *> *data)
{
    Ransac<Correspondence, ModelFundamental8Point>
        ransac(trySize, ransacParams);

    ransac.data = data;

    ModelFundamental8Point result = ransac.getModelRansac();
    for (unsigned i = 0; i < data->size(); i++)
    {
        bool fits = result.fits(*(data->operator[](i)), ransacParams.inlierThreshold());
        data->operator[](i)->flags |= (fits ? Correspondence::FLAG_PASSER : Correspondence::FLAG_FAILER);
    }
    for (unsigned i = 0; i < ransac.bestSamples.size(); i++)
    {
        ransac.bestSamples[i]->flags  |= Correspondence::FLAG_IS_BASED_ON;
    }

    return result.model;
}



Matrix33 RansacEstimator::getEssentialRansac1(CorrespondenceList *list)
{

    vector<Correspondence *> data;
    data.reserve(list->size());
    for(unsigned i = 0; i < list->size(); i++)
        data.push_back(&(list->at(i)));

    return getEssentialRansac(&data);
}

Matrix33 RansacEstimator::getEssentialRansac(vector<Correspondence *> *data)
{
    Ransac<Correspondence, ModelEssential8Point>
        ransac(trySize, ransacParams);

    ransac.data = data;

    ModelEssential8Point result = ransac.getModelRansac();
    for (unsigned i = 0; i < data->size(); i++)
    {
        bool fits = result.fits(*(data->operator[](i)), ransacParams.inlierThreshold());
        data->operator[](i)->flags |= (fits ? Correspondence::FLAG_PASSER : Correspondence::FLAG_FAILER);
    }

    for (unsigned i = 0; i < ransac.bestSamples.size(); i++)
    {
        ransac.bestSamples[i]->flags  |= Correspondence::FLAG_IS_BASED_ON;
    }

    return result.model;
}


#if 0
EssentialDecomposition RansacEstimatorScene::getEssentialRansac(FixtureScene *scene, FixtureCamera *camera1, FixtureCamera *camera2)
{
    vector<Correspondence > data;
    vector<Correspondence *> dataPtr;

    //CameraModel cam1world = camera1->getWorldCameraModel();
    //CameraModel cam2world = camera2->getWorldCameraModel();


    for(SceneFeaturePoint *point: scene->featurePoints())
    {
        SceneObservation *obs1 = point->getObservation(camera1);
        SceneObservation *obs2 = point->getObservation(camera2);

        if (obs1 != NULL && obs2 != NULL)
        {
            Correspondence c;
            Vector2dd startPixel = obs1->getUndist();
            Vector2dd endPixel   = obs2->getUndist();

            c.start = camera1->intrinsics.reverse(startPixel).xy();
            c.end   = camera2->intrinsics.reverse(endPixel  ).xy();

            data.push_back(c);
            dataPtr.push_back(&data.back());
        }
    }

    EssentialMatrix model;

    double thresholdScaler = camera1->intrinsics.fx();
    RansacParameters scaledParams = params;
    scaledParams.setInlierThreshold(scaledParams.inlierThreshold() / thresholdScaler);


#if 1
    Ransac<Correspondence, Model5Point>  ransac(5, params);
    ransac.rng.seed(1337);
    ransac.trace = trace;
    ransac.data = &dataPtr;
    Model5Point result = ransac.getModelRansacMultimodel();    
    model = result.model;
#else
    Ransac<Correspondence, Model8Point>  ransac(8, scaledParams);
    ransac.rng.seed(1337);
    ransac.trace = trace;
    ransac.data = &dataPtr;
    Model8Point result = ransac.getModelRansac();
    model = result.model;
#endif

    cout << "Model as a result" << model << endl;

    int count = 0;
    for(SceneFeaturePoint *point: scene->featurePoints())
    {
        SceneObservation *obs1 = point->getObservation(camera1);
        SceneObservation *obs2 = point->getObservation(camera2);

        if (obs1 != NULL && obs2 != NULL)
        {
//            Correspondence::CorrespondenceFlags flag = (Correspondence::CorrespondenceFlags)data[count].flags;

            double cost = result.getCost(data[count]);

            Vector2dd accuracy(cost * thresholdScaler / params.inlierThreshold(), cost * thresholdScaler);

            obs1->accuracy = accuracy;
            obs2->accuracy = accuracy;
            count++;
        }
    }


    EssentialDecomposition variants[4];
    EssentialDecomposition dec = model.decompose(&dataPtr, variants);

    return dec;
}
#endif


} //namespace corecvs

