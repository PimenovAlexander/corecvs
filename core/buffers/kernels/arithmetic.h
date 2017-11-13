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

#include "core/buffers/abstractBuffer.h"
#include "core/utils/global.h"

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
template <typename Algebra, typename KernelType = AbstractBuffer<double> >
class GenericConvolveKernel
{
public:
    KernelType *elements;
    int x;
    int y;

    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline int getCenterX(){ return x; }
    inline int getCenterY(){ return y; }
    inline int getSizeX()  { return elements->w; }
    inline int getSizeY()  { return elements->h; }

    typedef typename Algebra::InputType Type;

    GenericConvolveKernel(KernelType *_elements, int _y, int _x) :
        elements(_elements), x(_x), y(_y) {}

template <typename OtherAlgebra>
    GenericConvolveKernel(const GenericConvolveKernel<OtherAlgebra, KernelType> & other) :
        elements(other.elements),
        x(other.x),
        y(other.y)
    {}

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

template <typename Algebra>
    class ConvolveKernel : public GenericConvolveKernel<Algebra, AbstractBuffer<double>>
    {
    public:
        ConvolveKernel(AbstractBuffer<double> *_elements, int _y, int _x) :
            GenericConvolveKernel<Algebra, AbstractBuffer<double>>(_elements, _y, _x)
        {}

    template <typename OtherAlgebra>
        ConvolveKernel(const ConvolveKernel<OtherAlgebra> & other) :
            GenericConvolveKernel<Algebra, AbstractBuffer<double>>(other)
        {}

    };


template <typename Algebra>
    class FloatConvolveKernel : public GenericConvolveKernel<Algebra, AbstractBuffer<float>>
    {
    public:
        FloatConvolveKernel(AbstractBuffer<float> *_elements, int _y, int _x) :
            GenericConvolveKernel<Algebra, AbstractBuffer<float> >(_elements, _y, _x)
        {}

    template <typename OtherAlgebra>
        FloatConvolveKernel(const FloatConvolveKernel<OtherAlgebra> & other) :
            GenericConvolveKernel<Algebra, AbstractBuffer<float> >(other)
        {}

    };



} //namespace corecvs
#endif  //ARITHMETIC_H_

