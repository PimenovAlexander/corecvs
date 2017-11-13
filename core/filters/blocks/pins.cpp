#include "core/filters/blocks/pins.h"
#include "core/filters/blocks/filterBlock.h"
#include "core/filters/blocks/filterGraph.h"

namespace corecvs {

int G12Pin::instanceCounter = 0;
int TxtPin::instanceCounter = 0;

Pin::~Pin()
{
//   if (!parent->isOuterPin(this))  free(instanceName);
   free(instanceName);
   clear();
}

XMLNode* Pin::serialize(XMLNode* node, bool isOuterPin)
{
    XMLElement* mPin = node->GetDocument()->NewElement("pin");
    mPin->SetAttribute("type", getTypeName());
    mPin->SetAttribute("id", instanceId);
    mPin->SetAttribute("name", instanceName);
    if (!isOuterPin && takeFrom)
    {   mPin->SetAttribute("takeFromBlock", takeFrom->parent->getFullName().c_str());
        mPin->SetAttribute("takeFromPin",   takeFrom->instanceId);
    }
    XMLNode* element = node->InsertEndChild(mPin);

    return element;
}

void Pin::deserialize(XMLNode* node)
{
    XMLElement* mPin = node->ToElement();
    setInstanceId(mPin->IntAttribute("id"));
    if (parent->sort != FilterBlock::COMPOUND_FILTER)
    {   free(instanceName);
        instanceName = strdup(mPin->Attribute("name"));
    }

    if ( mPin->Attribute("takeFromBlock") &&
         parent &&
         parent->parent )
    {
        FilterBlock* block = parent->parent->blockByName(mPin->Attribute("takeFromBlock"));
        if (block)
            takeFrom = block->outPinById(mPin->IntAttribute("takeFromPin"));
    }
}

void G12Pin::deserialize(XMLNode* node)
{
    Pin::deserialize(node);
}

void TxtPin::deserialize(XMLNode* node)
{
    Pin::deserialize(node);
}

} //namespace corecvs
