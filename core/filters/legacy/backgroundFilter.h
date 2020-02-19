#pragma once
/**
 * \file backgroundFilter.h
 * \brief Add Comment Here
 *
 * \date Oct 11, 2012
 */

#include "filters/legacy/blocks/filterBlock.h"
#include "xml/generated/backgroundFilterParameters.h"
#include "filters/legacy/filtersCollection.h"

namespace corecvs
{
using namespace tinyxml2;

class BackgroundFilter: public FilterBlock
{
public:

    BackgroundFilter(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = CORE_MAX(instanceCounter, instanceId);

        inputPins.push_back (new G12Pin(this, Pin::INPUT_PIN, (id == -1),  "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));

        mPreviousInput = NULL;
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const
    {
        return FiltersCollection::typenames[FiltersCollection::BACKGROUND_FILTER];
    }

    virtual int operator()();

    BackgroundFilterParameters mBackgroundFilterParameters;
    G12Buffer                  *mPreviousInput;

    virtual bool setParameters(const void * newParameters)
    {
        mBackgroundFilterParameters = *(BackgroundFilterParameters *)newParameters;
        return true;
    }
    virtual void *getParameters()
    {
        return (void*)&mBackgroundFilterParameters;
    }

    virtual XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

//    virtual void clear();
    virtual ~BackgroundFilter();

private:
    static int instanceCounter;
};

} /* namespace corecvs */

/* EOF */

