#pragma once
/**
 * \file kltGenerator.h
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 23, 2010
 * \author alexander
 */
#include "buffers/mipmapPyramid.h"
#include "buffers/integralBuffer.h"
#include "buffers/g12Buffer.h"
#include "buffers/flow/flowBuffer.h"
#include "math/vector/vector3d.h"
#include "math/vector/vector2d.h"
#include "buffers/interpolator.h"
#include "buffers/kernels/spatialGradient.h"
#include "utils/global.h"
#include "math/mathUtils.h"
#include "buffers/mipmapPyramid.h"

namespace corecvs {

class KLTCalculationContext
{
public:
    G12Buffer *first = NULL;
    G12Buffer *second = NULL;
    SpatialGradientIntegralBuffer *gradient = NULL;


    bool isFilled() const
    {
        return (first != NULL) || (second != NULL) || (gradient != NULL);
    }
};

template <typename InterpolationType, typename FloatType = double>
class KLTGenerator
{
public:
    Vector2d32 windowSize;
    int newtonIterations;
    double *maxThresholds;
    double *minThresholds;
    int maxLevels;

    bool trace = false;


    KLTGenerator():
        windowSize(Vector2d32(5,5)),
        newtonIterations(7),
        maxLevels(5)
    {}
    KLTGenerator(Vector2d32 _windowSize, int _newtonIterations = 7, int _maxLevels = 5) :
        windowSize(_windowSize),
        newtonIterations(_newtonIterations),
        maxLevels(_maxLevels)
    {
    }


    FlowBuffer *calculateFlowFromPrevLevelBuffer(
            G12Buffer *first,
            G12Buffer *second,
            FlowBuffer *prevLevelFlow,
            bool isHighestFlowBuffer);


    FlowBuffer *calculateHierarchicalKLTFlow(
            G12Buffer *first,
            G12Buffer *second
            );

    bool kltIteration (
            const KLTCalculationContext &calculationContext,
            const Vector2d32 &point,
            Vector2d<FloatType> *startGuess,
            double nullThreshold) const;

    bool kltIterationSubpixel (
            const KLTCalculationContext &calculationContext,
            const Vector2d<FloatType> &point,
            Vector2d<FloatType> *startGuess,
            float nullThreshold) const;

    /*============ Fast version ============ */
    bool kltIterationSubpixelFast (const KLTCalculationContext &calculationContext,
            const Vector2df &point,
            Vector2df *startGuess,
            float nullThreshold) const;


    virtual ~KLTGenerator(){}
};


extern template class KLTGenerator<BilinearInterpolator , float>;
extern template class KLTGenerator<BilinearInterpolatorD, double>;
extern template class KLTGenerator<Splain3Interpolator, double>;
extern template class KLTGenerator<PolynomInterpolator, double>;

extern template class KLTGenerator<Splain3Interpolator, float>;
extern template class KLTGenerator<PolynomInterpolator, float>;



} //namespace corecvs

/* EOF */
