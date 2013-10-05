/**
 * \file energyBuffer.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 2, 2010
 * \author alexander
 */

#ifndef CENERGYBUFFER_H_
#define CENERGYBUFFER_H_

#include "abstractContiniousBuffer.h"
namespace corecvs {

class EnergyBuffer : public AbstractContiniousBuffer<double, int>
{
    typedef AbstractContiniousBuffer<double, int> EnergyBufferBase;

public:

    EnergyBuffer(int h, int w) : EnergyBufferBase (h,w) {};
    EnergyBuffer(EnergyBuffer &that) : EnergyBufferBase (that) {};
    EnergyBuffer(EnergyBuffer *that) : EnergyBufferBase (that) {};

    EnergyBuffer(EnergyBuffer *src, int x1, int y1, int x2, int y2) :
        EnergyBufferBase(src, x1, y1, x2, y2) {};

    EnergyBuffer(int32_t h, int32_t w, double *data) : EnergyBufferBase(h, w, data) {};

    ~EnergyBuffer(){};
};


} //namespace corecvs
#endif /* CENERGYBUFFER_H_ */

