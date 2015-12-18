/**
 * \file gaussian.h
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 24, 2010
 * \author alexander
 */

#ifndef GAUSSIAN3X3_H_
#define GAUSSIAN3X3_H_

#include "global.h"

#include "abstractKernel.h"
namespace corecvs {

class Gaussian3x3 : public AbstractKernel<double, int32_t>
{
public:
    static double data[9];
    static Gaussian3x3 *instance;

    Gaussian3x3() : AbstractKernel<double, int32_t> (3, 3, data, 16, 0, 1, 1) {}
    virtual ~Gaussian3x3(){}
};


class Gaussian3x3int : public AbstractKernel<uint32_t, int32_t>
{
public:
    static uint32_t data[9];
    static Gaussian3x3int *instance;

    Gaussian3x3int() : AbstractKernel<uint32_t, int32_t> (3, 3, data, 16, 0, 1, 1) {}
    virtual ~Gaussian3x3int(){}
};


template <typename Algebra>
class Gaussian3x3Kernel
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 1; }
    inline static int getCenterY(){ return 1; }
    inline static int getSizeX(){ return 3; }
    inline static int getSizeY(){ return 3; }

    typedef typename Algebra::InputType Type;

template<typename OtherAlgebra>
    Gaussian3x3Kernel(const Gaussian3x3Kernel<OtherAlgebra> &) {}

    Gaussian3x3Kernel(){}

    void process(Algebra &algebra) const
    {
        Type a00 = algebra.getInput(0,0);
        Type a01 = algebra.getInput(0,1);
        Type a02 = algebra.getInput(0,2);
        Type a10 = algebra.getInput(1,0);
        Type a11 = algebra.getInput(1,1);
        Type a12 = algebra.getInput(1,2);
        Type a20 = algebra.getInput(2,0);
        Type a21 = algebra.getInput(2,1);
        Type a22 = algebra.getInput(2,2);

        Type center = Algebra::template mul<2>(a11);
        Type cross  = Algebra::template mul<2>(a01 + a10 + a12 + a21 + center);
        Type result = Algebra::template div<16>(a22 + a00 + a02 + a20 + cross);
        algebra.putOutput(0,0,result);
    }
};

/**
 * Computes
 *  (a +  b * 2 + c * 4 +  d * 2 +  e) / 10
 **/
template <typename Algebra>
class Blur5
{
public:
    typedef typename Algebra::InputType Type;
    static Type blur(const Type &a, const Type &b, const Type &c, const Type &d, const Type &e)
    {
        Type result = b + Algebra::template mul<2>(c) + d;
        result = Algebra::template div<2>(a + e) + result;
        return Algebra::template div<5>(result);
    }
};

template <typename Algebra>
class Blur5Horisontal
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 2; };
    inline static int getCenterY(){ return 0; };
    inline static int getSizeX(){ return 5; };
    inline static int getSizeY(){ return 1; };

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    Blur5Horisontal(const Blur5Horisontal<OtherAlgebra> &){}

    Blur5Horisontal(){};

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0);
        Type b = algebra.getInput(0,1);
        Type c = algebra.getInput(0,2);
        Type d = algebra.getInput(0,3);
        Type e = algebra.getInput(0,4);

        Type result = Blur5<Algebra>::blur(a,b,c,d,e);
        algebra.putOutput(0,0,result);

    }
};

template <typename Algebra>
class Blur5Vertical
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; }
    inline static int getCenterY(){ return 2; }
    inline static int getSizeX(){ return 1; }
    inline static int getSizeY(){ return 5; }

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    Blur5Vertical(const Blur5Vertical<OtherAlgebra> &){}

    Blur5Vertical(){}

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0);
        Type b = algebra.getInput(1,0);
        Type c = algebra.getInput(2,0);
        Type d = algebra.getInput(3,0);
        Type e = algebra.getInput(4,0);
        Type result = Blur5<Algebra>::blur(a,b,c,d,e);
        algebra.putOutput(0,0,result);
    }
};


} //namespace corecvs
#endif /* GAUSSIAN3X3_H_ */

