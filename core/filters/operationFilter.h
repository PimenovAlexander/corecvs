#pragma once

/**
 * \file operationFilter.h
 *
 * \date Nov 11, 2012
 **/
#include "g12Buffer.h"

#include "filterBlock.h"
#include "baseAlgebra.h"
#include "fastKernel.h"
#include "vectorTraits.h"
#include "arithmetic.h"
#include "filtersCollection.h"
#include "operationParameters.h"

namespace corecvs
{
using namespace tinyxml2;

class OperationFilter : public FilterBlock
{
public:
    OperationFilter(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = max(instanceCounter, instanceId);

        inputPins.push_back(new G12Pin(this, Pin::INPUT_PIN, (id == -1), "0"));
        inputPins.push_back(new G12Pin(this, Pin::INPUT_PIN, (id == -1), "1"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    /**
     *   This function is a copy of the one in G12Buffer.cpp. But this is for a reason
     **/
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

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const
    {
        return FiltersCollection::typenames[FiltersCollection::OPERATION_FILTER];
    }

    virtual bool setParameters(const void * newParameters)
    {
        mOperation = *(OperationParameters*)newParameters;
        return true;
    }

    virtual void *getParameters()
    {
        return (void*)&mOperation;
    }

    virtual int operator() ( void )
    {
        if (operation().operation() == Operation::FIRST && !inputPins[0]->isEmpty())
        {
            outputPins[0]->setPin(inputPins[0], true);
            return 0;
        }

        if (operation().operation() == Operation::SECOND && !inputPins[1]->isEmpty())
        {
            outputPins[0]->setPin(inputPins[1], true);
            return 0;
        }

//        setOut(NULL);
        if (inputPins[0]->isEmpty()) {
            printf("Input 1 is NULL\n");
            return 1;
        }
        if (inputPins[1]->isEmpty()) {
            printf("Input 2 is NULL\n");
            return 1;
        }

//        G12Buffer *in[2] = {in1(), in2()};
        G12Buffer *in[2];
        inputPins[0]->getPin(in[0]);
        inputPins[1]->getPin(in[1]);

        G12Buffer* out = new G12Buffer(in[0]->h, in[0]->w, false);

        switch (operation().operation()) {
            case Operation::ADD:
                process<SumBuffers>(in, &out);
                break;
            case Operation::SUBTRACT:
                process<SubtractBuffers>(in, &out);
                break;
            default:
                break;
        }
//        setOut(out);
        outputPins[0]->initPin(out);
        return 0;
    }
    virtual ~OperationFilter();

    OperationParameters operation() const
    {
        return mOperation;
    }

    /* Section with setters */
    void setOperation(OperationParameters const &operation)
    {
        mOperation = operation;
    }

    virtual XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

    OperationParameters mOperation;

private:
    static int instanceCounter;
};

} /* namespace corecvs */
/* EOF */
