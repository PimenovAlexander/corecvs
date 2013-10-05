#ifndef OUTPUTFILTERBLOCK_H
#define OUTPUTFILTERBLOCK_H
#include "filterBlock.h"
#include "g12Buffer.h"
#include "../xml/generated/outputFilterParameters.h"
#include "filtersCollection.h"

namespace corecvs
{

class OutputFilter : public FilterBlock
{
public:
    enum FrameName {
       LEFT_FRAME,
       RIGHT_FRAME
    };

    FrameName frame;

    OutputFilter(int id = -1) : FilterBlock(id)
    {
        sort = OUTPUT_FILTER;
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = max(instanceCounter, instanceId);

        inputPins.push_back(new G12Pin(this, Pin::INPUT_PIN, (id == -1), "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

//    virtual void clear();
    virtual ~OutputFilter();

    virtual void setInstanceName(char* name)
    {
        instanceName = name;
        free (outputPins[0]->instanceName);
        outputPins[0]->instanceName = strdup(instanceName);
//        outputPins[0]->instanceName = instanceName;
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const
    {
        return FiltersCollection::typenames[FiltersCollection::OUTPUT_FILTER];
    }

    virtual int operator()(void);

    OutputFilterParameters mOutputParameters;

//    G12Buffer* GetBuffer(void){ return buffer; }

    virtual bool setParameters(const void* newParameters)
    {
        mOutputParameters = *(OutputFilterParameters *)newParameters;
        return true;
    }
    virtual void *getParameters()
    {
        return (void*)&mOutputParameters;
    }

    virtual XMLNode* serialize(XMLNode*);
    virtual void deserialize(XMLNode*, bool = true);

private:
    static int instanceCounter;
};

} /* namespace corecvs */
#endif // OUTPUTFILTERBLOCK_H
