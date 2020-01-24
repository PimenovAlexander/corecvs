/**
 * \file filterBlock.cpp
 *
 * \date Nov 9, 2012
 **/

#include "core/filters/legacy/blocks/filterBlock.h"
//#include "core/filters/legacy/blocks/filterGraph.h"
#include "core/reflection/serializerVisitor.h"

namespace corecvs
{
using namespace tinyxml2;

FilterBlock::FilterBlock(/*FilterGraph* _parent,*/ int id) :
//    parent(_parent),
    parent(NULL),
    instanceId(id),
    inLinks(0),
    sort(PROCESSING_FILTER),
    instanceName(NULL)
{
//    if (parent)
//    {  if (instanceId == -1)
//            instanceId = ++parent->blockCounter;
//       else parent->blockCounter = max(parent->blockCounter, instanceId);
//    }
}

FilterBlock::~FilterBlock()
{
    free(instanceName);
    instanceName = NULL;
    if (sort == COMPOUND_FILTER)  return;
    vector<Pin*>::iterator itPins;
    for (itPins = inputPins.begin(); itPins != inputPins.end(); ++itPins)
        /*if (!isOuterPin(*itPins))*/  delete_safe(*itPins);

    for (itPins = outputPins.begin(); itPins != outputPins.end(); ++itPins)
        /*if (!isOuterPin(*itPins))*/  delete_safe(*itPins);
}

XMLNode* FilterBlock::serialize(XMLNode* node)
{
    XMLElement* block = node->GetDocument()->NewElement("block");
    block->SetAttribute("type", getTypeName());
    block->SetAttribute("id", instanceId);
    XMLNode* blockNode = node->InsertEndChild(block);

    XMLNode* presentationNode = blockNode->InsertEndChild(node->GetDocument()->NewElement("presentation"));
    XMLElement* presentation[] = { node->GetDocument()->NewElement("name"),
                                   node->GetDocument()->NewElement("x"),
                                   node->GetDocument()->NewElement("y") };
    presentation[0]->SetAttribute("value", instanceName);
    presentation[1]->SetAttribute("value", x);
    presentation[2]->SetAttribute("value", y);
    for (unsigned int j = 0; j < 3; j++)
        presentationNode->InsertEndChild(presentation[j]);

    XMLElement* in = node->GetDocument()->NewElement("inputPins");
    in->SetAttribute("size", (int)inputPins.size());
    XMLNode* ins = blockNode->InsertEndChild(in);

    for (unsigned int j = 0; j < inputPins.size(); j++)
        inputPins[j]->serialize(ins, isOuterPin(inputPins[j]));

    XMLElement* out = node->GetDocument()->NewElement("outputPins");
    out->SetAttribute("size", (int)outputPins.size());
    XMLNode* outs = blockNode->InsertEndChild(out);

    for (unsigned int j = 0; j < outputPins.size(); j++)
        outputPins[j]->serialize(outs, isOuterPin(outputPins[j]));

    return blockNode;
} // serialize

void FilterBlock::deserialize(XMLNode* node, bool force)
{
    XMLNode* p1 = node->FirstChild();

    XMLNode* p2 = p1->FirstChild();
    setInstanceName( strdup(p2->ToElement()->Attribute("value")) );
    p2 = p2->NextSibling();
    x = p2->ToElement()->IntAttribute("value");
    p2 = p2->NextSibling();
    y = p2->ToElement()->IntAttribute("value");

    p1 = p1->NextSibling();
    deserializePins(inputPins, p1, force);

    p1 = p1->NextSibling();
    deserializePins(outputPins, p1, force);
} // deserialize

void FilterBlock::deserializePins(vector<Pin*> &vec, XMLNode* node, bool force)
{
    unsigned int s = node->ToElement()->IntAttribute("size");
    XMLNode* p2 = node->FirstChild();
    for (unsigned int j = 0; j < s; j++)
    {   Pin* pin = NULL;
        if (force)
        {    if (j < vec.size())  pin = vec[j]; }
        else pin = PinById(vec, p2->ToElement()->IntAttribute("id"));
        if (pin)  pin->deserialize(p2);
        p2 = p2->NextSibling();
    }
}

void FilterBlock::clear()
{
    for (unsigned int i = 0; i < outputPins.size(); i++)
        outputPins[i]->clear();
}

} /* namespace corecvs */
