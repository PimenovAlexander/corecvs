#pragma once
/**
 * \file floatFlowBuffer.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 20, 2010
 * \author alexander
 */
#include <iostream>

#include "global.h"

#include "vector2d.h"
#include "flowBuffer.h"
#include "g12Buffer.h"
#include "abstractContiniousBuffer.h"
#include "kltGenerator.h"

#include "preciseInterpolationType.h"
#include "makePreciseParameters.h"

namespace corecvs {


class FloatFlow
{
public:
    bool isKnown;
    Vector2dd vector;
    FloatFlow() {}
    FloatFlow(int    val)               : isKnown(val != 0), vector(0.0) {}
    FloatFlow(bool  _isKnown)           : isKnown(_isKnown), vector(0.0) {}
    FloatFlow(const Vector2dd &_vector) : isKnown(true), vector(_vector) {}
};


typedef AbstractContiniousBuffer<FloatFlow, int32_t> FloatFlowBufferBase;

class FloatFlowBuffer :
    public FloatFlowBufferBase,
    public PunchedBufferOperations<FloatFlowBuffer, FloatFlowBufferBase::InternalElementType>
{
public:
    FloatFlowBuffer() : FloatFlowBufferBase () {};

    FloatFlowBuffer(int32_t h, int32_t w) : FloatFlowBufferBase (h,w) {}
    FloatFlowBuffer(int32_t h, int32_t w, bool shouldInit) : FloatFlowBufferBase (h,w, shouldInit) {}

    FloatFlowBuffer(FloatFlowBuffer &that) : FloatFlowBufferBase (that) {}
    FloatFlowBuffer(FloatFlowBuffer *that) : FloatFlowBufferBase (that) {}
    explicit FloatFlowBuffer(FlowBuffer *that);


    FloatFlowBuffer(FloatFlowBuffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        FloatFlowBufferBase(src, x1, y1, x2, y2) {}

    FloatFlowBuffer(int32_t h, int32_t w, uint16_t *data) : FloatFlowBufferBase(h, w, data) {}

    inline bool isElementKnown(const int32_t y, const int32_t x) const
    {
        return this->element(y, x).isKnown;
    }

    inline void setElementUnknown(const int32_t y, const int32_t x)
    {
        this->element(y, x).isKnown = false;
    }

    static FloatFlowBuffer *preciseFlow(
            FlowBuffer *flow,
            G12Buffer *first,
            G12Buffer *second,
            const MakePreciseParameters &params
    )
    {
//    	printf("FloatFlowBuffer::preciseFlow() called\n");
        FloatFlowBuffer *toReturn = NULL;

        SpatialGradient *sg = new SpatialGradient(first);
        SpatialGradientIntegralBuffer *gradient = new SpatialGradientIntegralBuffer(sg);
        delete_safe(sg);


        switch (params.interpolation())
        {
            case PreciseInterpolationType::BILINEAR:
                toReturn = preciseFlowCalc<BilinearInterpolator>(flow, first, second, gradient, params);
            break;
            case PreciseInterpolationType::POLYNOM:
                toReturn = preciseFlowCalc<PolynomInterpolator> (flow, first, second, gradient, params);
            break;
            case PreciseInterpolationType::SPLINE:
                toReturn = preciseFlowCalc<Splain3Interpolator> (flow, first, second, gradient, params);
            break;
            default:
            break;
        }

        delete_safe(gradient);
        return toReturn;
    }

    template<typename InterpolationType>
    static FloatFlowBuffer *preciseFlowCalc(FlowBuffer *flow,
                                 G12Buffer *first,
                                 G12Buffer *second,
                                 SpatialGradientIntegralBuffer *gradient,
                                 const MakePreciseParameters &params
                                 )
    {
        FloatFlowBuffer *toReturn = new FloatFlowBuffer(first->h, first->w, false);

        KLTCalculationContext context;
        context.first = first;
        context.second = second;
        context.gradient = gradient;

        KLTGenerator<InterpolationType> kltGenerator(
                Vector2d32(params.kLTRadiusW(), params.kLTRadiusH()),
                params.kLTIterations());
        for (int i = 0; i < first->h; i++)
        {
            for (int j = 0; j < first->w; j++)
            {
                FloatFlow preciseFlow(false);
                if (flow->isElementKnown(i,j))
                {
                    FlowElement shift = flow->element(i,j);
                    Vector2dd guess = Vector2dd(shift.x(), shift.y());
                    bool status = kltGenerator.kltIterationSubpixel(context, Vector2dd(j,i), &guess, 2);
                    if (status) {
                        preciseFlow = FloatFlow(guess);
                    }
                }
                toReturn->element(i,j) = preciseFlow;
            }
        }
        return toReturn;
    }

};


} //namespace corecvs

/* EOF */
