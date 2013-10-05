/**
 * \file gainOffsetFilter.cpp
 * \brief Add Comment Here
 *
 * \date May 1, 2012
 * \author alexander
 */

#include "gainOffsetFilter.h"
#include "g12Buffer.h"
#include "../buffers/commonMappers.h"
#include "serializerVisitor.h"
#include "deserializerVisitor.h"

namespace corecvs
{

int GainOffsetFilter::instanceCounter = 0;

GainOffsetFilter::~GainOffsetFilter()
{
    // TODO Auto-generated destructor stub
}

int GainOffsetFilter::operator()()
{
//    printf("GainOffsetFilter::operator() called\n");
    G12Buffer* &input  = static_cast<G12Pin*>(inputPins[0])->getData();
    G12Buffer* &result = static_cast<G12Pin*>(outputPins[0])->getData();

    if (input == NULL)
        return 0;

    result = new G12Buffer(input);
    GainOffsetMapper gom(mGainOffsetParameters.gain(), mGainOffsetParameters.offset());
    result->mapOperationElementwize<GainOffsetMapper>(gom);
    return 0;
}

XMLNode* GainOffsetFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("GainOffsetParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mGainOffsetParameters.accept(visitor);
    return mBlock;
}

void GainOffsetFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mGainOffsetParameters.accept(visitor);
}

} /* namespace corecvs */
