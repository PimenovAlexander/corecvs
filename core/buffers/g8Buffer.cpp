/**
 * \file g8Buffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 6, 2010
 * \author alexander
 */

#include "g8Buffer.h"
#include "g12Buffer.h"
#include "sseWrapper.h"

#include "vectorTraits.h"
#include "fastKernel.h"
#include "arithmetic.h"

namespace corecvs {

G8Buffer* G8Buffer::FromG12Buffer(G12Buffer *input)
{
    G8Buffer* result = new G8Buffer(input->h, input->w, false);
    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            result->element(i,j) = (input->element(i,j) >> 4);
        }
    }
    return result;
}


G12Buffer* G8Buffer::toG12Buffer(corecvs::G8Buffer *input)
{
    G12Buffer* result = new G12Buffer(input->getSize(), false);
    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            result->element(i,j) = (input->element(i,j) << 4);
        }
    }
    return result;
}

int G8Buffer::countValues(uint8_t value, int x1, int y1, int x2, int y2)
{
    if (x1 == -1) x1 = 0;
    if (y1 == -1) y1 = 0;

    if (x2 == -1) x2 = this->w - 1;
    if (y2 == -1) y2 = this->h - 1;


    int count = 0;
    for (int i = y1; i <= y2; i++)
        for (int j = x1; j <= x2; j++)
            if (element(i,j) == value)
                count++;
    return count;
}

void G8Buffer::addBufferSat(G8Buffer *that)
{
    int effH = CORE_MIN(h, that->h);
    int effW = CORE_MIN(w, that->w);

    for (int i = 0; i < effH; i++)
    {
        int j = 0;
#ifdef WITH_SSE
        for (; j + 8 <= effW; j += 8)
        {
            UInt8x16 eThis(&this->element(i, j));
            UInt8x16 eThat(&that->element(i, j));

            adds(eThis, eThat).save(&this->element(i, j));
        }
#endif
        for (; j < effW; j++)
        {
            uint16_t sum = this->element(i, j) + that->element(i, j);
            this->element(i, j) = CORE_MIN(sum, G8Buffer::BUFFER_MAX_VALUE);
        }
    }
}

void G8Buffer::addValueSat(int16_t val)
{
    int effH = h;
    int effW = w;

    if (val > 0)
    {
        for (int i = 0; i < effH; i++)
        {
            int j = 0;
#ifdef WITH_SSE
            for (; j + 8 <= effW; j += 8)
            {
                UInt8x16 eThis(&this->element(i, j));
                UInt8x16 eThat(val);

                adds(eThis, eThat).save(&this->element(i, j));
            }
#endif
            for (; j < effW; j++)
            {
                uint16_t sum = this->element(i, j) + val;
                this->element(i, j) = CORE_MIN(sum, G8Buffer::BUFFER_MAX_VALUE);
            }
        }
    } else {
        val = -val;
        for (int i = 0; i < effH; i++)
        {
            int j = 0;
#ifdef WITH_SSE
            for (; j + 8 <= effW; j += 8)
            {
                UInt8x16 eThis(&this->element(i, j));
                UInt8x16 eThat(val);

                subs(eThis, eThat).save(&this->element(i, j));
            }
#endif
            for (; j < effW; j++)
            {
                int16_t sum = this->element(i, j) + val;
                this->element(i, j) = CORE_MAX(sum, 0);
            }
        }
    }
}

void G8Buffer::averageWith(G8Buffer * /*that*/)
{

}

template<template <typename> class KernelType>
    static void process(
            G8Buffer* input [KernelType<DummyAlgebra>::inputNumber],
            G8Buffer* output[KernelType<DummyAlgebra>::inputNumber],
            const KernelType<DummyAlgebra> &kernel = KernelType<DummyAlgebra>())
{
    BufferProcessor<
        G8Buffer,
        G8Buffer,
        KernelType,
        G8BufferAlgebra
    > processor;
    processor.process(input, output, kernel);
}


G8Buffer *G8Buffer::difference (G8Buffer *first, G8Buffer *second)
{
    G8Buffer *in[2] = {first, second};
    G8Buffer *toReturn = new G8Buffer(first->h, second->w, false);
    process<DifferenceBuffers>(in, &toReturn);
    return toReturn;
}


} //namespace corecvs

