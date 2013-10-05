#ifndef LEVEN_MARQ_H
#define LEVEN_MARQ_H

/**
 * \file levenmarq.h
 * \brief This class implements the Levenberg Marquardt algorithm
 *
 * \ingroup cppcorefiles
 * \date Mar 4, 2010
 * \author alexander
 */

#include <vector>

#include "global.h"

#include "vector2d.h"
#include "matrix.h"
#include "function.h"
namespace corecvs {


class LevenbergMarquardt
{
public:
    FunctionArgs *f;
    FunctionArgs *normalisation;
    double startLambda;
    double lambdaFactor;
    int maxIterations;

    LevenbergMarquardt(int _maxIterations = 25, double _startLambda = 10, double _lambdaFactor = 2.0) :
        f(NULL),
        normalisation(NULL),
        startLambda(_startLambda),
        lambdaFactor(_lambdaFactor),
        maxIterations(_maxIterations)
        {};

    vector<double> fit(const vector<double> &input, const vector<double> &output);

};


} //namespace corecvs
#endif

