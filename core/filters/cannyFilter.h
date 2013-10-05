#ifndef CANNY_FILTER_H_
#define CANNY_FILTER_H_
/**
 * \file cannyFilter.h
 * \brief Add Comment Here
 *
 * \date Sept 13, 2012
 * \author a.melnikov
 */

#include "filterBlock.h"
#include "../xml/generated/cannyParameters.h"
#include "filtersCollection.h"
#include "g12Buffer.h"

namespace corecvs
{
using namespace tinyxml2;

class CannyFilter : public FilterBlock
{
public:
    static const int white = G12Buffer::BUFFER_MAX_VALUE;
    static const int gray  = G12Buffer::BUFFER_MAX_VALUE / 2;


    CannyFilter(int id = -1) : FilterBlock(id)
    {
        if (instanceId == -1)
             instanceId = ++instanceCounter;
        else instanceCounter = max(instanceCounter, instanceId);

        inputPins.push_back (new G12Pin(this, Pin::INPUT_PIN, (id == -1),  "0"));
        outputPins.push_back(new G12Pin(this, Pin::OUTPUT_PIN, (id == -1), "0"));
    }

    virtual int getInstanceId() const { return instanceId; }

    virtual const char* getTypeName() const
    {
        return FiltersCollection::typenames[FiltersCollection::CANNY_FILTER];
    }

    virtual int operator()();

    CannyParameters mCannyParameters;

    virtual bool setParameters(const void * newParameters)
    {
        mCannyParameters = *(CannyParameters *)newParameters;
        return true;
    }
    virtual void *getParameters()
    {
        return (void*)&mCannyParameters;
    }

    virtual XMLNode* serialize(XMLNode* node);
    virtual void deserialize(XMLNode*, bool = true);

    virtual ~CannyFilter();
private:
    G12Buffer *input;
    G12Buffer *result;

    static void recursiveEdgeProver(G12Buffer *buffer, int h, int w);


    static int instanceCounter;
};

} /* namespace corecvs */
#endif /* CANNY_FILTER_H_ */
