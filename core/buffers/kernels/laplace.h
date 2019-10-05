#ifndef LAPLACE_H
#define LAPLACE_H

#include "core/buffers/abstractKernel.h"

namespace corecvs {


template<class BaseType>
class Laplace3x3 : public BaseType
{
public:
    static double data[9];

    static Laplace3x3 *instance;

    Laplace3x3(typename BaseType::InternalElementType bias = 0) :
        BaseType(3,3)
    {
        this->bias = bias;

        for (int i = 0; i < this->h; i++)
        {
            for (int j = 0; j < this->w; j++)
            {
                 this->element(i,j) = data[i * this->w + j];
            }
        }
    }
    virtual ~Laplace3x3(){}
};

template<class BaseType>
double Laplace3x3<BaseType>::data[9] = {
    0, -1,  0,
   -1,  4, -1,
    0, -1,  0
};



/**
 *  \f[  LoG(x,y) = {1 \over {\pi \sigma^4}} \left(  1 -  {{x^2 + y^2} \over {2 \sigma^2}} \right)  {e^{{x^2 + y^2} \over {2 \sigma^2}}} \f]
 **/
class LaplacianKernel : public DpKernel
{
public:
    LaplacianKernel(int height, int width, int x, int y, double sigma);
    //GaussianKernel(int height, int width, double sigma);

    virtual ~LaplacianKernel(){}
};


template <typename Algebra>
class Laplacian3x3Kernel
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;
    int bias;

    inline static int getCenterX(){ return 1; }
    inline static int getCenterY(){ return 1; }
    inline static int getSizeX(){ return 3; }
    inline static int getSizeY(){ return 3; }

    typedef typename Algebra::InputType Type;

template<typename OtherAlgebra>
    Laplacian3x3Kernel(const Laplacian3x3Kernel<OtherAlgebra> &other) :
        bias(other.bias)
    {}

    Laplacian3x3Kernel(int bias = 0) :
        bias(bias)
    {}

    void process(Algebra &algebra) const
    {
        Type a01 = algebra.getInput(0,1);
        Type a10 = algebra.getInput(1,0);
        Type a11 = algebra.getInput(1,1);
        Type a12 = algebra.getInput(1,2);
        Type a21 = algebra.getInput(2,1);

        Type cross  = (a01 + a10 + a12 + a21);
        Type result = Algebra::template mul<4>(a11) - cross + Type(bias);
        algebra.putOutput(0,0,result);
    }
};


} // namespace corecvs

#endif // LAPLACE_H
