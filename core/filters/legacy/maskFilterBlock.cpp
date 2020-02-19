/**
 * \file maskFilterBlock.cpp
 * \brief Add Comment Here
 *
 * \date Jan 17, 2013
 */

#include "filters/legacy/maskFilterBlock.h"
#include "reflection/serializerVisitor.h"
#include "reflection/deserializerVisitor.h"

namespace corecvs
{
int MaskFilterBlock::instanceCounter = 0;

int MaskFilterBlock::operator()()
{
    G12Buffer* &input  = static_cast<G12Pin*>(inputPins [0])->getData();
    G12Buffer* &mask   = static_cast<G12Pin*>(inputPins [1])->getData();
    G12Buffer* &result = static_cast<G12Pin*>(outputPins[0])->getData();

    if (input == NULL || mask == NULL)  return 0;
    result = new G12Buffer(input->getSize());

    for (int h = 0; h < input->h; h++ )
    {
        for (int w = 0; w < input->w; w++)
        {
            result->element(h,w) = mask->element(h,w) != 0 ? input->element(h,w) : 0;
        }
    }

    return 0;
}

XMLNode* MaskFilterBlock::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("MaskingParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mMaskingParameters.accept(visitor);
    return mBlock;
}

void MaskFilterBlock::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mMaskingParameters.accept(visitor);
}


MaskFilterBlock::~MaskFilterBlock()
{
    // TODO Auto-generated destructor stub
}

} /* namespace corecvs */
