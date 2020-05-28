/**
 * \file derivativeBuffer.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 13, 2010
 * \author alexander
 */

#ifndef DERIVATIVEBUFFER_H_
#define DERIVATIVEBUFFER_H_

#include <stdint.h>

#include "utils/global.h"

#include "math/vector/vector2d.h"
#include "buffers/abstractBuffer.h"
#include "buffers/g12Buffer.h"

namespace corecvs {

typedef AbstractBuffer<Vector2d<int16_t> > DerivativeBufferBase;

/*
template<TypeName>
class DeriverOperator
{
public:
    Vector2d<int16_t>* operator() (int x, int y)
};*/

class DerivativeBuffer : public DerivativeBufferBase
{

public:
    DerivativeBuffer(G12Buffer *input);

    G12Buffer *nonMaximalSuppression(int downscale = 2);
    G12Buffer *gradientMagnitudeBuffer(int downscale = 2);

    double elementMagnitude(int i, int j)
    {
        const Vector2d<int16_t> &element = this->element(i,j);
        Vector2dd vector(element.x(), element.y());
        return vector.getLengthStable();
    }


    DerivativeBuffer() {}
};



} //namespace corecvs
#endif /* DERIVATIVEBUFFER_H_ */

