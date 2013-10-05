#ifndef OPENCVFILTER_H
#define OPENCVFILTER_H


#include "g12Buffer.h"
#include "generated/openCVFilterParameters.h"
#include "filterBlock.h"

class OpenCVFilter: public FilterBlock
{
public:
    OpenCVFilter();

    static char* getName()
    {
        return (char*)"OpenCV Filter";
    }

    virtual char* getInstanceName()
    {
        return OpenCVFilter::getName();
    }

    virtual int operator()();

    OpenCVFilterParameters mOpenCVParameters;

    virtual bool setParameters(const void * newParameters)
    {
        mOpenCVParameters = *(OpenCVFilterParameters *)newParameters;
        return true;
    }

    virtual ~OpenCVFilter();

private:
    G12Buffer *input;
    G12Buffer *result;
};

#endif // OPENCVFILTER_H
