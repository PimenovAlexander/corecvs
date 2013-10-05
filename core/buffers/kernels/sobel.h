/**
 * \file sobel.h
 * \brief This file contains Sobel filters
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 27, 2010
 * \author alexander
 */

#ifndef SOBEL_H_
#define SOBEL_H_

#include "global.h"

#include "abstractKernel.h"

namespace corecvs {

/**
 *   Old style (AbstractKernel style) Kernel for Vertical Sobel operator for integer buffers
 *
 *  http://en.wikipedia.org/wiki/Sobel_operator
 *
 **/

class SobelVertInt : public AbstractKernel<int32_t, uint32_t>
{
public:
    static int32_t data[9];
    static SobelVertInt *instance;

    SobelVertInt() : AbstractKernel<int32_t, uint32_t> (3, 3, data, 4, 0, 1, 1) {};
    virtual ~SobelVertInt(){};
};


/**
 *   Old style (AbstractKernel style) Kernel for Horizontal Sobel operator for integer buffers
 *
 *  http://en.wikipedia.org/wiki/Sobel_operator
 *
 **/

class SobelHorInt : public AbstractKernel<int32_t, uint32_t>
{
public:
    static int32_t data[9];
    static SobelHorInt *instance;

    SobelHorInt() : AbstractKernel<int32_t, uint32_t> (3, 3, data, 4, 0, 1, 1) {};
    virtual ~SobelHorInt(){};
};

/**
 *  Cool new style (FastKernels) Horizontal Sobel filter
 **/
template <typename Algebra>
class SobelHorizontalKernel
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 1; }
    inline static int getCenterY(){ return 1; }
    inline static int getSizeX()  { return 3; }
    inline static int getSizeY()  { return 3; }

    typedef typename Algebra::InputType Type;
    typedef typename Algebra::InputSignedType SignedType;

    int16_t bias;

template <typename OtherAlgebra>
    SobelHorizontalKernel(const SobelHorizontalKernel<OtherAlgebra> &other)
    {
        bias = other.bias;
    }

    SobelHorizontalKernel() :
    	bias(0)
    {}

    inline void process(Algebra &algebra) const
    {
        Type a00 = algebra.getInput(0,0);
        Type a10 = algebra.getInput(1,0);
        Type a20 = algebra.getInput(2,0);
        Type a02 = algebra.getInput(0,2);
        Type a12 = algebra.getInput(1,2);
        Type a22 = algebra.getInput(2,2);

        SignedType positive = SignedType(a00 + a20 + Algebra::template mul<2>(a10));
        SignedType negative = SignedType(a02 + a22 + Algebra::template mul<2>(a12));

        Type result = Type(Algebra::template div<4>( positive - negative ) + SignedType(bias));
        algebra.putOutput(0,0,result);
    }
};


/**
 *  Cool new style (FastKernels) Vertical Sobel filter
 **/
template <typename Algebra>
class SobelVerticalKernel
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 1; }
    inline static int getCenterY(){ return 1; }
    inline static int getSizeX()  { return 3; }
    inline static int getSizeY()  { return 3; }

    typedef typename Algebra::InputType       Type;
    typedef typename Algebra::InputSignedType SignedType;

    int16_t bias;

template <typename OtherAlgebra>
    SobelVerticalKernel(const SobelVerticalKernel<OtherAlgebra> &other){
        bias = other.bias;
    }

	SobelVerticalKernel() :
		bias(0)
    {}


    inline void process(Algebra &algebra) const
    {
        Type a00 = algebra.getInput(0,0);
        Type a01 = algebra.getInput(0,1);
        Type a02 = algebra.getInput(0,2);
        Type a20 = algebra.getInput(2,0);
        Type a21 = algebra.getInput(2,1);
        Type a22 = algebra.getInput(2,2);

        SignedType positive = SignedType(a00 + a02 + Algebra::template mul<2>(a01));
        SignedType negative = SignedType(a20 + a22 + Algebra::template mul<2>(a21));

        Type result = Type(Algebra::template div<4>( positive - negative ) + SignedType(bias));
        algebra.putOutput(0,0,result);
    }
};


template <typename Algebra>
class EdgeMagnitude
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 1; }
    inline static int getCenterY(){ return 1; }
    inline static int getSizeX()  { return 3; }
    inline static int getSizeY()  { return 3; }

    typedef typename Algebra::InputType       Type;
    typedef typename Algebra::InputSignedType SignedType;

template <typename OtherAlgebra>
    EdgeMagnitude(const EdgeMagnitude<OtherAlgebra> &) {}
    EdgeMagnitude(){}

    inline void process(Algebra &algebra) const
    {
        Type a00 = algebra.getInput(0,0);
        Type a01 = algebra.getInput(0,1);
        Type a02 = algebra.getInput(0,2);
        Type a10 = algebra.getInput(1,0);
        Type a12 = algebra.getInput(1,2);
        Type a20 = algebra.getInput(2,0);
        Type a21 = algebra.getInput(2,1);
        Type a22 = algebra.getInput(2,2);

        SignedType positiveH = SignedType(a00 + a20 + Algebra::template mul<2>(a10));
        SignedType negativeH = SignedType(a02 + a22 + Algebra::template mul<2>(a12));
        SignedType resultH = Algebra::template div<4>( positiveH - negativeH );


        SignedType positiveV = SignedType(a00 + a02 + Algebra::template mul<2>(a01));
        SignedType negativeV = SignedType(a20 + a22 + Algebra::template mul<2>(a21));
        SignedType resultV = Algebra::template div<4>( positiveV - negativeV );

        Type final = Type(Algebra::abs(resultV)) + Type(Algebra::abs(resultH));

        algebra.putOutput(0,0,final);
    }
};


} //namespace corecvs
#endif /* SOBEL_H_ */
