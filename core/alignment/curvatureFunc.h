#pragma once
#include "core/function/function.h"

namespace corecvs {

/**
 * \ingroup distcorrect
 **/
class CurvatureFunc : public FunctionArgs
{
public:
    CurvatureFunc(const vector<vector<Vector2dd> > &sample, Vector2d32 const &center, int polynomDegree, double scaleFactor);
    virtual void operator()(const double in[], double out[]);
    virtual Matrix getJacobian(const double in[], double delta = 1e-15);

private:
    vector<vector<Vector2dd> > mSample;
    Vector2dd mCenter;
//    Vector2dd getDistoredVector(const double in[], Vector2dd const &undistoredVector);
    int mPolynomDegree;
    double mScaleFactor;
};
} // namespace corecvs

/*EOF*/
