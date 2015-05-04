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

#include "global.h"

#include "matrix.h"
#include "vector.h"

namespace corecvs {
using std::vector;

/**
 *  This class is a virtual version of a \f$ f: R^n \mapsto R^m\f$
 *
 *  TODO: Think of the similar class based on static polymorphism.
 **/
class FunctionArgs
{
public:
    int inputs;
    int outputs;

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
    virtual Matrix getJacobian(const double in[], double delta = 1e-7)
    {
        Matrix result(outputs, inputs);
        vector<double> xc(inputs);
        vector<double> y_minus(outputs);
        vector<double> y_plus (outputs);

        for (int i = 0; i < inputs; i++)
        {
            xc[i] = in[i];
        }

        for (int i = 0; i < inputs; i++)
        {
            xc[i] = in[i] - delta;
            operator()(&xc[0], &y_minus[0]);
            xc[i] = in[i] + delta;
            operator()(&xc[0], &y_plus[0]);
            xc[i] = in[i];

            for (int j = 0; j < outputs; j++)
            {
                result.element(j,i) = (y_plus[j] - y_minus[j]) / (2.0 * delta);
            }
        }
        return result;
    }

    virtual Matrix getJacobian(const Vector &in, double delta = 1e-7)
    {
        return getJacobian(in.element, delta);
    }

    virtual ~FunctionArgs() {};

};

class IdentityFunction : public FunctionArgs
{
public:
    IdentityFunction(int dimention) : FunctionArgs(dimention, dimention) {};

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
    };

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
    };

    virtual void operator()(const double in[], double out[])
    {
#ifdef WIN32
        double* tmpOut = new double[F->outputs];
#else
        double tmpOut[F->outputs];
#endif
        F->operator ()(in, tmpOut);
        out[0] = 0.0;
        for (int i = 0; i < F->outputs; i++) {
            out[0] += tmpOut[i] * tmpOut[i];
        }
        out[0] = sqrt(out[0]);
#ifdef WIN32
        delete[] tmpOut;
#endif
    }
};


template<typename RealFuncType, int inputDim, int outputDim>
class FunctionStatic
{
public:
    //const int  inputDim =  inputDim;
    //const int outputDim = outputDim;

    typedef FixedVector<double, inputDim>   InputType;
    typedef FixedVector<double, outputDim> OutputType;

    void operator()(const InputType &/*in*/, OutputType &/*out*/) {};

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

#if 0
    /**
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
    Matrix getJacobian(const InputType &x, double delta = 1e-7)
    {
        Matrix result(outputDim, inputDim);
        for (int i = 0; i < inputDim; i++)
        {
           Function::InputType xc = x;
           xc[i] -= delta;
           Function::InputType y_minus;
           Function::InputType y_plus;
           F(xc, y_minus);
           xc[i] = x[i] + delta;
           F(xc, y_plus);

           for (int j = 0; j < Function::outputDim; j++)
           {
               result.element(j,i) = (y_plus[j] - y_minus[j]) / (2.0 * delta);
           }
        }
        return result;
    }
#endif
};

#if 0
double const delta = 0.1;

class Function
{
public:
    Function()
    {
        memset(args, '\0', sizeof(float) * dimension);
    }

    static const int dimension = 3;

    float operator ()(float a, float b, float c)
    {
        return a + b*b + c*c*c;
    }

    float operator()()
    {
        return args[0] + args[1]*args[1] + args[2] * args[2] * args[2];
    }

    void setArg(int argNum, double arg)
    {
        args[argNum] = arg;
    }

    void modifyArg(int argNum, double delta)
    {
        args[argNum] += delta;
    }

private:
    double args[dimension];

};

template <typename T>
class Derivative
{
public:
    float operator ()(int number, ...)
    {
        T f;
        number--;
        double val = 0.0;
        va_list vl;
        va_start(vl, number);
        for (int i=0; i < Function::dimension; i++)
        {
            val = va_arg(vl, double);
            f.setArg(i, val);
        }
        va_end(vl);
        f.modifyArg(number, -delta);
        double f1 = f();
        f.modifyArg(number, 2 * delta);
        double f2 = f();
        return (f2 - f1) / (2 * delta);
    }
};
#endif

} //namespace corecvs
#endif // _FUNCTION_H_

