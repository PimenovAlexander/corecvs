/**
 * \file binarizeBlock.cpp
 * \brief Add Comment Here
 *
 * \date Jan 17, 2013
 */

#include "filters/legacy/binarizeBlock.h"
#include "reflection/serializerVisitor.h"
#include "reflection/deserializerVisitor.h"

namespace corecvs
{
int BinarizeBlock::instanceCounter = 0;

int BinarizeBlock::operator()()
{
    G12Buffer* &input  = static_cast<G12Pin*>(inputPins [0])->getData();
    G12Buffer* &result = static_cast<G12Pin*>(outputPins[0])->getData();

    if (input == NULL)  return 0;
    result = new G12Buffer(input->getSize());

    for (int h = 0; h < input->h; h++ )
    {
        for (int w = 0; w < input->w; w++)
        {
            result->element(h,w) = input->element(h,w) > mBinarizeParameters.threshold() ? G12Buffer::BUFFER_MAX_VALUE : 0;
        }
    }

    return 0;
}

XMLNode* BinarizeBlock::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("BinarizeParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mBinarizeParameters.accept(visitor);
    return mBlock;
}

void BinarizeBlock::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mBinarizeParameters.accept(visitor);
}


BinarizeBlock::~BinarizeBlock()
{
    // TODO Auto-generated destructor stub
}

} /* namespace corecvs */
