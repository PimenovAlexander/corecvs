#include "core/filters/legacy/filterGraph.h"
#include "core/filters/legacy/blocks/compoundFilter.h"

namespace corecvs
{
using namespace tinyxml2;

FilterGraph::~FilterGraph()
{
    vector<FilterBlock*>::iterator itBlocks;
    for (itBlocks = blocks.begin(); itBlocks != blocks.end(); ++itBlocks)
        delete_safe(*itBlocks);
    blocks.clear();
    inputs.clear();
    outputs.clear();
} // ~FilterGraph

void FilterGraph::clear()
{
    vector<FilterBlock*>::iterator itBlocks;
    for (itBlocks = blocks.begin(); itBlocks != blocks.end(); ++itBlocks)
        delete_safe(*itBlocks);
    blocks.clear();
    inputs.clear();
    outputs.clear();
} // clear()

//void FilterGraph::addIOPins()
//{

//} // clear()


FilterBlock* FilterGraph::addBlock(FilterBlock *block)
{
    blocks.push_back(block);
    block->parent = this;

    switch (block->sort)
    {
case FilterBlock::INPUT_FILTER:
        inputs.push_back(block->inputPins[0]);
        break;
case FilterBlock::OUTPUT_FILTER:
        outputs.push_back(block->outputPins[0]);
        break;
case FilterBlock::COMPOUND_FILTER:
        dynamic_cast<CompoundFilter*>(block)->graph->parent = this;
        break;
default:
        break;
    } // switch

    return block;
} // addBlock

void FilterGraph::removeBlock(FilterBlock* block)
{
    vector<FilterBlock*>::iterator blocksIterator = std::find(blocks.begin(), blocks.end(), block);
    if (blocksIterator != blocks.end())  blocks.erase(blocksIterator);

    for (unsigned int i = 0; i < block->inputPins.size(); i++)
    {   vector<Pin*>::iterator pinsIterator = std::find(inputs.begin(), inputs.end(), block->inputPins[i]);
        if (pinsIterator != inputs.end())  inputs.erase(pinsIterator);
    }

    for (unsigned int i = 0; i < block->outputPins.size(); i++)
    {   vector<Pin*>::iterator pinsIterator = std::find(outputs.begin(), outputs.end(), block->outputPins[i]);
        if (pinsIterator != outputs.end())  outputs.erase(pinsIterator);

        removeConnections(block->outputPins[i]);
        if (block->outputPins[i]->parent != block)
            dynamic_cast<CompoundFilter*>(block->outputPins[i]->parent)->parent->removeConnections(block->outputPins[i]);
    }
} // removeBlock

bool FilterGraph::connect(Pin* startPin, Pin* endPin)
{
    if (endPin == NULL || startPin == NULL)  return false;
    if (endPin->takeFrom)  return false;
    if (startPin->type == Pin::OUTPUT_PIN && endPin->type == Pin::INPUT_PIN)
         endPin->takeFrom = startPin;
    else return false;
    return true;
}

void FilterGraph::breakConnection(Pin* startPin, Pin* endPin)
{
    if (endPin == NULL || startPin == NULL)  return;
    endPin->takeFrom = NULL;
}

void FilterGraph::removeConnections(Pin* pin)
{
    for (unsigned int i = 0; i < blocks.size(); i++)
       for (unsigned int j = 0; j < blocks[i]->inputPins.size(); j++)
          if (blocks[i]->inputPins[j]->takeFrom == pin)
              blocks[i]->inputPins[j]->takeFrom = NULL;
}

/* This is totally uneffective, please fix */
bool FilterGraph::topologicSort()
{
    /* Count inputs to blocks */
    for (unsigned int i = 0; i < blocks.size(); i++)
    {  blocks[i]->inLinks = 0;
       for (unsigned int j = 0; j < blocks[i]->inputPins.size(); j++)
          if ( blocks[i]->inputPins[j]->takeFrom != NULL &&
               !blocks[i]->isOuterPin(blocks[i]->inputPins[j]) )
             blocks[i]->inLinks++;
    }

    vector<FilterBlock*> result;
    while(true)
    {   /* Find Blocks with no links */
        vector<FilterBlock *>::iterator itBlocks;
        for (itBlocks = blocks.begin(); itBlocks != blocks.end(); ++itBlocks)
            if ((*itBlocks)->inLinks == 0)  break;

        if (itBlocks == blocks.end()) /* No objects found */
            return false;

        result.push_back(*itBlocks);

        /* Count inputs to blocks */
        for (unsigned int i = 0; i < blocks.size(); i++)
           for (unsigned int j = 0; j < blocks[i]->inputPins.size(); j++)
               if (blocks[i]->inputPins[j]->takeFrom != NULL &&
                   blocks[i]->inputPins[j]->takeFrom->parent == *itBlocks)
               {
                   blocks[i]->inLinks--;
               }

        blocks.erase(itBlocks);
        if (blocks.size() == 0)  break;
    } // while(true)
    blocks = result;
    return true;
} // topologicSort

void FilterGraph::execute()
{
    /* Ok, we expect graph to be topologically sorted*/
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        FilterBlock *block = blocks[i];
        for (unsigned int j = 0; j < block->inputPins.size(); j++)
        {
            block->inputPins[j]->setPin(block->inputPins[j]->takeFrom);
        }

        Statistics::startInterval(stats);
            block->operator ()();
        Statistics::endInterval(stats, block->getFullName());
    } // for

    for (unsigned int i = 0; i < blocks.size(); i++)
        if (blocks[i]->sort == FilterBlock::PROCESSING_FILTER)  blocks[i]->clear();
} // execute

void FilterGraph::clearAllData()
{
    for (unsigned int i = 0; i < blocks.size(); i++)
       if (blocks[i]->sort != FilterBlock::INPUT_FILTER)  blocks[i]->clear();
}

void FilterGraph::print()
{
    printf("Quantity of blocks: %" PRISIZE_T "\n", blocks.size());
    vector<FilterBlock *>::iterator itBlocks;
    for (itBlocks = blocks.begin(); itBlocks != blocks.end(); ++itBlocks)
    {
        printf("%s\n", (*itBlocks)->getInstanceName());
        for (unsigned int j = 0; j < (*itBlocks)->inputPins.size(); j++)
            if ((*itBlocks)->inputPins[j]->takeFrom != NULL)
                printf("%s:%s -> %s:%s\n",
                       (*itBlocks)->inputPins[j]->takeFrom->parent->getInstanceName(),
                       (*itBlocks)->inputPins[j]->takeFrom->instanceName,
                       (*itBlocks)->getInstanceName(),
                       (*itBlocks)->inputPins[j]->instanceName);
    }
} // print

void FilterGraph::saveToFile(const char* filename)
{
    FILE *file = fopen(filename, "w");
    XMLPrinter* printer = new XMLPrinter(file);
    serialize(printer);
    fclose(file);
    delete printer;
}

//    void saveToFile(XMLPrinter* stream)
//    {
//        FILE *file = fopen(filename, "w");
//        XMLPrinter* printer = new XMLPrinter(stream);
//        serialize(printer);
//        fclose(file);
//        delete printer;
//    }

void FilterGraph::loadFromFile(const char* filename)
{
//        FILE *file = fopen("graph.conf", "r");
//        if (NULL == file)  return;
//        XMLDocument doc;
//        XMLError err = doc.LoadFile(file);
//        if (err != XML_SUCCESS)  return;
//        deserialize(&doc);
//        fclose(file);

    XMLDocument doc;
    XMLError err = doc.LoadFile(filename);
    if (err != XML_SUCCESS)  return;
    deserialize(&doc);
}

void FilterGraph::serialize(XMLPrinter* stream)
{
    XMLDocument* doc = serialize();
    doc->Print(stream);
    delete doc;
}

XMLDocument* FilterGraph::serialize()
{
    XMLDocument* doc = new XMLDocument();
    topologicSort();
    for (unsigned int i = 0; i < blocks.size(); i++)
       blocks[i]->serialize(doc);

    return doc;
}

void FilterGraph::deserialize(const char* xml)
{
    XMLDocument* doc = new XMLDocument();
    XMLError errorId = doc->Parse(xml);
    if (doc->Error())
    {   cout << "Parsing Error: " << errorId << endl;
        delete doc;
        return;
    }
    deserialize(doc);
    delete doc;
}

void FilterGraph::deserialize(XMLDocument* doc)
{
    XMLNode* p = doc->FirstChild();
    while (p)
    {   FilterBlock* filter = collection->getBlockFromName(p->ToElement()->Attribute("type"),
                                                           p->ToElement()->IntAttribute("id"));
        addBlock(filter);
        filter->deserialize(p);
        p = p->NextSibling();
    }
} // deserialize

FilterGraph* FilterGraph::ultimateParent()
{
    FilterGraph* p = this;
    while (p->parent)  p = p->parent;
    return p;
}

void FilterGraph::renewCompoundFilters(int _typeid, FilterBlock* blockToIgnore)
{
    for (unsigned int i = 0; i < blocks.size(); i++)
    {   CompoundFilter* b = dynamic_cast<CompoundFilter*>(blocks[i]);
        if (b == NULL || b == blockToIgnore)  continue;
        if (b->typeId == _typeid)
             b->loadGraph();
        else b->graph->renewCompoundFilters(_typeid, blockToIgnore);
    }
}

} /* namespace corecvs */
