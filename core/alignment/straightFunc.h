#pragma once
#include "function.h"

/**
 *  \ingroup distcorrect
 *  \ingroup cppcorefiles
 *
 *  Design is heavily based on
 *     http://glorfindel.mavrinac.com/~aaron/school/pdf/prescott97_linelens.pdf
 **/

namespace corecvs {

class StraightFunc : public FunctionArgs
{
public:
    StraightFunc(const vector<vector<Vector2dd> > &sample, Vector2d32 const &center);
    virtual void operator()(const double in[], double out[]);
    virtual Matrix getJacobian(const double in[], double delta = 1e-15);

private:
    vector<vector<Vector2dd> > mSample;
    Vector2d32 mCenter;
};
} // namespace corecvs

/*EOF*/
