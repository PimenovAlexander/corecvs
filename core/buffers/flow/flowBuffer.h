/**
 * \file flowBuffer.h
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 27, 2010
 * \author alexander
 */

#ifndef CFLOWBUFFER_H_
#define CFLOWBUFFER_H_

#include <vector>
#include <stdint.h>
#ifdef WITH_SSE
#include <emmintrin.h>
#include "core/math/sse/sseWrapper.h"
#endif

#include "core/utils/global.h"

#include "core/buffers/flow/punchedBufferOperations.h"
#include "core/buffers/abstractContiniousBuffer.h"
#include "core/math/vector/vector2d.h"
#include "core/buffers/flow/flowVector.h"
#include "core/math/matrix/matrix33.h"

namespace corecvs {

#ifndef INT16_MAX
#define INT16_MAX (0x7FFF)
#endif

typedef Vector2d16 FlowElement;
typedef AbstractContiniousBuffer< FlowElement , int32_t> FlowBufferBase;


/**
 *  This is actually a version of Displacement Buffer.
 *  TODO: Merge common code
 * */
class FlowBuffer :
    public FlowBufferBase,
    public PunchedBufferOperations<FlowBuffer, FlowBufferBase::InternalElementType>
{
public:

    //@{
    /**
     * \brief This constant stores the value that denotes the unknown flow
     * Not really supported them to be different. Sorry
     */
    static const int FLOW_UNKNOWN_X = INT16_MAX;
    static const int FLOW_UNKNOWN_Y = INT16_MAX;
    //@}

#ifdef WITH_SSE
#define CFLOW_X_MASK_SSE    Int32x4(0xFFFF)
#define CFLOW_Y_MASK_SSE    Int32x4(0xFFFF0000)
#define CFLOW_UNKNOWN_X_SSE Int32x4(INT16_MAX)
#define CFLOW_UNKNOWN_Y_SSE Int32x4(INT16_MAX << 16)
#define CFLOW_UNKNOWN_SSE   Int32x4((INT16_MAX << 16) | INT16_MAX)


#define CFLOW_X_MASK_VEC    Int32xN(0xFFFF)
#define CFLOW_Y_MASK_VEC    Int32xN(0xFFFF0000)
#define CFLOW_UNKNOWN_X_VEC Int32xN(INT16_MAX)
#define CFLOW_UNKNOWN_Y_VEC Int32xN(INT16_MAX << 16)
#define CFLOW_UNKNOWN_VEC   Int32xN((INT16_MAX << 16) | INT16_MAX)


#endif

    FlowBuffer(int32_t h, int32_t w) :
        FlowBufferBase ( h, w, FlowElement(FlowBuffer::FLOW_UNKNOWN_X, FlowBuffer::FLOW_UNKNOWN_Y))
    {}

    FlowBuffer(Vector2d<int32_t> size) :
        FlowBufferBase ( size, FlowElement(FlowBuffer::FLOW_UNKNOWN_X, FlowBuffer::FLOW_UNKNOWN_Y))
    {}


    FlowBuffer(FlowBuffer &that) : FlowBufferBase (that) {}
    FlowBuffer(FlowBuffer *that) : FlowBufferBase (that) {}

    FlowBuffer(FlowBuffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        FlowBufferBase(src, x1, y1, x2, y2) {}

    FlowBuffer(uint32_t h, uint32_t w, bool shouldClear, uint32_t stride = STRIDE_AUTO) : FlowBufferBase(h, w, shouldClear, stride) {}

    FlowBuffer(uint32_t h, uint32_t w, FlowElement *data) : FlowBufferBase(h, w, data) {}

    FlowBuffer() {}

    inline bool isElementKnown(const int32_t y, const int32_t x) const
    {
        const FlowElement *a = &(this->element(y, x));
        return ((a->x() != (int)FlowBuffer::FLOW_UNKNOWN_X) &&
                (a->y() != (int)FlowBuffer::FLOW_UNKNOWN_Y));
    }

    inline bool isElementValidAndKnown(const int32_t y, const int32_t x) const
    {
        return isValidCoord(y,x) && isElementKnown(y,x);
    }


    inline void setElementUnknown(const int32_t y, const int32_t x)
    {
        FlowElement *a = &element(y, x);
        a->x() = FLOW_UNKNOWN_X;
        a->y() = FLOW_UNKNOWN_Y;
    }

    std::vector<FloatFlowVector> *toVectorForm();

    FlowBuffer *invert();

    Vector2dd   getMeanDifference();


#ifdef WITH_SSE
    /*TODO: Move this to some more generic place*/

    /* Same with wrappers */
    /*TODO: Move this to some more generic place*/
    ALIGN_STACK_SSE static inline Int32x4 l1metric_sse(const Int16x8 &dist) {
        // Note, it is absolutely equal to the integer operations
        return (Int32x4(dist.data) & CFLOW_X_MASK_SSE) + shiftLogical(Int32x4(dist.data), 16);
    }

    ALIGN_STACK_SSE inline Int32x4 element_sse_w(const int32_t y, const int32_t x) {
        return Int32x4((int32_t *)&(this->element(y, x)));
    }
    /* Note this differs form the normal version a little bit */
    ALIGN_STACK_SSE inline static Int32x4 isFlowUnknown_sse(const Int32x4 &item) {
        return (item == CFLOW_UNKNOWN_SSE);
    }

    ALIGN_STACK_SSE inline void setElement_sse(const int32_t y, const int32_t x, const Int32x4 &item) {
        item.save((int32_t *)&element(y,x));
    }

    /* Wrapper for the largest possible type */


    ALIGN_STACK_SSE static inline Int32xN l1metric_vec(const Int16xN &dist) {
        // Note, it is absolutely equal to the integer operations
        return (Int32xN(dist.data) & CFLOW_X_MASK_VEC) + shiftLogical(Int32xN(dist.data), 16);
    }

    ALIGN_STACK_SSE inline Int32xN element_vec_w(const int32_t y, const int32_t x) {
        return Int32xN((int32_t *)&(this->element(y, x)));
    }
    /* Note this differs form the normal version a little bit */
    ALIGN_STACK_SSE inline static Int32xN isFlowUnknown_vec(const Int32xN &item) {
        return (item == CFLOW_UNKNOWN_VEC);
    }

    ALIGN_STACK_SSE inline void setElement_vec(const int32_t y, const int32_t x, const Int32xN &item) {
        item.save((int32_t *)&element(y,x));
    }




#endif



    static FlowBuffer* load(const string& path);
    bool               dump(const string& path) const;

    // Should I think about this?
    ~FlowBuffer() {}
};

typedef FlowBuffer DisparityBuffer;

} //namespace corecvs

#endif /* CFLOWBUFFER_H_ */

