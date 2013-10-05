#pragma once

/**
 * \file distPointsFunction.h
 *
 * \date Jun 11, 2013
 **/

#include "radialFunc.h"
#include "vector2d.h"

namespace corecvs
{

/**
 *  \ingroup distcorrect
 *
 *  This class describes the cost function that shows how a set of lines differ
 *  from the model. We use the polynomial model here and distance to best line
 *
 **/
class DistPointsFunction : public FunctionArgs
{
public:
    DistPointsFunction(
            const vector<vector<Vector2dd> > &sample,
            const ModelToRadialCorrection& modelFactory
    );

    virtual void operator()(const double in[], double out[]);
    virtual Matrix getJacobian(const double in[], double delta = 1e-15);

private:
    static int getOutputs(const vector<vector<Vector2dd> > &sample);

    bool mSimpleJacobian;
    vector<vector<Vector2dd> > mSample;
    const ModelToRadialCorrection& mModelFactory;
};

} /* namespace corecvs */
/* EOF */
