/**
 * \file iterativeEstimator.cpp
 * \brief Add Comment Here
 *
 * \date Oct 26, 2011
 * \author alexander
 */

#include "utils/global.h"
#include "utils/log.h"

#include "rectification/iterativeEstimator.h"

namespace corecvs {

vector<Correspondence *> sieveCorrespondanceList(const vector<Correspondence *> &workingSamples, size_t maximum)
{
    vector<Correspondence *> passedSamples;
    std::mt19937 rng(100);
    passedSamples = workingSamples;

    unsigned hash = 0;

    if (maximum < passedSamples.size())
    {
        for (size_t j = 0; j < maximum; j++ )
        {
            std::uniform_int_distribution<unsigned> rint((unsigned)j, (unsigned)workingSamples.size() - 1);
            unsigned newPos = rint(rng);
            hash = hash ^ newPos;
            std::swap(passedSamples[j], passedSamples[newPos]);
        }
        passedSamples.resize(maximum);
    }
    /* Check that implementation is platform independant*/
    SYNC_PRINT(("sieveCorrespondanceList(): RNDHASH:%d\n", hash));
    return passedSamples;
}

vector<Correspondence *> filterCorrespondanceList(const vector<Correspondence *> &workingSamples
    , const EssentialMatrix &model, double sigma, int iteration)
{
    int passed = 0;
    int rejected = 0;

    vector<Correspondence *> passedSamples;

    // We don't expect to shrink vector to much. Time however is valuable
    passedSamples.reserve(workingSamples.size());

    for (unsigned i = 0; i < workingSamples.size(); i++)
    {
        // SYNC_PRINT(("Cost at sample %d (%d)\n", i, workingSamples.size()));
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

#ifdef TRACE
    cout << "Iteration: " << iteration << endl;
    cout << "Error:" << cost.getCost(model) << endl;
    cout << "Sq per point:" << (cost.getCost(model) / workingSamples.size()) << endl;
    cout << "Error per point:" << errorPerPoint << endl;

    cout << " Total:" << workingSamples.size() << endl;
    cout << "   Passed  :" << passed   << endl;
    cout << "   Rejected:" << rejected << endl;
#endif
    // Make real callback
    L_INFO_P("Iteration %4d: %6d %6d %6d"
             , iteration, passed + rejected, passed, rejected);

#ifdef DEEP_TRACE
    double errorPerPoint     = sqrt(cost.getCost(model) / workingSamples.size());
    double errorPerPointInit = sqrt(initialCost.getCost(model) / samples.size());

    L_INFO_P("  %lf %lf" , errorPerPoint, errorPerPointInit);
#endif

    return passedSamples;
}


EssentialMatrix IterativeEstimator::getEssential(const vector<Correspondence *> &samples)
{
    SYNC_PRINT(("IterativeEstimator::getEssential(vector<%u>)\n", (unsigned)samples.size()));

    EssentialMatrix model;
    int iteration;
    vector<Correspondence *> workingSamples = samples;

    if (params.limitSamples() > 0) {
        workingSamples = sieveCorrespondanceList(workingSamples, params.limitSamples());
    }

    EssentialEstimator::CostFunction7to1 initialCost(&samples);

    L_INFO_P("Starting iterative rectification... with %d input samples", samples.size());

    double sigma = params.initialSigma();

    if (params.useInitial()) {
        workingSamples = filterCorrespondanceList(workingSamples, initalGuess, sigma, 0);
    }

    for (iteration = 1; iteration <= params.iterationsNumber(); iteration++)
    {
        SYNC_PRINT(("IterativeEstimator::getEssential(): starting itreation %d/%d (sigma=%lf) working samples %d\n",
                    (int)iteration, params.iterationsNumber(), sigma, (int)workingSamples.size()));

        EssentialEstimator estimator;
        if (workingSamples.size() < 9)
        {
            cout << "Too few inputs to continue:" << workingSamples.size() << endl;
            break;
        }
        model = estimator.getEssential(workingSamples, method);
       // cout << "Result:" << endl;
       // model.prettyPrint();
        EssentialEstimator::CostFunction7to1 cost(&workingSamples);

        /* Checking current result */
        vector<Correspondence *> passedSamples = filterCorrespondanceList(workingSamples, model, sigma, iteration);
        workingSamples = passedSamples;
        sigma *= params.sigmaFactor();
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

EssentialDecomposition IterativeEstimatorScene::getEssentialIterative(FixtureScene *scene, FixtureCamera *camera1, FixtureCamera *camera2)
{
    vector<Correspondence > data;

    //CameraModel cam1world = camera1->getWorldCameraModel();
    //CameraModel cam2world = camera2->getWorldCameraModel();

    for (SceneFeaturePoint *point: scene->featurePoints())
    {
        SceneObservation *obs1 = point->getObservation(camera1);
        SceneObservation *obs2 = point->getObservation(camera2);

        if (obs1 != NULL && obs2 != NULL)
        {
            Correspondence c;
            Vector2dd startPixel = obs1->getUndist();
            Vector2dd endPixel   = obs2->getUndist();

            c.start = camera1->intrinsics->reverse(startPixel).xy();
            c.end   = camera2->intrinsics->reverse(endPixel  ).xy();

            data.push_back(c);
        }
    }

    /* Be extreamly careful. You can only fill this array when actuall data is finilised */
    /* This array holds pointers, but target vector could be reallocated on resize       */
    vector<Correspondence *> dataPtr;
    dataPtr.reserve(data.size());
    for (size_t p = 0; p < data.size(); p++)
    {
        dataPtr.push_back(&data[p]);
    }

    // MEFIXASAP
    double thresholdScaler = 1000;//camera1->intrinsics.fx();

    IterativeEstimator estimator;
    estimator.params = params;
    estimator.params.setInitialSigma(estimator.params.initialSigma() / thresholdScaler);
    estimator.method = EssentialEstimator::METHOD_DEFAULT;
    if (params.useInitial())
    {
        estimator.initalGuess = camera1->getWorldCameraModel().essentialTo(camera2->getWorldCameraModel());
    }


    EssentialMatrix model = estimator.getEssential(dataPtr);

    int count = 0;
    for (SceneFeaturePoint *point: scene->featurePoints())
    {
        SceneObservation *obs1 = point->getObservation(camera1);
        SceneObservation *obs2 = point->getObservation(camera2);

        if (obs1 != NULL && obs2 != NULL)
        {
//            Correspondence::CorrespondenceFlags flag = (Correspondence::CorrespondenceFlags)data[count].flags;

            double cost = model.epipolarDistance(data[count]);
            Vector2dd accuracy(cost / params.initialSigma(), cost * thresholdScaler);

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
