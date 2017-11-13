#pragma once
/**
 * \file thickeningBlock.h
 * \brief Add Comment Here
 *
 * \date Jan 17, 2013
 */

#include "core/filters/blocks/filterBlock.h"
#include "../xml/generated/thickeningParameters.h"

namespace corecvs
{
using namespace tinyxml2;

class ThickeningBlock: public FilterBlock
{
public:

    ThickeningBlock(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = instanceId;

        inputPins.push_back (new G12Pin(this, Pin::INPUT_PIN,  (id == -1), "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const { return "Thickening Block"; }

    virtual int operator()();

    ThickeningParameters mThickeningParameters;

    virtual bool setParameters(const void * newParameters)
    {
        mThickeningParameters = *(ThickeningParameters*)newParameters;
        return true;
    }

    virtual void *getParameters()
    {
        return (void*)&mThickeningParameters;
    }

    XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

//    virtual void clear();
    virtual ~ThickeningBlock();

private:
    static int instanceCounter;
};

} /* namespace corecvs */

/* EOF */

