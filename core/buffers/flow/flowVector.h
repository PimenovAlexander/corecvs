#ifndef FLOWVECTOR_H_
#define FLOWVECTOR_H_
/**
 * \file flowVector.h
 * \brief a header for FlowVector.c
 *
 * \date Feb 21, 2011
 * \author alexander
 */


#include <iostream>

#include "core/math/vector/vector2d.h"

namespace corecvs {

class FloatFlowVector
{
public:
    Vector2dd start;
    Vector2dd end;

    FloatFlowVector(Vector2dd _start, Vector2dd _end) :
        start(_start), end(_end) {}

    friend ostream & operator <<(ostream &out, const FloatFlowVector &flowVector)
    {
        out << "[" << flowVector.start << " = > " << flowVector.end << "]";
        return out;
    }

};


} //namespace corecvs
#endif  //FLOWVECTOR_H_

