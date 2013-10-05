#include "inputFilter.h"
#include "serializerVisitor.h"
#include "deserializerVisitor.h"

namespace corecvs
{

int InputFilter::instanceCounter = 0;

int InputFilter::operator()(void)
{
//    outputPins[0]->initPin(buffer);
    outputPins[0]->setPin(inputPins[0]);
    return 0;
}

XMLNode* InputFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("InputParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mInputParameters.accept(visitor);
    return mBlock;
}

void InputFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mInputParameters.accept(visitor);
}

InputFilter::~InputFilter()
{

}

} // namespace corecvs
