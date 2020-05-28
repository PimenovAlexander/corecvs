#include "buffers/convolver/convolver.h"

#include "utils/global.h"
#include "buffers/kernels/fastkernel/fastKernel.h"
#include "buffers/kernels/arithmetic.h"

#include "math/sse/sseWrapper.h"
#include "buffers/kernels/fastkernel/vectorTraits.h"

namespace corecvs {

struct BaseConvolutorImpl {

    BaseConvolutorImpl(DpImage *src, DpImage *dst, DpKernel *kernel) : src(src), dst(dst), kernel(kernel)
    {
        h = src->h;
        w = src->w;
        kw = kernel->w;
        kh = kernel->h;
        kx = kernel->x;
        ky = kernel->y;

        t = 0; l = 0; r = w; d = h;

        t = std::max(t, ky);
        l = std::max(l, kx);
        d = std::min(d, h - kh + ky);
        r = std::min(r, w - kw + kx);
    }
    DpImage *src;      /**< Source image            */
    DpImage *dst;      /**< Destination image       */
    DpKernel *kernel;  /**< Kernel to convolve with */
    int h, w, kw, kh, kx, ky, t, l, d, r;
};

#if WITH_AVX


struct ConvolutorImplIntUnroll5 : public BaseConvolutorImpl
{
    void operator() (const corecvs::BlockedRange<int> &rr) const
    {
        for (int i = rr.begin(); i < rr.end(); ++i)
        {            
            // SYNC_PRINT(("ConvolutorImplIntUnroll5(): line %d (%d %d)\n", i, l, r));

            int j = l;
            for (; j + 19 < r; j += 20)
            {
                __m256d sum0= _mm256_setzero_pd();
                __m256d sum1= _mm256_setzero_pd();
                __m256d sum2= _mm256_setzero_pd();
                __m256d sum3= _mm256_setzero_pd();
                __m256d sum4= _mm256_setzero_pd();

                for (int ii = 0; ii < kh; ++ii)
                {
                    double *kp = &kernel->element(ii, 0);
                    double *ip0 = &src->element(ii + i - ky, j - kx);
                    double *ip1 = ip0 +  4;
                    double *ip2 = ip0 +  8;
                    double *ip3 = ip0 + 12;
                    double *ip4 = ip0 + 16;
                    for (int jj = 0; jj < kw; ++jj)
                    {
                        __m256d mul1 = _mm256_broadcast_sd(kp++);
                        __m256d mul20= _mm256_loadu_pd(ip0++);
#ifdef WITH_FMA
                        sum0 = _mm256_fmadd_pd(mul1, mul20, sum0);
#else
                        sum0 = _mm256_add_pd(_mm256_mul_pd(mul1, mul20), sum0);
#endif
                        __m256d mul21= _mm256_loadu_pd(ip1++);
#ifdef WITH_FMA
                        sum1 = _mm256_fmadd_pd(mul1, mul21, sum1);
#else
                        sum1 = _mm256_add_pd(_mm256_mul_pd(mul1, mul21), sum1);
#endif
                        __m256d mul22= _mm256_loadu_pd(ip2++);
#ifdef WITH_FMA
                        sum2 = _mm256_fmadd_pd(mul1, mul22, sum2);
#else
                        sum2 = _mm256_add_pd(_mm256_mul_pd(mul1, mul22), sum2);
#endif
                        __m256d mul23= _mm256_loadu_pd(ip3++);
#ifdef WITH_FMA
                        sum3 = _mm256_fmadd_pd(mul1, mul23, sum3);
#else
                        sum3 = _mm256_add_pd(_mm256_mul_pd(mul1, mul23), sum3);
#endif
                        __m256d mul24= _mm256_loadu_pd(ip4++);
#ifdef WITH_FMA
                        sum4 = _mm256_fmadd_pd(mul1, mul24, sum4);
#else
                        sum4 = _mm256_add_pd(_mm256_mul_pd(mul1, mul24), sum4);
#endif
                    }
                }
                _mm256_storeu_pd(&dst->element(i, j     ), sum0);
                _mm256_storeu_pd(&dst->element(i, j +  4), sum1);
                _mm256_storeu_pd(&dst->element(i, j +  8), sum2);
                _mm256_storeu_pd(&dst->element(i, j + 12), sum3);
                _mm256_storeu_pd(&dst->element(i, j + 16), sum4);
            }

            for (; j < r; j ++)
            {
                double sum = 0;
                for (int ii = 0; ii < kh; ii++)
                {
                    for (int jj = 0; jj < kw; jj++)
                    {
                        sum += kernel->element(ii, jj) * src->element(ii + i - ky, jj + j - kx);
                    }
                }
                dst->element(i, j) = sum;
            }

        }
    }

    ConvolutorImplIntUnroll5(DpImage *src, DpImage *dst, DpKernel *kernel) :
        BaseConvolutorImpl(src, dst, kernel)
    {
    }

};

/**
 * Same as above but we trust compiler to do the unroll
 **/
template<int UNROLL = 5>
struct ConvolutorImplIntUnrollAuto
{
    static const int SPRINT = UNROLL * 4;
    void operator() (const corecvs::BlockedRange<int> &rr) const
    {
        // SYNC_PRINT(("ConvolutorImplIntUnrollAuto:operator(%d %d): called\n", rr.begin(), rr.end()));
        for (int i = rr.begin(); i < rr.end(); i++)
        {
            int j = l;
            for (; j + SPRINT - 1 < r; j += SPRINT)
            {
                __m256d sum[UNROLL];
                for (int r = 0; r < UNROLL; r++) {
                    sum[r] = _mm256_setzero_pd();
                }

                for (int ii = 0; ii < kh; ++ii)
                {
                    double *kp = &kernel->element(ii, 0);
                    double *ip[UNROLL];

                    ip[0] = &src->element(ii + i - ky, j - kx);

                    for (int r = 0; r < UNROLL - 1; r++) {
                        ip[r + 1] = ip[r] +  4;
                    }

                    for (int jj = 0; jj < kw; ++jj)
                    {
                        __m256d mul1 = _mm256_broadcast_sd(kp++);
                        for (int r = 0; r < UNROLL; r++) {
                            __m256d mul2x = _mm256_loadu_pd(ip[r]++);
#ifdef WITH_FMA
                            sum[r] = _mm256_fmadd_pd(mul1, mul2x, sum[r]);
#else
                            sum[r] = _mm256_add_pd(_mm256_mul_pd(mul1, mul2x), sum[r]);
#endif
                        }
                    }
                }

                for (int r = 0; r < UNROLL; r++) {
                    _mm256_storeu_pd(&dst->element(i, j +  4 * r), sum[r]);
                }
            }
            for (; j < r; j ++)
            {
                double sum = 0;
                for (int ii = 0; ii < kh; ii++)
                {
                    for (int jj = 0; jj < kw; jj++)
                    {
                        sum += kernel->element(ii, jj) * src->element(ii + i - ky, jj + j - kx);
                    }
                }
                dst->element(i, j) = sum;
            }
        }
    }

    ConvolutorImplIntUnrollAuto(DpImage *src, DpImage *dst, DpKernel *kernel) : src(src), dst(dst), kernel(kernel)
    {
        h = src->h;
        w = src->w;
        kw = kernel->w;
        kh = kernel->h;
        kx = kernel->x;
        ky = kernel->y;
        t = 0; l = 0; r = w; d = h;

        t = std::max(t, ky);
        l = std::max(l, kx);
        d = std::min(d, h - kh + ky);
        r = std::min(r, w - kw + kx);
    }
    DpImage *src;
    DpImage *dst;
    DpKernel *kernel;
    int h, w, kw, kh, kx, ky, t, l, d, r;
};


/**
 * Same as top one but with ssewrappes
 **/
struct ConvolutorImplWrappers
{
    void operator() (const corecvs::BlockedRange<int> &rr) const
    {
        for (int i = rr.begin(); i < rr.end(); ++i)
        {
            int j = l;
            for (; j + 19 < r; j += 20)
            {
                Doublex4 sum0 = Doublex4::Zero();
                Doublex4 sum1 = Doublex4::Zero();
                Doublex4 sum2 = Doublex4::Zero();
                Doublex4 sum3 = Doublex4::Zero();
                Doublex4 sum4 = Doublex4::Zero();

                for (int ii = 0; ii < kh; ++ii)
                {
                    double *kp = &kernel->element(ii, 0);
                    double *ip0 = &src->element(ii + i - ky, j - kx);
                    double *ip1 = ip0 +  4;
                    double *ip2 = ip0 +  8;
                    double *ip3 = ip0 + 12;
                    double *ip4 = ip0 + 16;
                    for (int jj = 0; jj < kw; ++jj)
                    {
                        Doublex4 mul1 = Doublex4::Broadcast(kp++);
                        Doublex4 mul20(ip0++);
                        sum0 = multiplyAdd(mul1, mul20, sum0);

                        Doublex4 mul21(ip1++);
                        sum1 = multiplyAdd(mul1, mul21, sum1);

                        Doublex4 mul22(ip2++);
                        sum2 = multiplyAdd(mul1, mul22, sum2);

                        Doublex4 mul23(ip3++);
                        sum3 = multiplyAdd(mul1, mul23, sum3);

                        Doublex4 mul24(ip4++);
                        sum4 = multiplyAdd(mul1, mul24, sum4);

                    }
                }

                sum0.save(&dst->element(i, j      ));
                sum1.save(&dst->element(i, j + 4  ));
                sum2.save(&dst->element(i, j + 8  ));
                sum3.save(&dst->element(i, j + 12 ));
                sum4.save(&dst->element(i, j + 16 ));
            }
            for (; j < r; j ++)
            {
                double sum = 0;
                for (int ii = 0; ii < kh; ii++)
                {
                    for (int jj = 0; jj < kw; jj++)
                    {
                        sum += kernel->element(ii, jj) * src->element(ii + i - ky, jj + j - kx);
                    }
                }
                dst->element(i, j) = sum;
            }
        }
    }

    ConvolutorImplWrappers(DpImage *src, DpImage *dst, DpKernel *kernel) : src(src), dst(dst), kernel(kernel)
    {
        h = src->h; w = src->w; kw = kernel->w; kh = kernel->h; kx = kernel->x; ky = kernel->y;
        t = 0; l = 0; r = w; d = h;

        t = std::max(t, ky);
        l = std::max(l, kx);
        d = std::min(d, h - kh + ky);
        r = std::min(r, w - kw + kx);
    }
    DpImage *src, *dst;
    DpKernel *kernel;
    int h, w, kw, kh, kx, ky, t, l, d, r;
};

/**
 * Same as above. But with unrolls
 **/
template <int UNROLL = 5>
struct ConvolutorImplWrappersUnroll
{
    static const int SPRINT = UNROLL * 4;

    void operator() (const corecvs::BlockedRange<int> &rr) const
    {
        for (int i = rr.begin(); i < rr.end(); ++i)
        {
            int j = l;
            for (; j + SPRINT - 1 < r; j += SPRINT)
            {
                Doublex4 sum[UNROLL];
                for (int r = 0; r < UNROLL; r++) {
                    sum[r] = Doublex4::Zero();
                }

                for (int ii = 0; ii < kh; ++ii)
                {
                    double *kp = &kernel->element(ii, 0);
                    double *ip[UNROLL];
                    ip[0] = &src->element(ii + i - ky, j - kx);
                    for (int r = 0; r < UNROLL - 1; r++) {
                        ip[r + 1] = ip[r] +  4;
                    }
                    for (int jj = 0; jj < kw; ++jj)
                    {
                        Doublex4 mul1 = Doublex4::Broadcast(kp++);

                        for (int r = 0; r < UNROLL; r++) {
                            Doublex4 mul2x(ip[r]++);
                            sum[r] = multiplyAdd(mul1, mul2x, sum[r]);
                        }
                    }
                }

                for (int r = 0; r < UNROLL; r++) {
                    sum[r].save(&dst->element(i, j + 4 * r));
                }
            }
            for (; j < r; j ++)
            {
                double sum = 0;
                for (int ii = 0; ii < kh; ii++)
                {
                    for (int jj = 0; jj < kw; jj++)
                    {
                        sum += kernel->element(ii, jj) * src->element(ii + i - ky, jj + j - kx);
                    }
                }
                dst->element(i, j) = sum;
            }
        }
    }

    ConvolutorImplWrappersUnroll(DpImage *src, DpImage *dst, DpKernel *kernel) : src(src), dst(dst), kernel(kernel)
    {
        h = src->h; w = src->w; kw = kernel->w; kh = kernel->h; kx = kernel->x; ky = kernel->y;
        t = 0; l = 0; r = w; d = h;

        t = std::max(t, ky);
        l = std::max(l, kx);
        d = std::min(d, h - kh + ky);
        r = std::min(r, w - kw + kx);
    }
    DpImage *src, *dst;
    DpKernel *kernel;
    int h, w, kw, kh, kx, ky, t, l, d, r;
};

/* Internal unroll with extended type */
template <int UNROLL = 5>
struct ConvolutorImplWrappersExUnroll : public BaseConvolutorImpl
{
    static const int SPRINT = UNROLL * Doublex8::SIZE;

    void operator() (const corecvs::BlockedRange<int> &rr) const
    {
        for (int i = rr.begin(); i < rr.end(); ++i)
        {
            int j = l;
            for (; j + SPRINT - 1 < r; j += SPRINT)
            {
                Doublex8 sum[UNROLL];
                for (int r = 0; r < UNROLL; r++) {
                    sum[r] = Doublex8::Zero();
                }

                for (int ii = 0; ii < kh; ++ii)
                {
                    double *kp = &kernel->element(ii, 0);
                    double *ip[UNROLL];
                    ip[0] = &src->element(ii + i - ky, j - kx);
                    for (int r = 0; r < UNROLL - 1; r++) {
                        ip[r + 1] = ip[r] +  Doublex8::SIZE;
                    }
                    for (int jj = 0; jj < kw; ++jj)
                    {
                        Doublex8 mul1 = Doublex8::Broadcast(kp++);

                        for (int r = 0; r < UNROLL; r++) {
                            Doublex8 mul2x(ip[r]++);
                            sum[r] = multiplyAdd(mul1, mul2x, sum[r]);
                        }
                    }
                }

                for (int r = 0; r < UNROLL; r++) {
                    sum[r].save(&dst->element(i, j + Doublex8::SIZE * r));
                }
            }
            for (; j < r; j ++)
            {
                double sum = 0;
                for (int ii = 0; ii < kh; ii++)
                {
                    for (int jj = 0; jj < kw; jj++)
                    {
                        sum += kernel->element(ii, jj) * src->element(ii + i - ky, jj + j - kx);
                    }
                }
                dst->element(i, j) = sum;
            }

        }
    }

    ConvolutorImplWrappersExUnroll(DpImage *src, DpImage *dst, DpKernel *kernel) :
        BaseConvolutorImpl(src, dst, kernel)
    {}
};

#endif // WITH_AVX

#if WITH_AVX

void Convolver::unrolledConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    if (trace) SYNC_PRINT(("Convolver::unrolledConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst): called\n"));
    ConvolutorImplIntUnroll5 impl(&src, &dst, &kernel);
    corecvs::parallelable_for(impl.t, impl.d, impl, parallel);
}

template<int UNROLL>
void Convolver::unrolledAutoConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    if (trace) SYNC_PRINT(("Convolver<%d>::unrolledAutoConvolutor(): called %s parallel\n",
                           UNROLL, parallel ? "with" : "without"));

    ConvolutorImplIntUnrollAuto<UNROLL> impl(&src, &dst, &kernel);
    corecvs::parallelable_for(impl.t, impl.d, impl, parallel);
}

void Convolver::wrapperConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    ConvolutorImplWrappers impl(&src, &dst, &kernel);
    corecvs::parallelable_for(impl.t, impl.d, impl);
}

template<int UNROLL>
void Convolver::unrolledWrapperConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    if (trace) SYNC_PRINT(("Convolver::unrolledWrapperConvolutor<%d>(DpImage): called %s parallel\n",
                           UNROLL, parallel ? "with" : "without"));

    ConvolutorImplWrappersUnroll<UNROLL> impl(&src, &dst, &kernel);
    corecvs::parallelable_for(impl.t, impl.d, impl);
}

template<int UNROLL>
void Convolver::unrolledWrapperExConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    ConvolutorImplWrappersExUnroll<UNROLL> impl(&src, &dst, &kernel);
    corecvs::parallelable_for(impl.t, impl.d, impl);
}

#endif // WITH_AVX

/**
 *  Fast kernel implemenation
 **/

void Convolver::fastkernelConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    ConvolveKernel<DummyAlgebra> convKernel(&kernel, kernel.y, kernel.x);

    DpImage *in  = &src;
    DpImage *out = &dst;

    BufferProcessor<DpImage, DpImage, ConvolveKernel, AlgebraDouble> proScalar;
    proScalar.process(&in, &out, convKernel);
}

#ifdef WITH_AVX

void Convolver::fastkernelConvolutorExp(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    ConvolveKernel<DummyAlgebra> convKernel(&kernel, kernel.y, kernel.x);

    DpImage *in  = &src;
    DpImage *out = &dst;

    BufferProcessor<DpImage, DpImage, ConvolveKernel, VectorAlgebraDoubleEx> proScalar;
    proScalar.process(&in, &out, convKernel);
}

void Convolver::fastkernelConvolutorExp3(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    ConvolveKernel<DummyAlgebra> convKernel(&kernel, kernel.y, kernel.x);

    DpImage *in  = &src;
    DpImage *out = &dst;

    BufferProcessor<DpImage, DpImage, ConvolveKernel, VectorAlgebraDoubleEx3> proScalar;
    proScalar.process(&in, &out, convKernel);
}

void Convolver::fastkernelConvolutorExp3(FpImage &src, FpKernel &kernel, FpImage &dst)
{
    if (trace) SYNC_PRINT(("Convolver::fastkernelConvolutorExp3(FpImage): called %s parallel\n", parallel ? "with" : "without"));
    FloatConvolveKernel<DummyAlgebra> convKernel(&kernel, kernel.y, kernel.x);

    FpImage *in  = &src;
    FpImage *out = &dst;

    BufferProcessor<FpImage, FpImage, FloatConvolveKernel, VectorAlgebraFloatEx3> proScalar;
    proScalar.process(&in, &out, convKernel);
}

#endif // WITH_AVX



void Convolver::naiveConvolutor(DpImage &src, DpKernel &kernel, DpImage &dst)
{
    if (trace) SYNC_PRINT(("Convolver::naiveConvolutor(DpImage)\n"));
    src.doConvolve<DpImage>(&dst, &kernel, true);
}

void Convolver::naiveConvolutor(FpImage &src, FpKernel &kernel, FpImage &dst)
{
    if (trace) SYNC_PRINT(("Convolver::naiveConvolutor(FpImage)\n"));
    src.doConvolve<FpImage>(&dst, &kernel, true);
}

void Convolver::convolve(DpImage &src, DpKernel &kernel, DpImage &dst, Convolver::ConvolverImplementation impl)
{
    switch (impl) {
    default:
    case ALGORITHM_NAIVE:
        naiveConvolutor(src, kernel, dst);  return;

#ifdef WITH_AVX
    case ALGORITHM_SSE_DMITRY:
        unrolledConvolutor(src, kernel, dst);  return;

    case ALGORITHM_SSE_UNROLL_1:
        unrolledAutoConvolutor<1>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_2:
        unrolledAutoConvolutor<2>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_3:
        unrolledAutoConvolutor<3>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_4:
        unrolledAutoConvolutor<4>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_5:
        unrolledAutoConvolutor<5>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_6:
        unrolledAutoConvolutor<6>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_7:
        unrolledAutoConvolutor<7>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_8:
        unrolledAutoConvolutor<8>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_9:
        unrolledAutoConvolutor<9>(src, kernel, dst);  return;
    case ALGORITHM_SSE_UNROLL_10:
        unrolledAutoConvolutor<10>(src, kernel, dst);  return;


    case ALGORITHM_SSE_UNROLL_12:
                unrolledAutoConvolutor<12>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_16:
                unrolledAutoConvolutor<16>(src, kernel, dst);  return;


        case ALGORITHM_SSE_UNROLL_20:
                unrolledAutoConvolutor<20>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_40:
                unrolledAutoConvolutor<40>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_100:
                unrolledAutoConvolutor<100>(src, kernel, dst);  return;
#endif


#ifdef WITH_AVX
        case ALGORITHM_SSE_WRAPPERS:
            wrapperConvolutor(src, kernel, dst);  return;

        case ALGORITHM_SSE_WRAPPERS_UNROLL_1:
                unrolledWrapperConvolutor<1>(src, kernel, dst);  return;
        case ALGORITHM_SSE_WRAPPERS_UNROLL_5:
                unrolledWrapperConvolutor<5>(src, kernel, dst);  return;
        case ALGORITHM_SSE_WRAPPERS_UNROLL_10:
                unrolledWrapperConvolutor<10>(src, kernel, dst);  return;

        case ALGORITHM_SSE_WRAPPERS_EX_UNROLL_1:
                unrolledWrapperExConvolutor<1>(src, kernel, dst);  return;
        case ALGORITHM_SSE_WRAPPERS_EX_UNROLL_2:
                unrolledWrapperExConvolutor<2>(src, kernel, dst);  return;
#endif

        case ALGORITHM_SSE_FASTKERNEL:
                fastkernelConvolutor(src, kernel, dst);  return;

#ifdef WITH_AVX
        case ALGORITHM_SSE_FASTKERNEL_EXP:
                fastkernelConvolutorExp(src, kernel, dst);  return;
        case ALGORITHM_SSE_FASTKERNEL_EXP3:
                fastkernelConvolutorExp3(src, kernel, dst);  return;
#endif

    }
}


void Convolver::convolveIB(DpImage &src, DpKernel &kernel, DpImage &dst, Convolver::ConvolverImplementation impl)
{
    if (trace) SYNC_PRINT(("Convolver::convolveIB(DpImage)\n"));

    convolve(src, kernel, dst, impl);

    MulAddConstKernel<DummyAlgebra> mulAddKernel(kernel.bias, 1.0 / kernel.invFactor);
    BufferProcessor<DpImage, DpImage, MulAddConstKernel, AlgebraDouble> proMulAddConst;
    DpImage *inOut[] = {&dst};

    proMulAddConst.process(inOut, inOut, mulAddKernel);
}


void Convolver::convolveIB(FpImage &src, FpKernel &kernel, FpImage &dst, Convolver::ConvolverImplementation impl)
{
    if (trace) SYNC_PRINT(("Convolver::convolveIB(FpImage)\n"));

    convolve(src, kernel, dst, impl);
    MulAddConstKernel<DummyAlgebra> mulAddKernel(kernel.bias, 1.0 / kernel.invFactor);
    BufferProcessor<FpImage, FpImage, MulAddConstKernel, AlgebraFloat> proScalar;
    FpImage *inOut[] = {&dst};
    proScalar.process(inOut, inOut, mulAddKernel);

}

#if WITH_AVX

/**/
template <int UNROLL = 5>
struct ConvolutorImplWrappersUnrollFloat
{
    static const int SPRINT = UNROLL * Float32x8::SIZE;

    void operator() (const corecvs::BlockedRange<int> &rr) const
    {
        for (int i = rr.begin(); i < rr.end(); i++)
        {
            int j = l;
            for (; j + SPRINT - 1 < r; j += SPRINT)
            {
                Float32x8 sum[UNROLL];
                for (int r = 0; r < UNROLL; r++) {
                    sum[r] = Float32x8::Zero();
                }

                for (int ii = 0; ii < kh; ii++)
                {
                    float *kp = &kernel->element(ii, 0);
                    float *ip[UNROLL];
                    ip[0] = &src->element(ii + i - ky, j - kx);
                    for (int r = 0; r < UNROLL - 1; r++) {
                        ip[r + 1] = ip[r] +  Float32x8::SIZE;
                    }
                    for (int jj = 0; jj < kw; jj++)
                    {
                        Float32x8 mul1 = Float32x8::Broadcast(kp++);

                        for (int r = 0; r < UNROLL; r++) {
                            Float32x8 mul2x(ip[r]++);
                            sum[r] = multiplyAdd(mul1, mul2x, sum[r]);
                        }
                    }
                }

                for (int r = 0; r < UNROLL; r++) {
                    sum[r].save(&dst->element(i, j + Float32x8::SIZE * r));
                }
            }
            for (; j < r; j ++)
            {
                double sum = 0;
                for (int ii = 0; ii < kh; ii++)
                {
                    for (int jj = 0; jj < kw; jj++)
                    {
                        sum += kernel->element(ii, jj) * src->element(ii + i - ky, jj + j - kx);
                    }
                }
                dst->element(i, j) = sum;
            }

        }
    }

    ConvolutorImplWrappersUnrollFloat(FpImage *src, FpImage *dst, FpKernel *kernel) :
        src(src), dst(dst), kernel(kernel)
    {        
        h = src->h;
        w = src->w;
        kw = kernel->w;
        kh = kernel->h;
        kx = kernel->x;
        ky = kernel->y;
        t = 0; l = 0; r = w; d = h;

        t = std::max(t, ky);
        l = std::max(l, kx);
        d = std::min(d, h - kh + ky + 1);
        r = std::min(r, w - kw + kx + 1);
    }
    FpImage *src, *dst;
    FpKernel *kernel;
    int h, w, kw, kh, kx, ky, t, l, d, r;
};

#endif


#if WITH_AVX

template<int UNROLL>
void Convolver::unrolledWrapperConvolutor(FpImage &src, FpKernel &kernel, FpImage &dst)
{
    if (trace) SYNC_PRINT(("Convolver::unrolledWrapperConvolutor<%d>(FpImage): called %s parallel\n",
                           UNROLL, parallel ? "with" : "without"));

    ConvolutorImplWrappersUnrollFloat<UNROLL> impl(&src, &dst, &kernel);
    corecvs::parallelable_for(impl.t, impl.d, impl);
}

#endif



void Convolver::fillPerimeter(FpKernel &kernel, FpImage &dst, float value )
{

    for (int j = 0; j < dst.w; j++)
    {
        for (int i = 0; i < kernel.y; i++)
        {
                dst.element(i,j) = value;
        }

        for (int i = 0; i < kernel.h - kernel.y - 1; i++)
        {
                dst.element(dst.h - 1 - i ,j) = value;
        }
    }

    for (int i = 0; i < dst.h; i++)
    {
        for (int j = 0; j < kernel.x; j++)
        {
                dst.element(i,j) = value;
        }

        for (int j = 0; j < kernel.w - kernel.x - 1; j++)
        {
                dst.element(i, dst.w - j - 1) = value;
        }
    }
}



const char *Convolver::getName(const Convolver::ConvolverImplementation &value)
{
    switch (value)
    {
    case ALGORITHM_NAIVE:                return "ALGORITHM_NAIVE"; break ;
    case ALGORITHM_SSE_DMITRY:           return "ALGORITHM_SSE_DMITRY"; break ;

    case ALGORITHM_SSE_UNROLL_1:         return "ALGORITHM_SSE_UNROLL_1"; break ;
    case ALGORITHM_SSE_UNROLL_2:         return "ALGORITHM_SSE_UNROLL_2"; break ;
    case ALGORITHM_SSE_UNROLL_3:         return "ALGORITHM_SSE_UNROLL_3"; break ;
    case ALGORITHM_SSE_UNROLL_4:         return "ALGORITHM_SSE_UNROLL_4"; break ;
    case ALGORITHM_SSE_UNROLL_5:         return "ALGORITHM_SSE_UNROLL_5"; break ;
    case ALGORITHM_SSE_UNROLL_6:         return "ALGORITHM_SSE_UNROLL_6"; break ;
    case ALGORITHM_SSE_UNROLL_7:         return "ALGORITHM_SSE_UNROLL_7"; break ;
    case ALGORITHM_SSE_UNROLL_8:         return "ALGORITHM_SSE_UNROLL_8"; break ;
    case ALGORITHM_SSE_UNROLL_9:         return "ALGORITHM_SSE_UNROLL_9"; break ;
    case ALGORITHM_SSE_UNROLL_10:        return "ALGORITHM_SSE_UNROLL_10"; break ;
    case ALGORITHM_SSE_UNROLL_12:        return "ALGORITHM_SSE_UNROLL_12"; break ;
    case ALGORITHM_SSE_UNROLL_16:        return "ALGORITHM_SSE_UNROLL_16"; break ;
    case ALGORITHM_SSE_UNROLL_20:        return "ALGORITHM_SSE_UNROLL_20"; break ;
    case ALGORITHM_SSE_UNROLL_40:        return "ALGORITHM_SSE_UNROLL_40"; break ;
    case ALGORITHM_SSE_UNROLL_100:       return "ALGORITHM_SSE_UNROLL_100"; break ;

    case ALGORITHM_SSE_FASTKERNEL:       return "ALGORITHM_SSE_FASTKERNEL"; break ;
    case ALGORITHM_SSE_FASTKERNEL_EXP:   return "ALGORITHM_SSE_FASTKERNEL_EXP"; break ;
    case ALGORITHM_SSE_FASTKERNEL_EXP3:  return "ALGORITHM_SSE_FASTKERNEL_EXP3"; break ;

    case ALGORITHM_SSE_WRAPPERS:             return "ALGORITHM_SSE_WRAPPERS"; break ;
    case ALGORITHM_SSE_WRAPPERS_UNROLL_1:    return "ALGORITHM_SSE_WRAPPERS_UNROLL_1"; break ;
    case ALGORITHM_SSE_WRAPPERS_UNROLL_5:    return "ALGORITHM_SSE_WRAPPERS_UNROLL_5"; break ;
    case ALGORITHM_SSE_WRAPPERS_UNROLL_10:   return "ALGORITHM_SSE_WRAPPERS_UNROLL_10"; break ;

    case ALGORITHM_SSE_WRAPPERS_EX_UNROLL_1: return "ALGORITHM_SSE_WRAPPERS_EX_UNROLL_1"; break ;
    case ALGORITHM_SSE_WRAPPERS_EX_UNROLL_2: return "ALGORITHM_SSE_WRAPPERS_EX_UNROLL_2"; break ;

    default : return "Not in range"; break ;

    }
    return "Not in range";
}

void Convolver::convolve(FpImage &src, FpKernel &kernel, FpImage &dst, Convolver::ConvolverImplementation impl)
{
    switch (impl) {
        default:
        case ALGORITHM_NAIVE:
                naiveConvolutor(src, kernel, dst);  return;

#ifdef WITH_AVX
        case ALGORITHM_SSE_UNROLL_1:
                unrolledWrapperConvolutor<1>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_2:
                unrolledWrapperConvolutor<2>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_3:
                unrolledWrapperConvolutor<3>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_4:
                unrolledWrapperConvolutor<4>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_5:
                unrolledWrapperConvolutor<5>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_6:
                unrolledWrapperConvolutor<6>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_7:
                unrolledWrapperConvolutor<7>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_8:
                unrolledWrapperConvolutor<8>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_9:
                unrolledWrapperConvolutor<9>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_10:
                unrolledWrapperConvolutor<10>(src, kernel, dst);  return;


        case ALGORITHM_SSE_UNROLL_12:
                unrolledWrapperConvolutor<12>(src, kernel, dst);  return;
        case ALGORITHM_SSE_UNROLL_16:
                unrolledWrapperConvolutor<16>(src, kernel, dst);  return;
#endif

#ifdef WITH_AVX
        case ALGORITHM_SSE_WRAPPERS_UNROLL_1:
                unrolledWrapperConvolutor<1>(src, kernel, dst);  return;
        case ALGORITHM_SSE_WRAPPERS_UNROLL_5:
                unrolledWrapperConvolutor<5>(src, kernel, dst);  return;
        case ALGORITHM_SSE_WRAPPERS_UNROLL_10:
                unrolledWrapperConvolutor<10>(src, kernel, dst);  return;

        case ALGORITHM_SSE_FASTKERNEL_EXP3:
        case ALGORITHM_SSE_DMITRY:
                fastkernelConvolutorExp3(src, kernel, dst);  return;
#endif
    }
}


} //namespace corecvs

