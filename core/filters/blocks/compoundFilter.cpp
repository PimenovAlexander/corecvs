#include "compoundFilter.h"

namespace corecvs
{

int CompoundFilter::instanceCounter = 0;

CompoundFilter::CompoundFilter(//FilterGraph* _parent,
                               FiltersCollection* collection,
                               int _typeid,
                               int id) :
    FilterBlock(/*_parent,*/ id), typeId(_typeid)
{
    sort = COMPOUND_FILTER;
    if (instanceId == -1)
         instanceId = ++instanceCounter;
    else instanceCounter = max(instanceCounter, instanceId);

    graph = new FilterGraph(collection, parent);
    graph->loadFromFile(collection->compoundFilters[typeId].filename.c_str());
    stealPinsFromGraph();
} // constructor

void CompoundFilter::stealPinsFromGraph()
{
    inputPins.clear();
    outputPins.clear();

    for (unsigned int i = 0; i < graph->inputs.size(); i++)
       addInputPin(graph->inputs[i]);
    for (unsigned int i = 0; i < graph->outputs.size(); i++)
       addOutputPin(graph->outputs[i]);
}

void CompoundFilter::loadGraph()
{
    XMLDocument* doc = new XMLDocument();
    FilterBlock::serialize(doc);

    FilterBlock savedPins;
    savedPins.setInstanceName(strdup("tmp"));
    for (unsigned int i = 0; i < parent->blocks.size(); i++)
       for (unsigned int j = 0; j < parent->blocks[i]->inputPins.size(); j++)
          if (std::find(outputPins.begin(), outputPins.end(), parent->blocks[i]->inputPins[j]->takeFrom) != outputPins.end())
             savedPins.inputPins.push_back(parent->blocks[i]->inputPins[j]);
    XMLDocument* doc1 = new XMLDocument();
    savedPins.serialize(doc1);

    graph->clear();
    graph->loadFromFile(graph->collection->compoundFilters[typeId].filename.c_str());
    stealPinsFromGraph();

    FilterBlock::deserialize(doc->FirstChild(), false);
    savedPins.deserialize(doc1->FirstChild(), false);
    savedPins.inputPins.clear();
    delete doc1;

    delete doc;
}

XMLNode* CompoundFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    return mBlock;
}

void CompoundFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
}

void CompoundFilter::clear()
{
//    FilterBlock::clear();
    graph->clearAllData();
}

CompoundFilter::~CompoundFilter()
{
    delete_safe(graph);
    inputPins.clear();
    outputPins.clear();
}

} /* namespace corecvs */
