#ifndef SCALARALGEBRA_H_
#define SCALARALGEBRA_H_
/**
 * \file scalarAlgebra.h
 * \brief a header for ScalarAlgebra.c
 *
 * \ingroup cppcorefiles
 * \date Sep 26, 2010
 * \author alexander
 */

#include "global.h"

#include "baseAlgebra.h"
#include "genericMath.h"

#ifdef TRACE_SCALAR_ALGEBRA
#define DOTRACE_ALG(X) SYNC_PRINT(X)
#else
#define DOTRACE_ALG(X)
#endif

namespace corecvs {

/**
 *  \brief  ScalarAlgebraMulti is a crucial part of fastKernels
 *
 *   This algebra allows fallback to generic types such as int, double, float ...
 *
 *
 *
 **/
template<
    typename _InputTraits,
    typename _OutputTraits = _InputTraits,

    int inputNumber = 1,
    int outputNumber = 1
>
class ScalarAlgebraMulti : public BaseAlgebraMulti<
                                    typename _InputTraits::InternalType,
                                    typename _OutputTraits::InternalType,
                                    1, inputNumber, outputNumber
                           >,
                           public GenericMath<typename _InputTraits::InternalType>
{
public:
    STATIC_ASSERT((_InputTraits ::step == 1), scalar_algebra_supports_only_single_step_input);
    STATIC_ASSERT((_OutputTraits::step == 1), scalar_algebra_supports_only_single_step_output);

    typedef _InputTraits  InputTraits;
    typedef _OutputTraits OutputTraits;


    typedef ScalarAlgebraMulti FallbackAlgebra;
    typedef BaseAlgebraMulti<
                typename _InputTraits::InternalType,
                typename _OutputTraits::InternalType,
                1, inputNumber, outputNumber
            > BaseClass;


    /**
     * This algebra is scalar because it's internal type is equal to the processing type
     **/
    typedef typename InputTraits::Type        InputType;
    typedef typename InputTraits::SignedType  InputSignedType;

    typedef typename OutputTraits::Type       OutputType;
    typedef typename OutputTraits::SignedType OutputSignedType;

    ScalarAlgebraMulti() {}

    /**
     *  Copy constructor. It is absolutely crucial to overload copy constructor
     *  if you want to use OpenMP.
     **/
    ScalarAlgebraMulti(const ScalarAlgebraMulti &other) :
        BaseClass(other)
    {
        DOTRACE_ALG(("Scalar Algebra Copy constructor called (in %d, out %d)\n", inputNumber, outputNumber));
    }

    /* Multi Buffer variant  */
    inline InputType getInput(int input, int y, int x)
    {
        DOTRACE_ALG(("Read  SC (%d)[%d x %d] %x %d\n", input, x, y, this->ptrI[input], this->strideI[input]));
        return *BaseClass::getInputPos(input, y, x);
    }

    inline void putOutput(int output, int y, int x, const OutputType &result)
    {
        DOTRACE_ALG(("Write SC (%d)[%d x %d] %x %d\n", output, x, y, this->ptrO[output], this->strideO[output]));
        *BaseClass::getOutputPos(output, y, x) = result;
    }

    /* Single Buffer variant */
    inline InputType getInput(int y, int x)
    {
        return *(this->ptrI[0] + y * this->strideI[0]  + x);
    }

    inline void putOutput(int y, int x, const OutputType &result)
    {
        *(this->ptrO[0] + y * this->strideO[0] + x) = result;
    }

/* Just in case template version*/
template<int y, int x>
    inline InputType getInput()
    {
        return *(this->ptrI[0] + y * this->strideI[0]  + x);
    }

template<int y, int x>
    inline void putOutput(const OutputType &result)
    {
        *(this->ptrO[0] + y * this->strideO[0] + x) = result;
    }

};

} //namespace corecvs
#endif  //SCALARALGEBRA_H_
