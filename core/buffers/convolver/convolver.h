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
#include "utils/global.h"
#include "math/matrix/matrix.h"

#include "buffers/abstractBuffer.h"
#include "buffers/abstractKernel.h"

#include "buffers/rgb24/rgb24Buffer.h"
#include "buffers/float/dpImage.h"

namespace corecvs {

class Convolver
{
public:
    enum ConvolverImplementation {
        ALGORITHM_NAIVE,
        ALGORITHM_SSE_DMITRY,

        /**/
        ALGORITHM_SSE_UNROLL_TEMPLATE,

        ALGORITHM_SSE_UNROLL_1 = ALGORITHM_SSE_UNROLL_TEMPLATE,
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
        ALGORITHM_SSE_FASTKERNEL_EXP3,

        ALGORITHM_SSE_WRAPPERS,
        ALGORITHM_SSE_WRAPPERS_UNROLL_1,
        ALGORITHM_SSE_WRAPPERS_UNROLL_5,
        ALGORITHM_SSE_WRAPPERS_UNROLL_10,

        ALGORITHM_SSE_WRAPPERS_EX_UNROLL_1,
        ALGORITHM_SSE_WRAPPERS_EX_UNROLL_2,

        ALGORITHM_LAST
    };

    static const char *getName(const ConvolverImplementation &value);

    bool trace = true;
    bool parallel = true;

    Convolver(bool trace = false, bool parallel = true) :
        trace(trace), parallel(parallel)
    {
    }

    void convolve(DpImage &src, DpKernel &kernel, DpImage &dst, ConvolverImplementation impl = ALGORITHM_SSE_DMITRY);
    void convolve(FpImage &src, FpKernel &kernel, FpImage &dst, ConvolverImplementation impl = ALGORITHM_SSE_UNROLL_8);

    void convolveIB(DpImage &src, DpKernel &kernel, DpImage &dst, ConvolverImplementation impl = ALGORITHM_SSE_DMITRY);
    void convolveIB(FpImage &src, FpKernel &kernel, FpImage &dst, ConvolverImplementation impl = ALGORITHM_SSE_UNROLL_8);


    /* Double Implementations */
    /**
     * This uses AbstractBuffer::doConvolve()
     **/
    void naiveConvolutor       (DpImage &src, DpKernel &kernel, DpImage &dst);

    /**
     *  So far best implementation
     *
     *  This convolutor does parallel call between lines using multithearing of your platfrom.
     *  Within each line 20 values are processed with 5 AVX registers.
     *
     *  This code effective with AVX only
     **/
    void unrolledConvolutor    (DpImage &src, DpKernel &kernel, DpImage &dst);

template<int UNROLL = 5>
    void unrolledAutoConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst);

    void fastkernelConvolutor  (DpImage &src, DpKernel &kernel, DpImage &dst);

#ifdef WITH_AVX
    void fastkernelConvolutorExp(DpImage &src, DpKernel &kernel, DpImage &dst);
    void fastkernelConvolutorExp3(DpImage &src, DpKernel &kernel, DpImage &dst);
#endif

#if WITH_AVX
    void wrapperConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst);

template<int UNROLL = 5>
    void unrolledWrapperConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst);
template<int UNROLL = 5>
    void unrolledWrapperExConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst);
#endif


    /* Float implementations */
    void naiveConvolutor          (FpImage &src, FpKernel &kernel, FpImage &dst);
#ifdef WITH_AVX
    void fastkernelConvolutorExp3 (FpImage &src, FpKernel &kernel, FpImage &dst);
template<int UNROLL = 5>
    void unrolledWrapperConvolutor(FpImage &src, FpKernel &kernel, FpImage &dst);
#endif

    /**
     * This is used not too init the whole image
     *
     * If performance is issue we could try to patch convolver methods
     **/
    static void fillPerimeter(FpKernel &kernel, FpImage &dst, float value = 0);

};

} //namespace corecvs

#endif // CONVOLVER_H
