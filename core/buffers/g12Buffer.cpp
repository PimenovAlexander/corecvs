/**
 * \file g12Buffer.cpp
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 22, 2010
 * \author alexander
 */

#include "core/buffers/g12Buffer.h"
#include "core/buffers/commonMappers.h"
#include "core/buffers/kernels/fastkernel/fastKernel.h"
#include "core/buffers/kernels/fastkernel/scalarAlgebra.h"
#include "core/buffers/kernels/fastkernel/vectorAlgebra.h"
#include "core/buffers/kernels/arithmetic.h"
#include "core/buffers/kernels/threshold.h"
#include "core/buffers/kernels/fastkernel/vectorTraits.h"
//#include "rgb24/hardcodeFont.h"

namespace corecvs {

using std::ostream;
using std::istream;
using std::cout;

const int G12Buffer::BUFFER_BITS;
const int G12Buffer::BUFFER_MAX_VALUE;

G12Buffer::~G12Buffer()
{
}


void G12Buffer::print()
{
    cout << *this;
}

bool G12Buffer::verify()
{
    for (int i = 0; i < this->h; i++)
    {
        for (int j = 0; j < this->w; j++)
        {
            if (this->element(i,j) > G12Buffer::BUFFER_MAX_VALUE)
            {
                DOTRACE(("verification failed at position %d %d with value %x\n", i,j, this->element(i,j)));
                return false;
            }
        }
    }
    return true;

}


ostream & operator <<(ostream &out, const G12Buffer &buffer)
{
    out << "       ";
    for (int j = 0; j < buffer.w; j++)
    {
        out.width(5);
        out << j << " ";
    }
    out << endl;
    out << "     +-";
    for (int j = 0; j < buffer.w; j++)
    {
        out << "------";
    }
    out << "+" << endl;

    for (int i = 0; i < buffer.h; i++)
    {
        out.width(4);
        out << i;
        out <<" | ";
        for (int j = 0; j < buffer.w; j++)
        {
            out.width(5);
            out << buffer.element(i, j) << " ";
        }
        out << "| "<< endl;
    }
    out << "     +-";
    for (int j = 0; j < buffer.w; j++)
    {
      out << "------";
    }
    out << "+" << endl;
    //out.width(wasWidth);
    return out;
}

void G12Buffer::gainOffset (double gain, int offset)
{
    GainOffsetMapper mapper(gain, offset);
    mapOperationElementwize(mapper);
}

void G12Buffer::shiftMask (uint32_t mask, int8_t shift)
{
    ShiftMaskMapper mapper(mask, shift);
    mapOperationElementwize(mapper);
}

void G12Buffer::intervalMap (uint32_t min, uint32_t max)
{
    IntervalMapper mapper(min, max);
    mapOperationElementwize(mapper);
}

template<template <typename> class KernelType>
    static void process(
            G12Buffer* input [KernelType<DummyAlgebra>::inputNumber],
            G12Buffer* output[KernelType<DummyAlgebra>::inputNumber],
            const KernelType<DummyAlgebra> &kernel = KernelType<DummyAlgebra>())
{
    BufferProcessor<
        G12Buffer,
        G12Buffer,
        KernelType,
        G12BufferAlgebra
    > processor;
    processor.process(input, output, kernel);
}


G12Buffer *G12Buffer::difference (G12Buffer *first, G12Buffer *second)
{
    G12Buffer *in[2] = {first, second};
    G12Buffer *toReturn = new G12Buffer(first->h, second->w, false);
    process<DifferenceBuffers>(in, &toReturn);
    return toReturn;
}

G12Buffer *G12Buffer::binarize (uint16_t threshold)
{
    G12Buffer *toReturn = new G12Buffer(this->h, this->w, false);
    ThresholdBinariseKernel<DummyAlgebra> kernel(threshold);
    G12Buffer *in[1] = { this };
    process<ThresholdBinariseKernel>(in, &toReturn, kernel);
    return toReturn;
}

template <typename Algebra = DummyAlgebra>
class ConverterYUYV2G12
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; }
    inline static int getCenterY(){ return 0; }
    inline static int getSizeX()  { return 1; }
    inline static int getSizeY()  { return 1; }

    typedef typename Algebra::InputType Type;

    template<typename OtherAlgebra>
        ConverterYUYV2G12(const ConverterYUYV2G12<OtherAlgebra> &) {}
        ConverterYUYV2G12(){}

    void process(Algebra &algebra) const
    {
        Type a00 = algebra.getInput(0,0);
        Type result = (a00 & Type((uint16_t)0x00FF)) << 4;
        algebra.putOutput(0,0,result);
    }
};

void G12Buffer::fillWithYUYV (uint16_t *yuyv)
{
    ConverterYUYV2G12<> kernel;
    /*TODO: Rare case when we use default constructor. We should get rid of this */
    AbstractBuffer<uint16_t> inBuffer;

    inBuffer.data = yuyv;
    inBuffer.h = this->h;
    inBuffer.w = this->w;
    inBuffer.stride = this->stride;

    AbstractBuffer<uint16_t> *in[1] = { &inBuffer };
    G12Buffer *out[1] = { this };

    BufferProcessor<
        AbstractBuffer<uint16_t>,
        G12Buffer,
        ConverterYUYV2G12,
        G12BufferAlgebra
    > processor;
    processor.process(in, out, kernel);
}


} //namespace corecvs
