#ifndef COMPOUNDFILTER_H
#define COMPOUNDFILTER_H

#include "core/filters/blocks/filterBlock.h"
#include "core/filters/blocks/filterGraph.h"
#include "core/filters/filtersCollection.h"

namespace corecvs
{
using namespace tinyxml2;

class CompoundFilter : public FilterBlock
{
public:
    CompoundFilter(//FilterGraph* _parent,
                   FiltersCollection* collection,
                   int _typeid,
                   int id = -1);

    ~CompoundFilter();
    virtual void clear();

    void loadGraph();
    void stealPinsFromGraph();
    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const
    {
//        return "Compound Filter";
        return graph->collection->compoundFilters[typeId].name.c_str();
    }

    bool dependsOnMe(int filter)
    {
        return graph->collection->dependsOnMe(typeId, filter);
    }

    virtual int operator()()
    {
        graph->execute();
        return 0;
    }

    virtual bool setParameters(const void * /*newParameters*/)
    {
        return true;
    }
    virtual void *getParameters()
    {
        return NULL;
    }

    void addInputPin(Pin* pin)
    {
        addPin(inputPins, pin);
    }

    void addOutputPin(Pin* pin)
    {
        addPin(outputPins, pin);
    }

    void addPin(vector<Pin*> &vec, Pin* pin)
    {
        pin->parent = this;
        vec.push_back(pin);
    }

    void removeInputPin(Pin* pin)
    {
        removePin(inputPins, pin);
    }

    void removeOutputPin(Pin* pin)
    {
        graph->removeConnections(pin);
        removePin(outputPins, pin);
    }

    void removePin(vector<Pin*> &vec, Pin* pin)
    {
        vector<Pin*>::iterator pinsIterator  = std::find(vec.begin(), vec.end(), pin);
        if (pinsIterator != vec.end())  vec.erase(pinsIterator);
    }

    virtual XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

    int typeId;
    FilterGraph* graph;
private:
//    char *typeName;
    static int instanceCounter;
}; // CompoundFilter

} /* namespace corecvs */
#endif // COMPOUNDFILTER_H
