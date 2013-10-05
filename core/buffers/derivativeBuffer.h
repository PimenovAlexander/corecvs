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

#include "global.h"

#include "vector2d.h"
#include "abstractBuffer.h"
#include "g12Buffer.h"
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

    G12Buffer *nonMaximalSuppression(/*uint16_t low, uint16_t hight*/);
    G12Buffer *gradientMagnitudeBuffer();

    DerivativeBuffer() {}
};



} //namespace corecvs
#endif /* DERIVATIVEBUFFER_H_ */

