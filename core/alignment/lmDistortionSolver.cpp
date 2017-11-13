#include "core/alignment/lmDistortionSolver.h"
#include "camerasCalibration/camerasCalibrationFunc.h"
#include "core/math/levenmarq.h"
#include "core/utils/log.h"
#include "core/alignment/anglePointsFunction.h"
#include "core/alignment/distPointsFunction.h"

namespace corecvs {

LMDistortionSolver::LMDistortionSolver()
{
}


RadialCorrection LMDistortionSolver::solve()
{
    vector<double> values;
    vector<Vector3dd> arguments;

    for (unsigned i = 0; i < list->size(); i++ )
    {
        PointObservation observation = list->operator [](i);
        values.push_back(observation.u());
        values.push_back(observation.v());
        arguments.push_back(observation.point);
    }
    CamerasCalibrationFunc func(&arguments, initialCenter);
    vector<double> bestParams;

    double params[] = {0, 0, 0, 0, -5.09, 11.1, -30.4, 35, 0.12, 0.04, -4.81, 14.6, -11.2, 11.2};
    vector<double> parameters(params, params + CORE_COUNT_OF(params));
    LevenbergMarquardt LMTransform(10000, 4, 1.2);
    LMTransform.f = &func;
    func.setIsDistored(false);
    bestParams = LMTransform.fit(parameters,values);
    func.setIsDistored(true);
    bestParams = LMTransform.fit(bestParams, values);
    vector<double> polynomKoeff;
    polynomKoeff.push_back(bestParams.at(0));
    polynomKoeff.push_back(bestParams.at(1));
    //    LensDistortionModelParameters lenCorrectParams(polynomKoeff, bestParams.at(2), bestParams.at(3), center);

    LensDistortionModelParameters lenCorrectParams(
                initialCenter.x(),
                initialCenter.y(),
                bestParams.at(2), bestParams.at(3),
                polynomKoeff,
                1.0,
                1.0,
                initialCenter.l2Metric(),
                0.0,
                0.0);


    return RadialCorrection(lenCorrectParams);
 }


LMLinesDistortionSolver::LMLinesDistortionSolver()
{
    for (int costType = 0; costType < LineDistortionEstimatorCost::LINE_DISTORTION_ESTIMATOR_COST_LAST; costType++)
    {
        costs[costType] = EllipticalApproximation1d();
    }
}

RadialCorrection LMLinesDistortionSolver::solve()
{
    lineList->print();
    cout << "Parameters:" << parameters << std::endl;
    cout << "Guess center:" << initialCenter << std::endl;

    vector<vector<Vector2dd> > straights = lineList->getLines();
    L_INFO_P("Starting distortion calibration on %d lines", straights.size());

    RadialCorrection correction(LensDistortionModelParameters(
       initialCenter.x(),
       initialCenter.y(),
       0.0 ,0.0,
       vector<double>(parameters.polinomDegree()),
       1.0,
       1.0,
       initialCenter.l2Metric(),
       0.0,
       0.0
    ));

    ModelToRadialCorrection modelFactory(
        correction,
        parameters.estimateCenter(),
        parameters.estimateTangent(),
        parameters.polinomDegree(),
        parameters.evenPowersOnly()
    );

    FunctionArgs *costFunction = NULL;
    if (parameters.costAlgorithm() == LineDistortionEstimatorCost::JOINT_ANGLE_COST) {
        costFunction = new AnglePointsFunction (straights, modelFactory);
    } else {
        costFunction = new DistPointsFunction  (straights, modelFactory);
    }

    LevenbergMarquardt straightLevMarq(parameters.iterationNumber(), 2, 1.5);
    straightLevMarq.f = costFunction;

    /* First aproximation is zero vector */
    vector<double> first(costFunction->inputs, 0);
    modelFactory.getModel(correction, &first[0]);

    vector<double> value(costFunction->outputs, 0);
    vector<double> straightParams = straightLevMarq.fit(first, value);

    L_INFO_P("Ending distortion calibration");
//    updateScore();
    delete costFunction;
    return modelFactory.getRadial(&straightParams[0]);

}

void LMLinesDistortionSolver::computeCosts(const RadialCorrection &correction, bool updatePoints)
{
    vector<vector<Vector2dd> > straights = lineList->getLines();

    ModelToRadialCorrection modelFactory(
        correction,
        parameters.estimateCenter(),
        parameters.estimateTangent(),
        parameters.polinomDegree(),
        parameters.evenPowersOnly()
    );

    FunctionArgs *costFunction = NULL;
    for (int costType = 0; costType < LineDistortionEstimatorCost::LINE_DISTORTION_ESTIMATOR_COST_LAST; costType++)
    {
        costs[costType] = EllipticalApproximation1d();

        if (costType == LineDistortionEstimatorCost::JOINT_ANGLE_COST) {
            costFunction = new AnglePointsFunction (straights, modelFactory);
        } else {
            costFunction = new DistPointsFunction  (straights, modelFactory);
        }

        vector<double> modelParameters(costFunction->inputs, 0);
        modelFactory.getModel(correction, &modelParameters[0]);

        vector<double> result(costFunction->outputs);
        costFunction->operator()(&modelParameters[0], &result[0]);

        for (unsigned i = 0; i < result.size(); i++) {
            costs[costType].addPoint(result[i]);
        }

        if (updatePoints && costType == parameters.costAlgorithm())
        {
            int count = 0;
            double maxValue = costs[costType].getMax();
            for (unsigned i = 0; i < (unsigned)lineList->mPaths.size(); i++)
            {
                SelectableGeometryFeatures::VertexPath *path = lineList->mPaths[i];
                if (path->vertexes.size() < 3) {
                    continue;
                }
                if (parameters.costAlgorithm() == LineDistortionEstimatorCost::JOINT_ANGLE_COST)
                {
                    for (unsigned j = 1; j < path->vertexes.size() - 1; j++) {
                        path->vertexes[j]->weight = fabs(result[count]) / maxValue;
                        count++;
                    }
                } else {
                    for (unsigned j = 0; j < path->vertexes.size(); j++) {
                        path->vertexes[j]->weight = fabs(result[count]) / maxValue;
                        count++;
                    }
                }
            }
        }      
    }
    delete costFunction;
}

} // namespace corecvs
