#ifndef ELEMENT6D_H_
#define ELEMENT6D_H_
/**
 * \file element6D.h
 * \brief Add Comment Here
 *
 * \date Aug 3, 2011
 * \author alexander
 */

#include "core/buffers/flow/flowBuffer.h"
namespace corecvs {


/**
 *  This class holds fully or partially reconstructed 6D point at disparity level
 *
 **/
class Element6D
{
public:

    Element6D()
        : flow(Element6D::CFLOW_UNKNOWN_X, Element6D::CFLOW_UNKNOWN_Y)
        , disp(Element6D::CFLOW_UNKNOWN_X)
        , dispPrev(Element6D::CFLOW_UNKNOWN_X)
    {
    }

    Element6D(Vector2d16 const &flowValue, int32_t const dispValue, int32_t const dispPrevValue)
        : flow(flowValue)
        , disp(dispValue)
        , dispPrev(dispPrevValue)
    {
    }

    //@{
    /**
     * Constants that denote unknown values
     **/
    static const int CFLOW_UNKNOWN_X = INT16_MAX;
    static const int CFLOW_UNKNOWN_Y = INT16_MAX;
    //@}

    bool hasFlow() const
    {
        return
            (flow.x() != (int) Element6D::CFLOW_UNKNOWN_X) &&
            (flow.y() != (int) Element6D::CFLOW_UNKNOWN_Y);
    }

    bool hasDisp() const
    {
        return (disp  != (int) Element6D::CFLOW_UNKNOWN_X);
    }

    bool hasPrevDisp() const
    {
        return (dispPrev  != (int) Element6D::CFLOW_UNKNOWN_X);
    }

    bool is5D () const
    {
        return hasFlow() && hasDisp();
    }

    bool is6D () const
    {
        return is5D() && hasPrevDisp();
    }


    Vector2dd floatFlow;
    Vector2d16 flow;
    int32_t disp;
    int32_t dispPrev;
};




} //namespace corecvs
#endif /* ELEMENT6D_H_ */

