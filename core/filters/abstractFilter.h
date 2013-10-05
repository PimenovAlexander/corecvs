#ifndef ABSTRACT_FILTER_H_
#define ABSTRACT_FILTER_H_
/**
 * \file abstractFilter.h
 * \brief Add Comment Here
 *
 * \date May 1, 2012
 * \author alexander
 */


#include "g12Buffer.h"

namespace corecvs
{

class AbstractFilter
{
public:
    AbstractFilter();


    static const char * getName()
    {
        return "Base";
    }

    virtual const char * getInstanceName()
    {
        return AbstractFilter::getName();
    }

    /* TODO: Change this ASAP. Should have a common base class for parameters */
    virtual bool setParameters(const void *newParameters) = 0;

    virtual G12Buffer *filter (G12Buffer *input) = 0;


    virtual ~AbstractFilter();
};

} /* namespace corecvs */
#endif /* ABSTRACT_FILTER_H_ */
