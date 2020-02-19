#ifndef VECTOR_ALGEBRA_H_
#define VECTOR_ALGEBRA_H_
/**
 * \file vectorAlgebra.h
 * \brief a header for VectorAlgebra.c
 *
 * \ingroup cppcorefiles
 * \date Sep 26, 2010
 * \author alexander
 */

#include "utils/global.h"

#include "buffers/kernels/fastkernel/baseAlgebra.h"
#include "buffers/kernels/fastkernel/scalarAlgebra.h"

#ifdef WITH_SSE
#include "math/sse/sseWrapper.h"
#endif

namespace corecvs {

#ifdef WITH_SSE

template<
    class _Traits,
    int inputNumber = 1,
    int outputNumber = 1
>
class VectorAlgebraMulti : public BaseAlgebraMulti<
                                typename _Traits::InternalType,
                                typename _Traits::InternalType,
                                _Traits::step,
                                inputNumber,
                                outputNumber
                           >,
                           public SSEMath
{
public:
    typedef _Traits  Traits;

    typedef  VectorAlgebraMulti<Traits, inputNumber, outputNumber> Type;

    typedef  BaseAlgebraMulti<
            typename _Traits::InternalType,
            typename _Traits::InternalType,
            _Traits::step,
            inputNumber,
            outputNumber
            > BaseClass;

    typedef ScalarAlgebraMulti<
            typename Traits::FallbackTraits,
            typename Traits::FallbackTraits,
            inputNumber,
            outputNumber> FallbackAlgebra;

    typedef typename Traits::Type       InputType;
    typedef typename Traits::SignedType InputSignedType;
    typedef typename Traits::Type       OutputType;
    typedef typename Traits::SignedType OutputSignedType;

    VectorAlgebraMulti(){}
    VectorAlgebraMulti(const VectorAlgebraMulti &other) : BaseClass(other){}


    /* Multi Buffer variant  */
    ALIGN_STACK_SSE inline InputType getInput(const int input, const int y, const int x)
    {
        return InputType(BaseClass::getInputPos(input, y, x));
    }

    ALIGN_STACK_SSE inline void putOutput(const int output, const int y, const int x, const OutputType &result)
    {
        result.save(BaseClass::getOutputPos(output, y, x));
    }

    /* Single Buffer variant */
    ALIGN_STACK_SSE inline InputType getInput(const int y, const int x)
    {
        return InputType(BaseClass::getInputPos(0, y, x));
    }

    ALIGN_STACK_SSE inline void putOutput(const int y, const int x, const OutputType &result)
    {
        result.save(BaseClass::getOutputPos(0, y, x));
    }

/* Just in case template version*/
template<int y, int x>
    ALIGN_STACK_SSE inline InputType getInput()
    {
        return InputType(BaseClass::getInputPos(0, y, x));
    }

template<int y, int x>
    ALIGN_STACK_SSE inline void putOutput(const OutputType &result)
    {
        result.save(BaseClass::getOutputPos(0, y, x));
    }
};


/**
 *  This version of VectorAlgebra is using streaming to save values
 **/
template<
    class _Traits,
    int inputNumber = 1,
    int outputNumber = 1
>
class VectorAlgebraMultiStreaming : public VectorAlgebraMulti<_Traits, inputNumber, outputNumber>
{
public:
    typedef  VectorAlgebraMultiStreaming<_Traits, inputNumber, outputNumber> Type;
    typedef  VectorAlgebraMulti<_Traits, inputNumber, outputNumber> BaseClass;

    typedef typename BaseClass::InputType  InputType;
    typedef typename BaseClass::OutputType OutputType;

    VectorAlgebraMultiStreaming(){}
    VectorAlgebraMultiStreaming(const VectorAlgebraMultiStreaming &other) : BaseClass(other){}

    ALIGN_STACK_SSE inline void putOutput(const int output, const int y, const int x, const OutputType &result)
    {
        result.streamAligned(BaseClass::getOutputPos(output, y, x));
    }

    ALIGN_STACK_SSE inline void putOutput(const int y, const int x, const OutputType &result)
    {
        result.streamAligned(BaseClass::getOutputPos(0, y, x));
    }

/* Just in case template version*/
template<int y, int x>
    ALIGN_STACK_SSE inline void putOutput(const OutputType &result)
    {
        result.streamAligned(BaseClass::getOutputPos(0, y, x));
    }

};

#endif // WITH_SSE

} //namespace corecvs
#endif  //VECTOR_ALGEBRA_H_

