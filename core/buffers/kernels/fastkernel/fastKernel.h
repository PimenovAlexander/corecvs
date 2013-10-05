#pragma once
/**
 * \file fastKernel.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 26, 2010
 * \author alexander
 */
#include "global.h"

#include "baseKernel.h"
#include "scalarAlgebra.h"
#include "tbbWrapper.h"

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


} //namespace corecvs
