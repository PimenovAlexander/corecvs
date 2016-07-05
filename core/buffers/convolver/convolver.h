#ifndef CONVOLVER_H
#define CONVOLVER_H

/**
 * \file convolver.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 24, 2010
 * \author alexander
 */

#include "global.h"
#include "matrix.h"

#include "abstractBuffer.h"
#include "abstractKernel.h"

#include "rgb24Buffer.h"

namespace corecvs {


typedef AbstractBuffer<double> DpImage;
typedef AbstractKernel<double> DpKernel;

typedef AbstractBuffer<float> FpImage;
typedef AbstractKernel<float> FpKernel;


class Convolver
{
public:
    enum ConvolverImplementation {
        ALGORITHM_NAIVE,
        ALGORITHM_SSE_DMITRY,

        /**/
        ALGORITHM_SSE_UNROLL_TEMPLATE,

        ALGORITHM_SSE_UNROLL_1,
        ALGORITHM_SSE_UNROLL_2,
        ALGORITHM_SSE_UNROLL_3,
        ALGORITHM_SSE_UNROLL_4,
        ALGORITHM_SSE_UNROLL_5,
        ALGORITHM_SSE_UNROLL_6,
        ALGORITHM_SSE_UNROLL_7,
        ALGORITHM_SSE_UNROLL_8,
        ALGORITHM_SSE_UNROLL_9,
        ALGORITHM_SSE_UNROLL_10,
        ALGORITHM_SSE_UNROLL_12,
        ALGORITHM_SSE_UNROLL_16,
        ALGORITHM_SSE_UNROLL_20,
        ALGORITHM_SSE_UNROLL_40,
        ALGORITHM_SSE_UNROLL_100,
        ALGORITHM_SSE_UNROLL_LAST,

        ALGORITHM_SSE_DMITRY_SIMP = ALGORITHM_SSE_UNROLL_5,

        ALGORITHM_SSE_FASTKERNEL = ALGORITHM_SSE_UNROLL_LAST,
        ALGORITHM_SSE_FASTKERNEL_EXP,
        ALGORITHM_SSE_FASTKERNEL_EXP5,

        ALGORITHM_SSE_WRAPPERS,
        ALGORITHM_SSE_WRAPPERS_UNROLL_1,
        ALGORITHM_SSE_WRAPPERS_UNROLL_5,
        ALGORITHM_SSE_WRAPPERS_UNROLL_10,

        ALGORITHM_SSE_WRAPPERS_EX_UNROLL_1,
        ALGORITHM_SSE_WRAPPERS_EX_UNROLL_2,

        ALGORITHM_LAST
    };

    Convolver();

    static void convolve(DpImage &src, DpKernel &kernel, DpImage &dst, ConvolverImplementation impl = ALGORITHM_SSE_DMITRY);
    static void convolve(FpImage &src, FpKernel &kernel, FpImage &dst, ConvolverImplementation impl = ALGORITHM_SSE_UNROLL_8);


    /* Double Implementations */
    static void naiveConvolutor       (DpImage &src, DpKernel &kernel, DpImage &dst);
    static void unrolledConvolutor    (DpImage &src, DpKernel &kernel, DpImage &dst);

template<int UNROLL = 5>
    static void unrolledAutoConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst);

    static void fastkernelConvolutor  (DpImage &src, DpKernel &kernel, DpImage &dst);

#ifdef WITH_AVX
    static void fastkernelConvolutorExp(DpImage &src, DpKernel &kernel, DpImage &dst);
    static void fastkernelConvolutorExp5(DpImage &src, DpKernel &kernel, DpImage &dst);
#endif

#if WITH_AVX
    static void wrapperConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst);

template<int UNROLL = 5>
    static void unrolledWrapperConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst);
template<int UNROLL = 5>
    static void unrolledWrapperExConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst);
#endif


    /* Float implementations */
    static void naiveConvolutor          (FpImage &src, FpKernel &kernel, FpImage &dst);
#ifdef WITH_AVX
    static void fastkernelConvolutorExp5 (FpImage &src, FpKernel &kernel, FpImage &dst);
template<int UNROLL = 5>
    static void unrolledWrapperConvolutor(FpImage &src, FpKernel &kernel, FpImage &dst);
#endif

};

} //namespace corecvs

#endif // CONVOLVER_H
