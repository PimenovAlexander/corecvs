#ifndef BIT_SELECTOR_FILTER_H_
#define BIT_SELECTOR_FILTER_H_
/**
 * \file bitSelectorFilter.h
 * \brief Add Comment Here
 *
 * \date May 1, 2012
 * \author alexander
 */

#include "core/filters/legacy/blocks/filterBlock.h"
#include "core/xml/generated/bitSelectorParameters.h"
#include "core/filters/legacy/filtersCollection.h"

namespace corecvs
{
using namespace tinyxml2;

class BitSelectorFilter : public FilterBlock
{
public:

    BitSelectorFilter(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = CORE_MAX(instanceCounter, instanceId);

        inputPins.push_back (new G12Pin(this, Pin::INPUT_PIN, (id == -1),  "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const
    {
        return FiltersCollection::typenames[FiltersCollection::BIT_SELECTOR_FILTER];
    }


//    virtual G12Buffer *filter (G12Buffer *input);
    virtual int operator()();

    BitSelectorParameters mBitSelectorParameters;
    virtual bool setParameters(const void *newParameters)
    {
        mBitSelectorParameters = *(BitSelectorParameters *)newParameters;
        return true;
    }
    virtual void *getParameters()
    {
        return (void*)&mBitSelectorParameters;
    }

    virtual XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

    virtual ~BitSelectorFilter();

private:
    G12Buffer *input;
    G12Buffer *result;

    static int instanceCounter;
};

} /* namespace corecvs */
#endif /* BIT_SELECTOR_FILTER_H_ */
