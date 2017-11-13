#ifndef INPUTFILTERBLOCK_H
#define INPUTFILTERBLOCK_H
#include "core/filters/blocks/filterBlock.h"
#include "core/buffers/g12Buffer.h"
#include "../xml/generated/inputFilterParameters.h"
#include "core/filters/filtersCollection.h"

namespace corecvs
{

class InputFilter : public FilterBlock
{
public:
    enum FrameName {
       LEFT_FRAME,
       RIGHT_FRAME
    };

    FrameName frame;

    InputFilter(int id = -1) : FilterBlock(id)
    {
        sort = INPUT_FILTER;
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = CORE_MAX(instanceCounter, instanceId);

        inputPins.push_back(new G12Pin(this, Pin::INPUT_PIN, (id == -1), "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    virtual ~InputFilter();

    virtual void setInstanceName(char* name)
    {
        instanceName = name;
        free (inputPins[0]->instanceName);
        inputPins[0]->instanceName = strdup(instanceName);
//        inputPins[0]->instanceName = instanceName;
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const
    {
        return FiltersCollection::typenames[FiltersCollection::INPUT_FILTER];
    }

    virtual int operator()(void);

    InputFilterParameters mInputParameters;

//    void SetBuffer(G12Buffer* _buffer){ buffer = _buffer; }

    virtual bool setParameters(const void* newParameters)
    {
        mInputParameters = *(InputFilterParameters*)newParameters;
        return true;
    }
    virtual void *getParameters()
    {
        return (void*)&mInputParameters;
    }

    virtual XMLNode* serialize(XMLNode*);
    virtual void deserialize(XMLNode*, bool = true);

private:
    static int instanceCounter;
//    G12Buffer* buffer;
};

} /* namespace corecvs */
#endif // INPUTFILTERBLOCK_H
