#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_
/**
 * \file helperFunctions.h
 * \brief Add Comment Here
 *
 * \date Oct 21, 2011
 * \author alexander
 */
#include "function/function.h"
namespace corecvs {

/**
 *   http://en.wikipedia.org/wiki/Rosenbrock_function
 *
 *
 *  \f[ f(x, y) = (1-x)^2 + 100(y-x^2)^2 \f]
 *
 */
class RosenbrockFunction : public FunctionArgs
{
public:
    enum {
        INPUT_X,
        INPUT_Y,
        INPUT_NUMBER
    };

    RosenbrockFunction() : FunctionArgs(INPUT_NUMBER,1)
    { }

    virtual void operator()(const double in[], double out[])
    {
        double x = in[INPUT_X];
        double y = in[INPUT_Y];
        out[0] = (1.0 - x)*(1.0 - x) + 100 * (y - x * x) * (y - x * x);
    }

};


} //namespace corecvs
#endif /* HELPERFUNCTIONS_H_ */

