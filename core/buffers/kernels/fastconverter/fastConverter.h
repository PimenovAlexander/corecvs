/**
 * \file fastConverter.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 26, 2010
 * \author alexander
 */

#ifndef FASTKERNEL_H_
#define FASTKERNEL_H_

#include "utils/global.h"

#include "buffers/kernels/fastkernel/baseKernel.h"
#include "buffers/kernels/fastkernel/scalarAlgebra.h"
#include "tbbwrapper/tbbWrapper.h"
namespace corecvs {


class DummyReader
{

};

class DummyWriter
{

};



/**
 *  This is a main template that provides fast processing of buffers
 *  It uses two approaches - an algebra and a fallback algebra
 *
 *
 *
 **/
template<
         typename ReaderType,
         typename WriterType,
         template <typename> class GenericKernelType,
         template <int, int> class GAlgebraType
         >
class BufferConverter
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

    class ParallelConverter
    {
    protected:
        InputBuffer* input;
        OutputBuffer* output;

        MKernelType* mKernel;
        FKernelType* fKernel;
    public:
        ParallelConverter(
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

            int shiftX = DKernelType::getCenterX();
            int shiftY = DKernelType::getCenterY();
            int sizeX  = DKernelType::getSizeX();
            IndexType wLimit = input[0]->w - sizeX + 1;

            for ( IndexType i = r.begin(); i != r.end(); ++i )
            {
                IndexType j;
                /*TODO: Correct void ptr here */
                for (int in = 0; in < inputNumber; in++)
                    mAlgebra.setInputPos ((void *)&input[in]->element(i,0), in);
                for (int out = 0; out < outputNumber; out++)
                    mAlgebra.setOutputPos((void *)&output[out]->element(i + shiftY, shiftX), out);

                for (j = 0; j +  mStep <= wLimit ; j += mStep)
                {
                    mKernel->process(mAlgebra);
                    mAlgebra.advance();
                }

                fAlgebra.copyPos(mAlgebra);
                for (     ; j + fStep <= wLimit ; j += fStep)
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

        int sizeY  = DKernelType::getSizeY();
        IndexType hLimit = input[0]->h - sizeY + 1;

        MKernelType mKernel(kernel);
        FKernelType fKernel(kernel);


        ParallelProcess exe(input, output, &mKernel, &fKernel);

        parallelable_for<IndexType, ParallelProcess>((IndexType)0, hLimit, exe);
    }



class ParallelProcessAlign
{
protected:
    InputBuffer** input;
    OutputBuffer** output;

    MKernelType* mKernel;
    FKernelType* fKernel;
public:
    ParallelProcessAlign(
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

        int shiftX = DKernelType::getCenterX();
        int shiftY = DKernelType::getCenterY();
        int sizeX  = DKernelType::getSizeX();
        IndexType wLimit = input[0]->w - sizeX + 1;

        for ( IndexType i = r.begin(); i != r.end(); ++i )
        {
            IndexType j;
            /*TODO: Correct void ptr here */
            for (int  in = 0; in  <  inputNumber; in++)
                fAlgebra.setInputPos ((void *)&input [ in]->element(         i,      0),  in);
            for (int out = 0; out < outputNumber; out++)
                fAlgebra.setOutputPos((void *)&output[out]->element(i + shiftY, shiftX), out);

            for (j = 0; j + fStep <= wLimit ; j += fStep)
            {
                if ((((uint64_t)(void *)fAlgebra.getOutputPos(0)) % 16) == 0 )
                    break;
                fKernel->process(fAlgebra);
                fAlgebra.advance();
            }

            mAlgebra.copyPos(fAlgebra);
            for (      ; j +  mStep <= wLimit ; j += mStep)
            {
                mKernel->process(mAlgebra);
                mAlgebra.advance();
            }

            fAlgebra.copyPos(mAlgebra);
            for (     ; j + fStep <= wLimit ; j += fStep)
            {
                fKernel->process(fAlgebra);
                fAlgebra.advance();
            }
        }
    }
};



ALIGN_STACK_SSE  void processSaveAligned(
            InputBuffer* input[inputNumber],
            OutputBuffer* output[outputNumber],
            const DKernelType &kernel = DKernelType())
    {
        int sizeY  = DKernelType::getSizeY();
        IndexType hLimit = input[0]->h - sizeY + 1;

        MKernelType mKernel(kernel);
        FKernelType fKernel(kernel);


        ParallelProcessAlign exe(input, output, &mKernel, &fKernel);

        parallelable_for<IndexType, ParallelProcessAlign>((IndexType)0, hLimit, exe);


    }

};


} //namespace corecvs
#endif /* FASTKERNEL_H_ */

