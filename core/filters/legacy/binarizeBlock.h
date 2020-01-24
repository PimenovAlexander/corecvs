#pragma once
/**
 * \file binarizeBlock.h
 * \brief Add Comment Here
 *
 * \date Jan 17, 2013
 */

#include "core/filters/legacy/blocks/filterBlock.h"
#include "core/xml/generated/binarizeParameters.h"

namespace corecvs
{
using namespace tinyxml2;

class BinarizeBlock: public FilterBlock
{
public:

    BinarizeBlock(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = instanceId;

        inputPins.push_back (new G12Pin(this, Pin::INPUT_PIN,  (id == -1), "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const { return "Binarize Block"; }

    virtual int operator()();

    BinarizeParameters mBinarizeParameters;

    virtual bool setParameters(const void * newParameters)
    {
        mBinarizeParameters = *(BinarizeParameters *)newParameters;
        return true;
    }

    virtual void *getParameters()
    {
        return (void*)&mBinarizeParameters;
    }

    XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

//    virtual void clear();
    virtual ~BinarizeBlock();

private:
    static int instanceCounter;
};

} /* namespace corecvs */

/* EOF */

