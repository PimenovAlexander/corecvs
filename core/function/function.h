#ifndef _FUNCTION_H_
#define _FUNCTION_H_
/**
 * \file function.h
 * \brief a header that contains generic function
 *
 *
 * \ingroup cppcorefiles
 * \date Oct 28, 2009
 * \author alexander
 */
#include <vector>
#include <chrono>

#include "global.h"

#include "cblasLapackeWrapper.h"

#include "matrix.h"
#include "sparseMatrix.h"
#include "vector.h"

namespace corecvs {
using std::vector;

/**
 *  This class is a virtual version of a \f$ f: R^n \mapsto R^m\f$
 *
 *  TODO: Think of the similar class based on static polymorphism.
 **/
class JacobianFunctor;
class FunctionArgs
{
public:
    int inputs;
    int outputs;
    std::vector<int> schurBlocks;

    FunctionArgs(int _inputs, int _outputs) :
        inputs (_inputs),
        outputs (_outputs)
    {}


    /**
     *  Operator that computes funсtion
     **/
    virtual void operator()(const double in[], double out[]) = 0;

    virtual void operator()(const Vector &in, Vector &out)
    {
        return operator()(in.element, out.element);
    }

    virtual void operator()(const vector<double> &in, vector<double> &out)
    {
        CORE_ASSERT_TRUE( (int)  in.size() > inputs , "Too few input numbers");
        CORE_ASSERT_TRUE( (int) out.size() > outputs, "Too few output numbers");

        return operator()(&in[0], &out[1]);
    }

    /**
     *  This function computes Jacobian
     *   \f[
     *
     *   J= \pmatrix{
     *      \frac{\partial y_1}{\partial x_1} & \cdots & \frac{\partial y_1}{\partial x_n} \cr
     *              \vdots                    & \ddots &                   \vdots          \cr
     *      \frac{\partial y_m}{\partial x_1} & \cdots & \frac{\partial y_m}{\partial x_n}
     *      }
     *
     *   \f]
     *
     *
     *   TODO: Bring differencing out and use following information
     *   http://en.wikipedia.org/wiki/Numerical_differentiation#Practical_considerations_using_floating_point_arithmetic
     **/
    virtual Matrix getJacobian(const double in[], double delta = 1e-7);

    Matrix getNativeJacobian(const double in[], double delta = 1e-7)
    {
        return getJacobian(in, delta);
    }

    double resultLength(const double in[])
    {
        vector<double> out(outputs);

        operator ()(in, &out[0]);
        double sumsq = 0.0;
        for (double d : out)
        {
            sumsq += d * d;
        }
        sumsq /= outputs;
        return sqrt(sumsq);
    }

    virtual Matrix getJacobian(const Vector &in, double delta = 1e-7)
    {
        return getJacobian(in.element, delta);
    }

    /*
     * This function returns hessian matrices for all output functions
     * Output is outputs x (inputs x inputs matrix) vector
     * We use a second-order finite differences in order to get this,
     * for example, for f(x,y) we will evaluate f at the following points:
     *       +
     *    *  +  *
     * +  +  +  +  +
     *    *  +  *
     *       +
     *  Where '+' points are used for f^{(2)}_{xx} or f^{(2)}_{yy} computations,
     *  while '*' points are used for f^({2})_{xy} computations
     */
    virtual std::vector<Matrix> getHessians(const Vector &in, double delta = 1e-5);

    // Gets Hessian for \frac{1}{2}\sum f_k(x_1,...,x_n)^2
    virtual Matrix getLSQHessian(const double* in, double delta = 1e-5);

    virtual ~FunctionArgs() {}

};

// This class only reshapes jacobian of another function
class JacobianFunctor : public FunctionArgs
{
public:
    JacobianFunctor(FunctionArgs *fun) : FunctionArgs(fun->inputs, fun->inputs * fun->outputs), fun(fun)
    {
    }
    void operator() (const double* in, double* out)
    {
        auto J = fun->getJacobian(in, 1e-9);
        for (int i = 0; i < J.h; ++i)
            for (int j = 0; j < J.w; ++j)
                *out++ = J.a(i, j);
    }
    FunctionArgs *fun;
};

class SparseFunctionArgs : public FunctionArgs
{
public:
    double feval = 0.0, transp = 0.0, construct = 0.0, prepare = 0.0, other = 0.0, subscale = 0.0;
    SparseFunctionArgs() : FunctionArgs(0, 0) {}
    SparseFunctionArgs(int inputs, int outputs, const std::vector<std::vector<int>> &dependencyList) : FunctionArgs(inputs, outputs), dependencyList(dependencyList), fullIdx(outputs)
    {
        init(inputs, outputs, dependencyList);
    }

    void init(int in, int out, const std::vector<std::vector<int>> &dep)
    {
        inputs = in;
        outputs = out;
        dependencyList  = dep;
        fullIdx.clear();
        fullIdx.resize(outputs);

        std::cout << "Sparse: R^" << inputs << "->R^" << outputs << std::endl;
        for (int i = 0; i < outputs; ++i)
            fullIdx[i] = i;
        minify();
    }
    //! \brief This should compute only needed indices
    virtual void operator() (const double* in, double* out, const std::vector<int> &idx) = 0;
    virtual void operator() (const double* in, double* out)
    {
        (*this)(in, out, fullIdx);
    }

    Matrix getJacobian(const double* in, double delta = 1e-7)
    {
        return (Matrix)getNativeJacobian(in, delta);
    }

    SparseMatrix getNativeJacobian(const double* in, double delta = 1e-7);

    /*
     * This function groups input variables using provided output dependency lists
     * in order to minimize number of function calls during jacobian computation
     */
    void minify();

    virtual ~SparseFunctionArgs()
    {
        double total = (feval + transp + construct + prepare + other + subscale);
        std::cout << "Feval: " << feval << "s " << feval / total * 100.0 << "%" << std::endl;
        std::cout << "Trans: " << transp<< "s " << transp/ total * 100.0 << "%" << std::endl;
        std::cout << "Prep : " <<prepare << "s " <<prepare/ total * 100.0 << "%" << std::endl;
        std::cout << "Const: " <<construct<< "s " <<construct / total * 100.0 << "%" << std::endl;
        std::cout << "Subscale:" << subscale << "s " << subscale / total * 100.0 << "%" << std::endl;
        std::cout << "Other: " <<other<< "s " <<other / total * 100.0 << "%" << std::endl;
    }

protected:
    std::vector<std::vector<int>> groupInputs, groupOutputs, remapIdx;
    std::vector<std::vector<int>> dependencyList;
    std::vector<int>              fullIdx;
};

class IdentityFunction : public FunctionArgs
{
public:
    IdentityFunction(int dimension) : FunctionArgs(dimension, dimension) {}

    virtual void operator()(const double in[], double out[])
    {
        for (int i = 0; i < inputs; i++)
            out[i] = in[i];
    }

    virtual Matrix getJacobian(const double /*in*/[], double /*delta*/ = 1e-7)
    {
        Matrix result(outputs, inputs, 1.0);
        return result;
    }
};


class ScalerFunction : public FunctionArgs
{
public:
    FunctionArgs *F;
    double scale;
    double shift;


    ScalerFunction(FunctionArgs *_F, double _scale, double _shift) :
        FunctionArgs(_F->inputs, _F->outputs),
        F(_F),
        scale(_scale),
        shift(_shift)
    {
        cout << inputs << " " << outputs << endl;
        cout << F->inputs << " " << F->outputs << endl;
    }

    virtual void operator()(const double in[], double out[])
    {
        F->operator ()(in,out);
        for (int i = 0; i < outputs; i++)
            out[i] = out[i] * scale + shift;
    }
};

template <class Map2D>
class Map2DFunction : public FunctionArgs
{
    Map2D *mMap;

public:
    Map2DFunction(Map2D *map) :
        FunctionArgs(2, 2),
        mMap(map)
    {}

    virtual void operator()(const double in[], double out[])
    {
        Vector2dd mapResult = mMap->map(in[0], in[1]);
        out[1] = mapResult.x() - in[1];
        out[0] = mapResult.y() - in[0];
    }
};


class LengthFunction : public FunctionArgs
{
    FunctionArgs *F;

public:
    LengthFunction(FunctionArgs *_F) :
        FunctionArgs(_F->inputs, 1),
        F(_F)
    {
        cout << inputs << " " << outputs << endl;
        cout << F->inputs << " " << F->outputs << endl;
    }

    virtual void operator()(const double in[], double out[])
    {
        vector<double> tmpOut(F->outputs);

        F->operator ()(in, &tmpOut[0]);
        out[0] = 0.0;
        for (int i = 0; i < F->outputs; i++) {
            out[0] += tmpOut[i] * tmpOut[i];
        }
        out[0] = sqrt(out[0]);
    }
};


template<typename RealFuncType, int inputDim, int outputDim>
class FunctionStatic
{
public:
    typedef FixedVector<double, inputDim>   InputType;
    typedef FixedVector<double, outputDim> OutputType;

    void operator()(const InputType &/*in*/, OutputType &/*out*/) {}

    OutputType f(const InputType &in)
    {
        OutputType result;
        RealFuncType *realThis = (RealFuncType *)this;
        realThis->operator()(in, result);
        return result;
    }

    virtual OutputType vf(const InputType &in)
    {
        OutputType result;
        RealFuncType *realThis = (RealFuncType *)this;
        realThis->operator()(in, result);
        return result;
    }

};

} //namespace corecvs

#endif // _FUNCTION_H_
