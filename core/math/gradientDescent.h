#ifndef GRADIENTDESCENT_H_
#define GRADIENTDESCENT_H_

/**
 * \file gradientDescent.h
 * \brief Gradient Descent algorithms for minimizing the functions
 *
 * \date Oct 20, 2011 
 */

#include <limits>

#include "utils/global.h"

#include "function/function.h"

namespace corecvs {


class GradientDescent
{
public:
    FunctionArgs *f;
    FunctionArgs *normalisation;
    double lambda;
    int maxIterations;
    double minGradient;

    GradientDescent(int _maxIterations = 25, double _lambda = 0.1) :
        f(NULL),
        normalisation(NULL),
        lambda(_lambda),
        maxIterations(_maxIterations),
        minGradient(std::numeric_limits<double>::min())
        {};

    vector<double> minimise(const vector<double> &input);
    vector<double> fit(const vector<double> &input, const vector<double> &output);
};


/**
 *   This class implements a solver that uses Gradient descent for functions
 *   \f$ F: R^n \mapsto R^m\f$. Quite obviously function should be smooth
 *
 **/
class GradientDescentRnToRm
{
public:
    FunctionArgs *f;
    FunctionArgs *normalisation;
    double lambda;
    int maxIterations;
    double minGradient;

    GradientDescentRnToRm(int _maxIterations = 25, double _lambda = 0.001) :
        f(NULL),
        normalisation(NULL),
        lambda(_lambda),
        maxIterations(_maxIterations),
        minGradient(std::numeric_limits<double>::min())
        {};

    vector<double> fit(const vector<double> &input, const vector<double> &output);
};


/*
class GradientDescentDividable
{
public:
    FunctionArgs *f;
    FunctionArgs *normalisation;
    double lambda;
    double lambdaFactor;
    double epsilon;
    int maxIterations;

    GradientDescent(int _maxIterations = 25, double _lambda = 0.1) :
        f(NULL),
        normalisation(NULL),
        lambda(_lambda),
        maxIterations(_maxIterations)
        {};

    vector<double> minimise(const vector<double> &input);
    vector<double> fit(const vector<double> &input, const vector<double> &output);
};
*/


} //namespace corecvs

#endif /* GRADIENTDESCENT_H_ */
