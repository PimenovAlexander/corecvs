#pragma once
#include "function.h"

/**
 * \file angleFunction.h
 * \ingroup cppcorefiles
 *
 **/

namespace corecvs
{

/**
 *   \ingroup distcorrect
 **/
class AngleFunction : public FunctionArgs
{
public:
    AngleFunction(const vector<vector<Vector2dd> > &sample, Vector2d32 const &center, int polynomDegree, double scaleFactor);
    virtual void operator()(const double in[], double out[]);
    virtual Matrix getJacobian(const double in[], double delta = 1e-15);

private:
    vector<vector<Vector2dd> > mSample;
    Vector2dd mCenter;
    int mPolynomDegree;
    double mScaleFactor;
};
}
