#pragma once
/**
 * \file fastKernel.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 26, 2010
 * \author alexander
 */
#include "utils/global.h"

#include "buffers/kernels/fastkernel/baseKernel.h"
#include "buffers/kernels/fastkernel/scalarAlgebra.h"
#include "tbbwrapper/tbbWrapper.h"

namespace corecvs {

/**
 *  This is a main template that provides fast processing of buffers
 *  It uses two approaches - an algebra and a fallback algebra
 *
 *
 *
 **/
template<
         typename InputBuffer,
         typename OutputBuffer,
         template <typename> class GenericKernelType,
         template <int, int> class GAlgebraType
         >
class BufferProcessor
{
    typedef GenericKernelType<DummyAlgebra> DKernelType;
    const static int inputNumber  = DKernelType::inputNumber;
    const static int outputNumber = DKernelType::outputNumber;

    typedef typename GAlgebraType<inputNumber, outputNumber>::Type MAlgebraType;

    typedef GenericKernelType<MAlgebraType> MKernelType;
    typedef typename MAlgebraType::FallbackAlgebra  FAlgebraType;
    typedef GenericKernelType<FAlgebraType> FKernelType;

    typedef typename InputBuffer::InternalIndexType IndexType;


public:

    template<int shouldAlign>
    class ParallelProcess
    {
    protected:
        InputBuffer** input;
        OutputBuffer** output;

        MKernelType* mKernel;
        FKernelType* fKernel;
    public:
        ParallelProcess(
            InputBuffer*  _input[inputNumber],
            OutputBuffer* _output[outputNumber],
            MKernelType* _mKernel,
            FKernelType* _fKernel ):
                input (_input),
                output(_output),
                mKernel(_mKernel),
                fKernel(_fKernel)
        {}

        ALIGN_STACK_SSE void operator()( const BlockedRange<IndexType>& r ) const
        {
            const static int mStep = MAlgebraType::step();
            const static int fStep = FAlgebraType::step();

            MAlgebraType mAlgebra;
            FAlgebraType fAlgebra;

            for (int in = 0; in < inputNumber; in++)
            {
                mAlgebra.setInputStride (input[in]->stride, in);
                fAlgebra.setInputStride (input[in]->stride, in);
            }
            for (int out = 0; out < outputNumber; out++)
            {
                mAlgebra.setOutputStride(output[out]->stride, out);
                fAlgebra.setOutputStride(output[out]->stride, out);
            }

            int shiftX = fKernel->getCenterX();
            int shiftY = fKernel->getCenterY();
            int sizeX  = fKernel->getSizeX();
            IndexType wLimit = input[0]->w - sizeX + 1;  // wLimit is a last not included index

            for (IndexType i = r.begin(); i != r.end(); ++i)
            {
                IndexType j;
                /**
                 * In case when we should align, first execute iterations until the aligned address is reached.
                 */
                if (shouldAlign)
                {
                    for (int  in = 0; in  <  inputNumber; in++)
                        fAlgebra.setInputPos ((void *)&input [ in]->element(         i,      0),  in);
                    for (int out = 0; out < outputNumber; out++)
                        fAlgebra.setOutputPos((void *)&output[out]->element(i + shiftY, shiftX), out);

                    for (j = 0; j + fStep <= wLimit; j += fStep)   // the next step index is validated
                    {
                        if ((((uint64_t)(void *)fAlgebra.getOutputPos(0)) % 16) == 0)
                            break;
                        fKernel->process(fAlgebra);
                        fAlgebra.advance();
                    }
                    mAlgebra.copyPos(fAlgebra);
                }
                else {
                    j = 0;
                    for (int  in = 0; in  < inputNumber;  in++)
                        mAlgebra.setInputPos ((void *)&(input[in]  ->element(        i,       0)), in);
                    for (int out = 0; out < outputNumber; out++)
                        mAlgebra.setOutputPos((void *)&(output[out]->element(i + shiftY, shiftX)), out);

                }

                for (; j + mStep <= wLimit; j += mStep)   // the next step index is validated
                {
                    mKernel->process(mAlgebra);
                    mAlgebra.advance();
                    }

                fAlgebra.copyPos(mAlgebra);
                for (; j + fStep <= wLimit; j += fStep)   // the next step index is validated
                {
                    fKernel->process(fAlgebra);
                    fAlgebra.advance();
                }
            }
        }
    };


ALIGN_STACK_SSE void process(
            InputBuffer* input[inputNumber],
            OutputBuffer* output[outputNumber],
            const DKernelType &kernel = DKernelType())
    {
        MKernelType mKernel(kernel);
        FKernelType fKernel(kernel);

        IndexType hLimit = input[0]->h - fKernel.getSizeY() + 1;  // hLimit is a last not included index

        ParallelProcess<false> exe(input, output, &mKernel, &fKernel);

        parallelable_for<IndexType, ParallelProcess<false> >((IndexType)0, hLimit, exe);
    }


ALIGN_STACK_SSE  void processSaveAligned(
            InputBuffer* input[inputNumber],
            OutputBuffer* output[outputNumber],
            const DKernelType &kernel = DKernelType())
    {
        MKernelType mKernel(kernel);
        FKernelType fKernel(kernel);

        IndexType hLimit = input[0]->h - fKernel.getSizeY() + 1;  // hLimit is a last not included index

        ParallelProcess<true> exe(input, output, &mKernel, &fKernel);

        parallelable_for<IndexType, ParallelProcess<true> >((IndexType)0, hLimit, exe);
    }

};

#if 0
template<
         typename InputBuffer,
         typename OutputBuffer,
         template <typename> class GenericKernelType,
         template <int, int> class GAlgebraType
         >
class BufferProcessor
{
    typedef GenericKernelType<DummyAlgebra> DKernelType;
    const static int inputNumber  = DKernelType::inputNumber;
    const static int outputNumber = DKernelType::outputNumber;

    typedef typename GAlgebraType<inputNumber, outputNumber>::Type MAlgebraType;

    typedef GenericKernelType<MAlgebraType> MKernelType;
    typedef typename MAlgebraType::FallbackAlgebra  FAlgebraType;
    typedef GenericKernelType<FAlgebraType> FKernelType;

    typedef typename InputBuffer::InternalIndexType IndexType;


public:

    template<int shouldAlign, int unroll = 1>
    class ParallelProcess
    {
    protected:
        InputBuffer** input;
        OutputBuffer** output;

        MKernelType* mKernel;
        FKernelType* fKernel;
    public:
        ParallelProcess(
            InputBuffer*  _input[inputNumber],
            OutputBuffer* _output[outputNumber],
            MKernelType* _mKernel,
            FKernelType* _fKernel ):
                input (_input),
                output(_output),
                mKernel(_mKernel),
                fKernel(_fKernel)
        {}

        ALIGN_STACK_SSE void operator()( const BlockedRange<IndexType>& r ) const
        {
            const static int mStep = MAlgebraType::step();
            const static int fStep = FAlgebraType::step();

            MAlgebraType mAlgebra[unroll];
            FAlgebraType fAlgebra;

            for (int in = 0; in < inputNumber; in++)
            {
                for (int r = 0; r < unroll; r++)
                    mAlgebra[r].setInputStride (input[in]->stride, in);
                fAlgebra.setInputStride (input[in]->stride, in);
            }
            for (int out = 0; out < outputNumber; out++)
            {
                for (int r = 0; r < unroll; r++)
                    mAlgebra[r].setOutputStride(output[out]->stride, out);
                fAlgebra.setOutputStride(output[out]->stride, out);
            }

            int shiftX = fKernel->getCenterX();
            int shiftY = fKernel->getCenterY();
            int sizeX  = fKernel->getSizeX();
            IndexType wLimit = input[0]->w - sizeX + 1;  // wLimit is a last not included index

            for (IndexType i = r.begin(); i != r.end(); ++i)
            {
                IndexType j;
                /**
                 * In case when we should align, first execute iterations until the aligned address is reached.
                 */
                if (shouldAlign)
                {
                    for (int  in = 0; in  <  inputNumber; in++)
                        fAlgebra.setInputPos ((void *)&input [ in]->element(         i,      0),  in);
                    for (int out = 0; out < outputNumber; out++)
                        fAlgebra.setOutputPos((void *)&output[out]->element(i + shiftY, shiftX), out);

                    for (j = 0; j + fStep <= wLimit; j += fStep)   // the next step index is validated
                    {
                        if ((((uint64_t)(void *)fAlgebra.getOutputPos(0)) % 16) == 0)
                            break;
                        fKernel->process(fAlgebra);
                        fAlgebra.advance();
                    }

                    mAlgebra[0].copyPos(fAlgebra);
                    for (int r = 0; r < unroll - 1; r++) {
                        mAlgebra[r + 1].copyPos(mAlgebra[r]);
                        mAlgebra[r + 1].advance();
                    }

                }
                else {
                    j = 0;
                    for (int  in = 0; in  < inputNumber;  in++)
                       for (int r = 0; r < unroll; r++)
                           mAlgebra[r].setInputPos ((void *)&(input[in]  ->element(        i,       mStep * r)), in);

                    for (int out = 0; out < outputNumber; out++)
                        for (int r = 0; r < unroll; r++)
                           mAlgebra[r].setOutputPos((void *)&(output[out]->element(i + shiftY, shiftX + mStep * r)), out);

                }

                for (; j + mStep * unroll <= wLimit ; j += mStep * unroll)   // the next step index is validated
                {
                    for (int r = 0; r < unroll; r++) {
                        mKernel->process(mAlgebra[r]);
                    }

                    mAlgebra[0].advance();
                    for (int r = 0; r < unroll - 1; r++) {
                        mAlgebra[r + 1].copyPos(mAlgebra[r]);
                        mAlgebra[r + 1].advance();
                    }
                }

                fAlgebra.copyPos(mAlgebra[0]);
                for (; j + fStep <= wLimit; j += fStep)   // the next step index is validated
                {
                    fKernel->process(fAlgebra);
                    fAlgebra.advance();
                }
            }
        }
    };


ALIGN_STACK_SSE void process(
            InputBuffer* input[inputNumber],
            OutputBuffer* output[outputNumber],
            const DKernelType &kernel = DKernelType(), int unroll = 1)
    {
        MKernelType mKernel(kernel);
        FKernelType fKernel(kernel);

        IndexType hLimit = input[0]->h - fKernel.getSizeY() + 1;  // hLimit is a last not included index

        if (unroll == 1) {
            ParallelProcess<false, 1> exe(input, output, &mKernel, &fKernel);
            parallelable_for<IndexType, ParallelProcess<false, 1> >((IndexType)0, hLimit, exe);
        }

        if (unroll == 2) {
            ParallelProcess<false, 2> exe(input, output, &mKernel, &fKernel);
            parallelable_for<IndexType, ParallelProcess<false, 2> >((IndexType)0, hLimit, exe);
        }


    }
#endif


} //namespace corecvs
