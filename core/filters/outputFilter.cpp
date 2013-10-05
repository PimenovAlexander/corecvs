#include "outputFilter.h"
#include "serializerVisitor.h"
#include "deserializerVisitor.h"

namespace corecvs
{

int OutputFilter::instanceCounter = 0;

int OutputFilter::operator()(void)
{
    outputPins[0]->setPin(inputPins[0], true);
    return 0;
}

XMLNode* OutputFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("OutputParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mOutputParameters.accept(visitor);
    return mBlock;
}

void OutputFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mOutputParameters.accept(visitor);
}

OutputFilter::~OutputFilter()
{
    clear();
}

} // namespace corecvs
