#pragma once
/**
 * \file maskFilterBlock.h
 * \brief Add Comment Here
 *
 * \date Jan 17, 2013
 */

#include "filterBlock.h"
#include "../xml/generated/maskingParameters.h"

namespace corecvs
{
using namespace tinyxml2;

class MaskFilterBlock: public FilterBlock
{
public:

    MaskFilterBlock(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = instanceId;

        inputPins.push_back (new G12Pin(this, Pin::INPUT_PIN,  (id == -1), "0"));
        inputPins.push_back (new G12Pin(this, Pin::INPUT_PIN,  (id == -1), "1"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const { return "Mask Filter Block"; }

    virtual int operator()();

    MaskingParameters mMaskingParameters;

    virtual bool setParameters(const void * newParameters)
    {
        mMaskingParameters = *(MaskingParameters *)newParameters;
        return true;
    }

    virtual void *getParameters()
    {
        return (void*)&mMaskingParameters;
    }

    XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

//    virtual void clear();
    virtual ~MaskFilterBlock();

private:
    static int instanceCounter;
};

} /* namespace corecvs */

/* EOF */
