/**
 * \file operationFilter.cpp
 *
 * \date Nov 11, 2012
 **/

#include "filters/legacy/operationFilter.h"
#include "reflection/serializerVisitor.h"
#include "reflection/deserializerVisitor.h"

namespace corecvs
{

int OperationFilter::instanceCounter = 0;

XMLNode* OperationFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("OperationParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mOperation.accept(visitor);
    return mBlock;
}

void OperationFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mOperation.accept(visitor);
}

OperationFilter::~OperationFilter()
{
    // TODO Auto-generated destructor stub
}

} /* namespace corecvs */
