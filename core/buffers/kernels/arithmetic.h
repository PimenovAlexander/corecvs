#ifndef ARITHMETIC_H_
#define ARITHMETIC_H_
/**
 * \file arithmetic.h
 * \brief a header for Arithmetic.c
 *
 * \ingroup cppcorefiles
 * \date Oct 8, 2010
 * \author alexander
 */

#include "abstractBuffer.h"
#include "global.h"

namespace corecvs {

/**
 *  A FastKernels class that has common code for binary operations.
 **/
template <typename Algebra>
class BinaryElementwiseOperation
{
public:
    static const int inputNumber  = 2;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; }
    inline static int getCenterY(){ return 0; }
    inline static int getSizeX(){ return 1; }
    inline static int getSizeY(){ return 1; }
};


template <typename Algebra>
class SumBuffers : public BinaryElementwiseOperation<Algebra>
{
public:
    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    SumBuffers(const SumBuffers<OtherAlgebra> &){}

    SumBuffers(){}

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = (a + b);
        algebra.putOutput(0,0,0,result);
    }
};

template <typename Algebra>
class MixBuffers : public BinaryElementwiseOperation<Algebra>
{
public:

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    MixBuffers(const MixBuffers<OtherAlgebra> &){}

    MixBuffers(){}

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = Algebra::template div<2>(a + b);
        algebra.putOutput(0,0,0,result);
    }
};

template <typename Algebra>
class SubtractBuffers : public BinaryElementwiseOperation<Algebra>
{
public:
    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    SubtractBuffers(const SubtractBuffers<OtherAlgebra> &){}

    SubtractBuffers(){}

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = (a - b);
        algebra.putOutput(0,0,0,result);
    }
};


template <typename Algebra>
class DifferenceBuffers : public BinaryElementwiseOperation<Algebra>
{
public:
    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    DifferenceBuffers(const DifferenceBuffers<OtherAlgebra> &){}

    DifferenceBuffers(){}

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = Algebra::difference(a,b);
        algebra.putOutput(0,0,0,result);
    }
};

#define PROTO_KERNEL(input, outputs, sizeX, sizeY, centerX, centerY)

/* Generic convolve for double */
template <typename Algebra>
class ConvolveKernel
{
public:
    AbstractBuffer<double> *elements;
    int x;
    int y;


    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline int getCenterX(){ return x; }
    inline int getCenterY(){ return y; }
    inline int getSizeX()  { return elements->w; }
    inline int getSizeY()  { return elements->h; }

    ConvolveKernel(AbstractBuffer<double> *_elements, int _y, int _x) :
        elements(_elements), x(_x), y(_y) {}

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    ConvolveKernel(const ConvolveKernel<OtherAlgebra> & other)
    {
        this->x = other.x;
        this->y = other.y;
        this->elements = other.elements;
    }

    inline void process(Algebra &algebra) const
    {
        Type result(0.0);

        for (int i = 0; i < elements->h; i++)
        {
            for (int j = 0; j < elements->w; j++)
            {
                 //result += algebra.getInput(i,j) * Type(elements->element(i,j));
                result = multiplyAdd(algebra.getInput(i,j), Type(elements->element(i,j)), result);
            }
        }
        algebra.putOutput(0,0,result);
    }
};




} //namespace corecvs
#endif  //ARITHMETIC_H_

