#ifndef ABSTRACTMEANSHIFTKERNEL_H_
#define ABSTRACTMEANSHIFTKERNEL_H_

/**
 * \file abstractMeanShiftKernel.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author tbryksin
 * \author ylitvinov
 */

#include "global.h"
namespace corecvs {

class AbstractMeanShiftKernel
{
public:
    virtual void setWindow(
            unsigned const windowMinX,
            unsigned const windowMinY,
            unsigned const windowMaxX,
            unsigned const windowMaxY) = 0;

    virtual unsigned value(
            unsigned const x,
            unsigned const y) const = 0;

    virtual ~AbstractMeanShiftKernel() {};
};


} //namespace corecvs
#endif //ABSTRACTMEANSHIFTKERNEL_H_

