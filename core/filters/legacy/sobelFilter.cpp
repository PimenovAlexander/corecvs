/**
 * \file sobelFilter.cpp
 * \brief Add Comment Here
 *
 * \date May 1, 2012
 * \author alexander
 */

#include "core/filters/legacy/sobelFilter.h"
#include "core/buffers/kernels/fastkernel/fastKernel.h"
#include "core/buffers/kernels/fastkernel/scalarAlgebra.h"
#include "core/buffers/kernels/fastkernel/vectorAlgebra.h"
#include "core/buffers/kernels/sobel.h"
#include "core/buffers/kernels/fastkernel/vectorTraits.h"
#include "core/buffers/derivativeBuffer.h"
#include "core/reflection/serializerVisitor.h"
#include "core/reflection/deserializerVisitor.h"

namespace corecvs
{

int SobelFilter::instanceCounter = 0;

int SobelFilter::operator()()
{
    G12Buffer *outputH = NULL;
    G12Buffer *outputV = NULL;

    G12Buffer* &input  = static_cast<G12Pin*>(inputPins[0])->getData();
    G12Buffer* &result = static_cast<G12Pin*>(outputPins[0])->getData();

    if (input == NULL)
        return 0;

    SobelHorizontalKernel<DummyAlgebra> kernelHor;
    SobelVerticalKernel<DummyAlgebra>   kernelVert;

    kernelHor.bias  = ((G12Buffer::BUFFER_MAX_VALUE + 1) / 2);
    kernelVert.bias = ((G12Buffer::BUFFER_MAX_VALUE + 1) / 2);

    if (!(mSobelParameters.horizontal() || mSobelParameters.vertical()))
    {
        result = new G12Buffer(input);
        return 0;
    }

    bool isDual = mSobelParameters.horizontal() && mSobelParameters.vertical();

    if (!isDual || (mSobelParameters.mMixingType != SobelMixingType::SUM_OF_ABSOLUTE))
    {
        if (mSobelParameters.horizontal())
        {
            outputH = new G12Buffer(input->getSize());
            BufferProcessor<
                G12Buffer,
                G12Buffer,
                SobelHorizontalKernel,
                G12BufferAlgebra
            > processorHor;
            processorHor.process(&input, &outputH, kernelHor);

            if (!isDual)
            {   result = outputH;
                return 0;
            }
        }

        if (mSobelParameters.vertical())
        {
            outputV = new G12Buffer(input->getSize());
            BufferProcessor<
                G12Buffer,
                G12Buffer,
                SobelVerticalKernel,
                G12BufferAlgebra
            > processorVer;
            processorVer.process(&input, &outputV, kernelVert);
            if (!isDual)
            {   result = outputV;
                return 0;
            }
        }

        result = new G12Buffer(input->getSize());

        for (int i = 0; i < result->h; i++) {
           for (int j = 0; j < result->w; j++) {
              Vector2dd grad(outputH->element(i,j), outputV->element(i,j));
              result->element(i,j) = grad.l2Metric();
           }
        }
    } else {
        result = new G12Buffer(input->getSize());
        EdgeMagnitude<DummyAlgebra> kernelEM;
        BufferProcessor<
            G12Buffer,
            G12Buffer,
            EdgeMagnitude,
            G12BufferAlgebra
        > processorEM;
        processorEM.process(&input, &result, kernelEM);
    }

    delete outputH;
    delete outputV;

    return 0;
} // operator()()

XMLNode* SobelFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("SobelParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mSobelParameters.accept(visitor);
    return mBlock;
}

void SobelFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mSobelParameters.accept(visitor);
}

SobelFilter::~SobelFilter()
{
    // TODO Auto-generated destructor stub
}

} /* namespace corecvs */
