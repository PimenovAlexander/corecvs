#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
/**
 * \file sobelFilter.h
 * \brief Add Comment Here
 *
 * \date May 1, 2012
 * \author alexander
 */

#include "filterBlock.h"
#include "g12Buffer.h"
#include "../xml/generated/sobelParameters.h"
#include "filtersCollection.h"

namespace corecvs
{
using namespace tinyxml2;

class SobelFilter : public FilterBlock
{
public:

    SobelFilter(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = CORE_MAX(instanceCounter, instanceId);

        inputPins.push_back(new G12Pin(this, Pin::INPUT_PIN, (id == -1), "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const
    {
        return FiltersCollection::typenames[FiltersCollection::SOBEL_FILTER];
    }

    virtual int operator()(void);

    SobelParameters mSobelParameters;

    virtual bool setParameters(const void* newParameters)
    {
        mSobelParameters = *(SobelParameters*)newParameters;
        return true;
    }
    virtual void *getParameters()
    {
        return (void*)&mSobelParameters;
    }

    virtual XMLNode* serialize(XMLNode*);
    virtual void deserialize(XMLNode*, bool = true);

    virtual ~SobelFilter();
private:
    static int instanceCounter;
};

} /* namespace corecvs */
#endif /* SOBEL_FILTER_H_ */
