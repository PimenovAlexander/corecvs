/**
 * \file ransacEstimator.cpp
 * \brief Add Comment Here
 *
 * \date Nov 14, 2011
 * \author alexander
 */

#include <vector>

#include "utils/global.h"

#include "rectification/ransacEstimator.h"
#include "rectification/essentialMatrix.h"
#include "rectification/essentialEstimator.h"
#include "rectification/ransac.h"
#include "buffers/correspondenceList.h"
namespace corecvs {


template<
        int subestimatorMethod = EssentialEstimator::METHOD_SVD_LSE,
        int _defaultSamples    = EssentialEstimator::defaultSamples(subestimatorMethod),
        int enforceFundamental = 0
        >
class EssentialEstimatorModelRansac
{
public:
    static const int defaultSamples = _defaultSamples;
    typedef Correspondence                   SampleType;
    typedef EssentialEstimatorModelRansac    ModelType;


    EssentialMatrix model;

    EssentialEstimatorModelRansac(const EssentialMatrix &model = EssentialMatrix() )
        : model(model)
    {}

    EssentialEstimatorModelRansac(const vector<Correspondence *> &samples)
    {
        EssentialEstimator estimator;
        model = estimator.getEssential(samples, (EssentialEstimator::OptimisationMethod)subestimatorMethod);
    }

    static vector<EssentialEstimatorModelRansac> getModels(const vector<Correspondence *> &samples, EssentialEstimatorModelRansac * /*context*/ = NULL)
    {
        EssentialEstimator estimator;
        vector<EssentialMatrix> ms = estimator.getEssentials(samples, (EssentialEstimator::OptimisationMethod)subestimatorMethod);
        vector<EssentialEstimatorModelRansac> result;
        result.reserve(ms.size());
        for ( EssentialMatrix &m: ms)
        {
            result.push_back(m);
        }
        return result;

    }

    double getCost(const Correspondence &data)
    {
        return std::abs(model.epipolarDistance(data));
    }

    bool fits(const Correspondence &data, double fitTreshold, double *costP = NULL)
    {
        double cost = getCost(data);
        if (costP != NULL) *costP = cost;
        return (cost < fitTreshold);
    }
};


typedef EssentialEstimatorModelRansac<EssentialEstimator::METHOD_SVD_LSE> Model8Point;
typedef EssentialEstimatorModelRansac<EssentialEstimator::METHOD_5_POINT> Model5Point;
typedef EssentialEstimatorModelRansac<EssentialEstimator::METHOD_7_POINT> Model7Point;



class ModelFundamental8Point : public Model8Point
{
public:
    typedef Correspondence           SampleType;
    typedef ModelFundamental8Point   ModelType;


    ModelFundamental8Point() : Model8Point() {}
    ModelFundamental8Point(const vector<Correspondence *> &samples) :
                Model8Point(samples)
    {
        model.assertRank2();
    }

    static vector<ModelFundamental8Point> getModels(const vector<Correspondence *> &samples, ModelFundamental8Point* /*context*/ = NULL)
    {
        vector<ModelFundamental8Point> result;
        result.push_back(ModelFundamental8Point(samples));
        return result;
    }

};

class ModelEssential8Point : public Model8Point
{
public:
    typedef Correspondence         SampleType;
    typedef ModelEssential8Point   ModelType;

    ModelEssential8Point() : Model8Point() {}
    ModelEssential8Point(const vector<Correspondence *> &samples) :
                Model8Point(samples)
    {
        model.projectToEssential();
    }

    static vector<ModelEssential8Point> getModels(const vector<Correspondence *> &samples, ModelEssential8Point * /*context*/ = NULL)
    {
        vector<ModelEssential8Point> result;
        result.push_back(ModelEssential8Point(samples));
        return result;
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
    Ransac<ModelFundamental8Point> ransac(trySize, ransacParams);

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


template<class Subestimator>
Matrix33 RansacEstimator::getEssentialRansacS(vector<Correspondence *> *data)
{
    int realTrySize = (trySize <= 0) ? Subestimator::defaultSamples : trySize;
    Ransac<Subestimator> ransac(realTrySize, ransacParams);

    ransac.data = data;
    ransac.trace = trace;

    Subestimator result = ransac.getModelRansacParallel();
    int fitcount = 0;
    for (unsigned i = 0; i < data->size(); i++)
    {
        bool fits = result.fits(*(data->operator[](i)), ransacParams.inlierThreshold());
        data->operator[](i)->flags |= (fits ? Correspondence::FLAG_PASSER : Correspondence::FLAG_FAILER);

        if (fits) fitcount++;
    }

    for (unsigned i = 0; i < ransac.bestSamples.size(); i++)
    {
        ransac.bestSamples[i]->flags  |= Correspondence::FLAG_IS_BASED_ON;
    }

    fitPercent = 100.0 * fitcount / data->size();
    if (trace) SYNC_PRINT(("RansacEstimator::getEssentialRansac() fits %2.2lf%%\n", fitPercent));
    return result.model;
}

Matrix33 RansacEstimator::getEssentialRansac(vector<Correspondence *> *data)
{
    if (algorithm == 0) {
        if (trace) SYNC_PRINT(("RansacEstimator::getEssentialRansac(%d) : ModelEssential8Point", (int)data->size()));
        return getEssentialRansacS<ModelEssential8Point>(data);
    }

    if (algorithm == 1) {
        if (trace) SYNC_PRINT(("RansacEstimator::getEssentialRansac(%d) : Model5Point", (int)data->size()));
        return getEssentialRansacS<Model5Point>(data);
    }

    if (algorithm == 2) {
        if (trace) SYNC_PRINT(("RansacEstimator::getEssentialRansac(%d) : Model7Point", (int)data->size()));
        return getEssentialRansacS<Model7Point>(data);
    }
    CORE_ASSERT_FAIL("unknown algorithm");
    return Matrix33::Identity();
}

EssentialDecomposition RansacEstimatorScene::getEssentialRansac(FixtureScene *scene, FixtureCamera *camera1, FixtureCamera *camera2)
{
    vector<Correspondence > data;
    vector<Correspondence *> dataPtr;

    size_t n = scene->featurePoints().size();
    data.reserve(n);
    dataPtr.reserve(n);

    //CameraModel cam1world = camera1->getWorldCameraModel();
    //CameraModel cam2world = camera2->getWorldCameraModel();

    for (SceneFeaturePoint *point : scene->featurePoints())
    {
        SceneObservation *obs1 = point->getObservation(camera1);
        SceneObservation *obs2 = point->getObservation(camera2);

        if (obs1 != NULL && obs2 != NULL)
        {
            Vector2dd startPixel = obs1->getUndist();
            Vector2dd endPixel   = obs2->getUndist();

            Vector2dd sta = camera1->intrinsics->reverse(startPixel).xy();
            Vector2dd end = camera2->intrinsics->reverse(endPixel  ).xy();

            data.emplace_back(Correspondence(sta, end));
            dataPtr.push_back(&data.back());
        }
    }

    EssentialMatrix model;

    // MEFIXASAP
    double thresholdScaler = 1000; //camera1->intrinsics.fx();
    RansacParameters scaledParams = params;
    scaledParams.setInlierThreshold(scaledParams.inlierThreshold() / thresholdScaler);


#if 1
    Ransac<Model5Point>  ransac(5, params);
    ransac.rng.seed(1337);
    ransac.trace = trace;
    ransac.data = &dataPtr;
    Model5Point result = ransac.getModelRansac();
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



} //namespace corecvs

