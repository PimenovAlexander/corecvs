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



#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"
#include "core/buffers/flow/flowBuffer.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/abstractContiniousBuffer.h"
#include "core/kltflow/kltGenerator.h"

#include "core/xml/generated/preciseInterpolationType.h"
#include "core/xml/generated/makePreciseParameters.h"

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
    FloatFlow(const double &vx, const double &vy)
                                        : isKnown(true), vector(vx,vy)   {}
    FloatFlow(const double &vx, const double &vy, bool _isKnown)
                                        : isKnown(_isKnown), vector(vx,vy)   {}



    FloatFlow& operator=(const FloatFlow &other) { isKnown = other.isKnown; vector = other.vector; return *this; }
};


typedef AbstractContiniousBuffer<FloatFlow, int32_t> FloatFlowBufferBase;

class Statistics;

struct  FloatFlowQualityData
{
    FloatFlowQualityData ()
    {
        corecvs::DefaultSetter setter;
        accept(setter);
    }

    double gtDensity;
    double density;

    double percent;
    double Finliers;
    double afe;

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(gtDensity, 0.0, "gtDensity");
        visitor.visit(density, 0.0, "density");
        visitor.visit(percent, 0.0, "precent");
        visitor.visit(Finliers, 0.0, "Finliers");
        visitor.visit(afe, 0.0, "AFE");
    }

    friend ostream& operator << (ostream &out, FloatFlowQualityData &toSave)
    {
        PrinterVisitor printer(out);
        toSave.accept<PrinterVisitor>(printer);
        return out;
    }

    void addToStats(Statistics *stats);

    void addOther(const FloatFlowQualityData &other)
    {
        gtDensity += other.gtDensity;
        density   += other.density;

        percent   += other.percent;
        Finliers  += other.Finliers;
        afe       += other.afe;
    }

    void divide(double value)
    {
        gtDensity /= value;
        density   /= value;

        percent   /= value;
        Finliers  /= value;
        afe       /= value;
    }
};

class FloatFlowBuffer :
    public FloatFlowBufferBase,
    public PunchedBufferOperations<FloatFlowBuffer, FloatFlowBufferBase::InternalElementType>
{
public:
    FloatFlowBuffer() : FloatFlowBufferBase () {}

    FloatFlowBuffer(int32_t h, int32_t w) : FloatFlowBufferBase (h,w) {}
    FloatFlowBuffer(int32_t h, int32_t w, bool shouldInit) : FloatFlowBufferBase (h,w, shouldInit) {}

    FloatFlowBuffer(FloatFlowBuffer &that) : FloatFlowBufferBase (that) {}
    FloatFlowBuffer(FloatFlowBuffer *that) : FloatFlowBufferBase (that) {}

    explicit FloatFlowBuffer(const FlowBuffer *that);
    FlowBuffer *toFlowBuffer() const;


    FloatFlowBuffer(const FloatFlowBuffer &src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        FloatFlowBufferBase(src, x1, y1, x2, y2) {}

    /**
     *  Flow compare against ground truth
     **/


    FloatFlowQualityData compare(FloatFlowBuffer &groundTruth, double percentTreshold = 5, double distanceTreshold = 3);


    /**
     *  Depricated. Ready for removal
     **/

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
        if (flow == NULL || first == NULL || second == NULL )
            return toReturn;

        SpatialGradient *sg = new SpatialGradient(first);
        SpatialGradientIntegralBuffer *gradient = new SpatialGradientIntegralBuffer(sg);
        delete_safe(sg);

        if (params.algorithm() == MakePreciseAlgorithm::KLT)
        {
            switch (params.interpolation())
            {
                case PreciseInterpolationType::BILINEAR:
                    toReturn = preciseFlowCalc<BilinearInterpolatorD>(flow, first, second, gradient, params);
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
        } else if (params.algorithm() == MakePreciseAlgorithm::KLT_FLOAT){
            switch (params.interpolation())
            {
                case PreciseInterpolationType::BILINEAR:
                    toReturn = preciseFlowCalc<BilinearInterpolator, float>(flow, first, second, gradient, params);
                break;
                case PreciseInterpolationType::POLYNOM:
                    toReturn = preciseFlowCalc<PolynomInterpolator, float> (flow, first, second, gradient, params);
                break;
                case PreciseInterpolationType::SPLINE:
                    toReturn = preciseFlowCalc<Splain3Interpolator, float> (flow, first, second, gradient, params);
                break;
                default:
                break;
            }
        } else {
             toReturn = preciseFlowCalcFast(flow, first, second, gradient, params);
        }


        delete_safe(gradient);
        return toReturn;
    }

    template<typename InterpolationType, typename FloatType = double>
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

        KLTGenerator<InterpolationType, FloatType> kltGenerator(
                Vector2d32(params.kLTRadiusW(), params.kLTRadiusH()),
                params.kLTIterations());

        parallelable_for(0, first->h, [&](const corecvs::BlockedRange<int> &r)
        {
            for (int i = r.begin(); i != r.end(); i++)
            {
                for (int j = 0; j < first->w; j++)
                {
                    FloatFlow preciseFlow(false);
                    if (flow->isElementKnown(i,j))
                    {
                        FlowElement shift = flow->element(i,j);
                        Vector2d<FloatType> guess(shift.x(), shift.y());
                        bool status = kltGenerator.kltIterationSubpixel(context, Vector2d<FloatType>(j,i), &guess, 2);
                        if (status) {
                            preciseFlow = FloatFlow(Vector2dd(guess.x(),guess.y()));
                        }
                    }
                    toReturn->element(i,j) = preciseFlow;
                }
            }
        }
        );
        return toReturn;
    }


    static FloatFlowBuffer *preciseFlowCalcFast(FlowBuffer *flow,
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

        KLTGenerator<BilinearInterpolator, float> kltGenerator(
                Vector2d32(params.kLTRadiusW(), params.kLTRadiusH()),
                params.kLTIterations());

        parallelable_for(0, first->h, [&](const corecvs::BlockedRange<int> &r)
        {
            for (int i = r.begin(); i != r.end(); i++)
            {
                for (int j = 0; j < first->w; j++)
                {
                    FloatFlow preciseFlow(false);
                    if (flow->isElementKnown(i,j))
                    {
                        FlowElement shift = flow->element(i,j);
                        Vector2df guess(shift.x(), shift.y());
                        bool status = kltGenerator.kltIterationSubpixelFast(context, Vector2df(j,i), &guess, 2);
                        if (status) {
                            preciseFlow = FloatFlow(Vector2dd(guess.x(),guess.y()));
                        }
                    }
                    toReturn->element(i,j) = preciseFlow;
                }
            }
        }
        );
        return toReturn;
    }

    double getMaximumMagnitude() const;
    void kittiBackgroudFill();

};


} //namespace corecvs

/* EOF */
