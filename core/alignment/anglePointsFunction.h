#pragma once
#include "radialFunc.h"

namespace corecvs
{

/**
 *  \ingroup distcorrect
 *
 *  This class describes the cost function that shows how a set of lines differ
 *  from the model. We use the polynomial model here and sine between segments
 *
 **/
class AnglePointsFunction : public FunctionArgs
{
public:
    AnglePointsFunction(
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

} // namespace corecvs

/*EOF*/


