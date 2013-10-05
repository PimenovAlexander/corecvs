/**
 * \file cannyFilter.cpp
 * \brief Add Comment Here
 *
 * \date Sept 13, 2012
 * \author a.melnikov
 */

#include "cannyFilter.h"
#include "fastKernel.h"
#include "scalarAlgebra.h"
#include "vectorAlgebra.h"
#include "vectorTraits.h"
#include "derivativeBuffer.h"
#include "serializerVisitor.h"
#include "deserializerVisitor.h"

namespace corecvs
{

/*#define white G12Buffer::G12BUFFER_MAX_VALUE
#define gray  G12Buffer::G12BUFFER_MAX_VALUE/2*/

int CannyFilter::instanceCounter = 0;

void CannyFilter::recursiveEdgeProver(G12Buffer *buffer, int h, int w)
{
    if ( h <= 0 || h >= buffer->h -1 || w <= 0 || w >= buffer->w - 1 ) return;

    int l = buffer->element(h,w - 1);
    int r = buffer->element(h,w + 1);
    int u = buffer->element(h - 1, w);
    int d = buffer->element(h + 1, w);

    int lu = buffer->element(h - 1, w - 1);
    int ld = buffer->element(h + 1, w - 1);
    int ru = buffer->element(h - 1, w + 1);
    int rd = buffer->element(h + 1, w + 1);
    if (l == gray)
    {
        buffer->element(h, w - 1) = white;
        recursiveEdgeProver(buffer,h, w - 1);
    }
    if (r == gray)
    {
        buffer->element(h, w + 1) = white;
        recursiveEdgeProver(buffer,h, w + 1);
    }
    if (u == gray)
    {
        buffer->element(h - 1, w) = white;
        recursiveEdgeProver(buffer,h - 1, w);
    }
    if (d == gray)
    {
        buffer->element(h + 1, w) = white;
        recursiveEdgeProver(buffer, h + 1, w);
    }
    if (lu == gray)
    {
        buffer->element(h - 1, w - 1) = white;
        recursiveEdgeProver(buffer, h - 1, w - 1);
    }
    if (ld == gray)
    {
        buffer->element(h + 1, w - 1) = white;
        recursiveEdgeProver(buffer, h + 1, w - 1);
    }
    if (ru == gray)
    {
        buffer->element(h - 1, w + 1) = white;
        recursiveEdgeProver(buffer,h - 1, w + 1);
    }
    if (rd == gray)
    {
        buffer->element(h + 1, w + 1) = white;
        recursiveEdgeProver(buffer, h + 1, w + 1);
    }

    return;
}

int CannyFilter::operator ()()
{

    G12Buffer* &input  = static_cast<G12Pin*>(inputPins [0])->getData();
    G12Buffer* &result = static_cast<G12Pin*>(outputPins[0])->getData();

    if (input == NULL)
        return 0;

    // non-maximum suppression
    DerivativeBuffer derBuf(input);
    G12Buffer *suppressedBuffer = derBuf.nonMaximalSuppression();

    // double thresholding
    for (int i = 0; i < suppressedBuffer->h; i++)
    {
        for (int j = 0; j < suppressedBuffer->w; j++ )
        {
             uint16_t pixel = suppressedBuffer->element(i,j);
             if (pixel < mCannyParameters.minimumThreshold())
             {
                 suppressedBuffer->element(i,j) = 0;
                 continue;
             }
             if (pixel > mCannyParameters.maximumThreshold())
             {
                 suppressedBuffer->element(i,j) = white;
                 continue;
             }
             suppressedBuffer->element(i,j) = gray;
        }
    }

    // edge tracking
    if (mCannyParameters.shouldEdgeDetect())
    {
        for (int i = 0; i < suppressedBuffer->h; i++)
        {
            for (int j = 0; j < suppressedBuffer->w; j++ )
            {
                if (suppressedBuffer->element(i,j) == white)
                    recursiveEdgeProver(suppressedBuffer, i, j);
            }
        }
        for (int i = 0; i < suppressedBuffer->h; i++)
            for (int j = 0; j < suppressedBuffer->w; j++ )
                if (suppressedBuffer->element(i,j) == gray) suppressedBuffer->element(i,j) = 0;
    }

    result = suppressedBuffer;
    return 0;
}

XMLNode* CannyFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("CannyParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mCannyParameters.accept(visitor);
    return mBlock;
}

void CannyFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mCannyParameters.accept(visitor);
}

CannyFilter::~CannyFilter()
{
    // TODO Auto-generated destructor stub
}

} /* namespace corecvs */

