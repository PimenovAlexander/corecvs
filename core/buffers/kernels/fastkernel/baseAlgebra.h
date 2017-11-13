#ifndef BASEALGEBRA_H_
#define BASEALGEBRA_H_
/**
 * \file baseAlgebra.h
 * \brief a header for BaseAlgebra.c
 *
 * \ingroup cppcorefiles
 * \date Sep 26, 2010
 * \author alexander
 */

#include "core/utils/global.h"
namespace corecvs {

/**
 * This class does nothing. And luckily this could be used to create
 * kernels not knowing and not giving the details over which algebra it will actually
 * work.
 *
 *
 * <pre>
 *
 *   Ignorance is bliss.
 *
 *   War is Peace
 *   Freedom is Slavery
 *   Ignorance is Power
 *           1984
 * </pre>
 *
 *
 *
 **/

class DummyAlgebra
{
public:
    typedef uint8_t InputType;
    typedef int8_t InputSignedType;
};


/**
 *   This class describes the basic operations and function prototypes to work with
 *   input and output buffers. Mostly it is consecutive iteration over the buffers.
 *
 *   Generally algebra is an interator + working type traits + operations with working types.
 *   This class implements interator functionality.
 *
 * */
template <
    typename _InternalTypeInput,
    typename _InternalTypeOutput = _InternalTypeInput,
    int stepSize = 1,
    int inputNumber = 1,
    int outputNumber = 1
    >
class BaseAlgebraMulti
{
public:
    typedef _InternalTypeInput  InternalInputType;
    typedef _InternalTypeOutput InternalOutputType;

    InternalInputType  *ptrI[inputNumber];
    InternalOutputType *ptrO[outputNumber];

    int strideI[inputNumber];
    int strideO[outputNumber];

    BaseAlgebraMulti()
    {
    }

    inline BaseAlgebraMulti(const BaseAlgebraMulti &other)
    {

//        DOTRACE(("Base Algebra Copy constructor called (in %d, out %d)\n", inputNumber, outputNumber));
        for (unsigned in = 0; in < inputNumber; in++)
        {
            this->ptrI[in] = other.ptrI[in];
            this->strideI[in] = other.strideI[in];
        }

        for (unsigned out = 0; out < outputNumber; out++)
        {
            this->ptrO[out] = other.ptrO[out];
            this->strideO[out] = other.strideO[out];
        }
    }

template<typename OtherAlgebra>
    inline void copyPos(const OtherAlgebra &other)
    {
        for (int in = 0; in < inputNumber; in++)
        {
            this->setInputPos (other.getInputPos(in), in);
        }
        for (int out = 0; out < outputNumber; out++)
        {
            this->setOutputPos(other.getOutputPos(out), out);
        }
    }

    inline static int step()
    {
        return stepSize;
    }

    inline void setInputStride(int _strideI, int input = 0) {
        strideI[input] = _strideI;
    }

    inline void setOutputStride(int _strideO, int output = 0) {
        strideO[output] = _strideO;
    }

    inline void setInputPos(void *_ptrI, int input = 0) {
        ptrI[input] = (InternalInputType *)_ptrI;
    }

    inline void setOutputPos(void *_ptrO,  int output = 0) {
        ptrO[output] = (InternalOutputType *)_ptrO;
    }

    inline InternalInputType * getInputPos(int input = 0) const {
        return ptrI[input];
    }

    inline InternalOutputType * getOutputPos(int output = 0) const {
        return ptrO[output];
    }

    inline InternalInputType * getInputPos(int input, int y, int x) const {
        //printf(" %p %p\n", this->ptrI[0], this->ptrI[1]);
        return this->ptrI[input] + y * this->strideI[input]  + x;
    }

    inline InternalOutputType * getOutputPos(int output, int y, int x) const {
        return this->ptrO[output] + y * this->strideO[output] + x;
    }

    inline void advance()
    {
        for (int in = 0; in < inputNumber; in++)
        {
            ptrI[in] += stepSize;
        }

        for (int out = 0; out < outputNumber; out++)
        {
            ptrO[out] += stepSize;
        }
    }
};


} //namespace corecvs
#endif  //BASEALGEBRA_H_
