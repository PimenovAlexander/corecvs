#ifndef GAIN_OFFSET_FILTER_H_
#define GAIN_OFFSET_FILTER_H_
#include "filterBlock.h"
#include "g12Buffer.h"
#include "../xml/generated/gainOffsetParameters.h"
#include "filtersCollection.h"

/**
 * \file gainOffsetFilter.h
 * \brief Add Comment Here
 *
 * \date May 1, 2012
 * \author alexander
 */

namespace corecvs
{
using namespace tinyxml2;

class GainOffsetFilter : public FilterBlock
{
public:
//    GainOffsetFilter();

    GainOffsetFilter(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = max(instanceCounter, instanceId);

        inputPins.push_back (new G12Pin(this, Pin::INPUT_PIN, (id == -1),  "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    virtual int getInstanceId() { return instanceId; }

    virtual const char* getTypeName() const
    {
        return FiltersCollection::typenames[FiltersCollection::GAIN_OFFSET_FILTER];
    }

    virtual int operator()();

    GainOffsetParameters mGainOffsetParameters;
    virtual bool setParameters(const void *newParameters)
    {
        mGainOffsetParameters = *(GainOffsetParameters *)newParameters;
        return true;
    }
    virtual void *getParameters()
    {
        return (void*)&mGainOffsetParameters;
    }

    virtual XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

    virtual ~GainOffsetFilter();

private:
    G12Buffer *input;
    G12Buffer *result;

    static int instanceCounter;
};

} /* namespace corecvs */
#endif /* GAIN_OFFSET_FILTER_H_ */
