/**
 * \file thickeningBlock.cpp
 * \brief Add Comment Here
 *
 * \date Jan 17, 2013
 */

#include "core/filters/thickeningBlock.h"
#include "core/reflection/serializerVisitor.h"
#include "core/reflection/deserializerVisitor.h"

namespace corecvs
{
int ThickeningBlock::instanceCounter = 0;

int ThickeningBlock::operator()()
{
    G12Buffer* &input  = static_cast<G12Pin*>(inputPins [0])->getData();
    G12Buffer* &result = static_cast<G12Pin*>(outputPins[0])->getData();

    if (input == NULL)
        return 0;
    result = new G12Buffer(input);

    int shift = mThickeningParameters.power() / 2;

    for (int h = shift + 1; h < input->h - shift - 1; h++ )
    {
        for (int w = shift + 1; w < input->w - shift - 1; w++)
        {
            if (input->element(h,w) == G12Buffer::BUFFER_MAX_VALUE)
            {
                result->fillRectangleWith(h - shift, w - shift, mThickeningParameters.power(), mThickeningParameters.power(), G12Buffer::BUFFER_MAX_VALUE);
            }
        }
    }

    return 0;
}

XMLNode* ThickeningBlock::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("ThickeningParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mThickeningParameters.accept(visitor);
    return mBlock;
}

void ThickeningBlock::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mThickeningParameters.accept(visitor);
}


ThickeningBlock::~ThickeningBlock()
{
    // TODO Auto-generated destructor stub
}

} /* namespace corecvs */
