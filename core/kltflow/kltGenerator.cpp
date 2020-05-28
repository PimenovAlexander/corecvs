/**
 * \file kltGenerator.cpp
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 23, 2010
 * \author Rastorguev Alexei
 * \author Alexander Pimenov
 */

#undef TRACE

#include "utils/global.h"

#include "math/mathUtils.h"
#include "buffers/mipmapPyramid.h"
#include "kltflow/kltGenerator.h"
#include "math/vector/vector2d.h"

#include "geometry/rectangle.h"

namespace corecvs {

template<typename InterpolationType, typename FloatType>
FlowBuffer *KLTGenerator<InterpolationType, FloatType>::calculateFlowFromPrevLevelBuffer(G12Buffer *first, G12Buffer *second, FlowBuffer *prevLevelFlow, bool isHighestFlowBuffer)
{
    if (!first->hasSameSize(second))
    {
        // TODO: Throw exception here
        printf("Images have different dimensions \n");
        return NULL;
    }

    if (!isHighestFlowBuffer)
    {
        if (prevLevelFlow->hasSameSize(first->h, first->w))
        {
            // TODO: Throw exception here
            printf("New flow buffer have wrong dimensions \n");
            return NULL;
        }
    }

    int i,j;

    double nullThresholdFactor = 1 << 24;
    double nullThreshold = 2;
    int h = first->h;
    int w = first->w;

    FlowBuffer *flow;
    flow = new FlowBuffer(h,w);

    // Yes, this excessively use memory. We should invent something for creating
    // interface for g12buffer and not lose the performance
    SpatialGradient *sg = new SpatialGradient(first);
    SpatialGradientIntegralBuffer *gradient = new SpatialGradientIntegralBuffer(sg);
    delete sg;

    KLTCalculationContext context;
    context.first = first;
    context.second = second;
    context.gradient = gradient;

    Vector2d<FloatType> g;

    for(i = 0; i < h; i++)
    {
        for(j = 0; j < w; j++)
        {
            Vector2d32 currCoord = Vector2d32(j, i);
            Vector2d32 prevCoord = currCoord / 2;
            bool status;

            if (isHighestFlowBuffer)
            {
                g = Vector2d<FloatType>(0.0);
            }
            else
            {
                if (!(prevLevelFlow->isElementKnownVec(prevCoord)))
                {
                    goto flowUnknownLabel;
                }

                g = Vector2d<FloatType>(prevLevelFlow->element(prevCoord));
                g *= 2.0;
            }

            status = kltIteration(context, currCoord, &g, nullThreshold * nullThresholdFactor);
            if (!status)
                goto flowUnknownLabel;


            flow->element(i,j) = FlowElement(fround(g.x()), fround(g.y()));
            continue;
flowUnknownLabel:
            flow->setElementUnknown(i,j);
        }
    }

    delete gradient;
    return flow;
}

template<typename InterpolationType, typename FloatType>
FlowBuffer *KLTGenerator<InterpolationType, FloatType>::calculateHierarchicalKLTFlow(G12Buffer *first, G12Buffer *second)
{
    CORE_ASSERT_TRUE(first != NULL, "Arguments should not be null");
    CORE_ASSERT_TRUE(second != NULL, "Arguments should not be null");

    FlowBuffer *prevFlow = NULL;
    FlowBuffer *currentFlow = NULL;
    FlowBuffer *tempFlow = NULL;

    AbstractMipmapPyramid<G12Buffer> *pyramid1 = new AbstractMipmapPyramid<G12Buffer> (first, maxLevels);
    AbstractMipmapPyramid<G12Buffer> *pyramid2 = new AbstractMipmapPyramid<G12Buffer> (second, maxLevels);

    //prevFlow initialization
    prevFlow = calculateFlowFromPrevLevelBuffer(
                pyramid1->levels[maxLevels - 1],
            pyramid2->levels[maxLevels - 1],
            NULL,
            true);

    printf("Processing level %d\n", maxLevels - 1);

    if (maxLevels == 1)
        return prevFlow;

    for(int l = maxLevels - 2;l >= 0;l--)
    {
        tempFlow = prevFlow;

        currentFlow = calculateFlowFromPrevLevelBuffer(
                    pyramid1->levels[l],
                    pyramid2->levels[l],
                    prevFlow,
                    false);

        if (l != 0)
            prevFlow = currentFlow;

        printf("Processing level %d\n", l);
        delete tempFlow;
    }

    delete pyramid1;
    delete pyramid2;

    return currentFlow;

}

template<typename InterpolationType, typename FloatType>
bool KLTGenerator<InterpolationType, FloatType>::kltIteration(const KLTCalculationContext &calculationContext, const Vector2d32 &point, Vector2d<FloatType> *startGuess, double nullThreshold) const
{

    CORE_ASSERT_TRUE(calculationContext.first != NULL, "NULL parameter fisrt");
    CORE_ASSERT_TRUE(calculationContext.second != NULL, "NULL parameter second");
    CORE_ASSERT_TRUE(calculationContext.gradient != NULL, "NULL parameter gradient");
    G12Buffer *first  = calculationContext.first;
    G12Buffer *second = calculationContext.second;

    uint32_t h = first->h;
    uint32_t w = first->w;

    Vector2d<FloatType> currentGuess = *startGuess;

    /* We use iterative method and stop if number of iterations is above the threshold*/
    for(int l = 0;l < newtonIterations; l++)
    {
        DOTRACE(("Starting iteration %d\n", l));
        DOTRACE(("\tEntering guess: [%lf %lf]\n", currentGuess.x(), currentGuess.y()));

        /* The integer position in the other image */
        Vector2d32 prediction( fround(point.x() + currentGuess.x()),
                               fround(point.y() + currentGuess.y()));

        DOTRACE(("\tPreditiction (%d %d)\n", prediction.x(), prediction.y()));


        /* If we hit outside the second image we should give up*/
        /*if (! second->isValidCoord(prediction - windowSize - Vector2d32(2,2)) ||
                ! second->isValidCoord(prediction + windowSize + Vector2d32(2,2)))
                return false;*/

        Vector2d32 low  = point - windowSize;
        Vector2d32 high = point + windowSize;
        low.mapToRect(Vector2d32(0,0), Vector2d32(w - 1, h - 1));
        high.mapToRect(Vector2d32(0,0), Vector2d32(w - 1, h - 1));

        int x0 =  low.x();
        int y0 =  low.y();
        int x1 = high.x();
        int y1 = high.y();

        DOTRACE(("\tWorking zone (%d %d)x(%d %d)\n", x0, y0, x1, y1));

        //        Vector3dd mSG = calculationContext.gradient->rectangle(x0,y0,x1,y1);
        //        DOTRACE(("\tG matrix:\n"));
        //        DOTRACE(("\t[%lf %lf]\n", mSG.x(), mSG.y()));
        //        DOTRACE(("\t[%lf %lf]\n", mSG.y(), mSG.z()));


        /**
             *  Solve the system
             *  Calculating the inverted matrix. Matrix is symmetrical thus not using standard function
             *
             * */
        //        double det = mSG.x() * mSG.z() - mSG.y() * mSG.y();

        //        if (fabs(det) < nullThreshold)
        //            return false;

        //        double inv11 =  mSG.z() / det;
        //        double inv12 = -mSG.y() / det;
        //        double inv22 =  mSG.x() / det;

        //calculating mismatch vector
        Vector2d<FloatType> b = Vector2d<FloatType>(0.0);
        DOTRACE(("\n"));
        FloatType sum11 = 0;
        FloatType sum12 = 0;
        FloatType sum22 = 0;
        InterpolationType interpolator;
        for (int i = y0; i <= y1; i++)
        {
            for (int j = x0; j <= x1; j++)
            {
                FloatType targetX = j + currentGuess.x();
                FloatType targetY = i + currentGuess.y();

                if (!second->isValidCoordBl(targetY, targetX))
                    return false;

                FloatType dI = (FloatType)first ->element  (i      , j      ) -
                        interpolator.interpolate(targetY, targetX, second);

                FloatType Ix = (FloatType)(
                            first->element(i, j + 1) -
                            first->element(i, j - 1)
                            ) / 2.0;
                FloatType Iy = (FloatType)(
                            first->element(i + 1, j) -
                            first->element(i - 1, j)
                            ) / 2.0;
                DOTRACE(("%d %d ( %lf %lf %lf) ", i, j, dI, Ix, Iy));

                sum11 += Ix * Ix;
                sum22 += Iy * Iy;
                sum12 += Iy * Ix;
                b += Vector2d<FloatType>(Ix, Iy) * dI;
            }
        }
        FloatType det = sum11 * sum22 - sum12 * sum12;

        if (fabs(det) < nullThreshold)
            return false;

        FloatType inv11 =  sum22 / det;
        FloatType inv12 = -sum12 / det;
        FloatType inv22 =  sum11 / det;

        DOTRACE(("\n"));
        DOTRACE(("\tB vector (%lf %lf)\n", b.x(), b.y()));
        Vector2d<FloatType> t = Vector2d<FloatType>( inv11 * b.x() + inv12 * b.y(), inv12 * b.x() + inv22 * b.y());
        DOTRACE(("\tT vector (%lf %lf)\n", t.x(), t.y()));

        currentGuess += t;
    }

    DOTRACE(("Correcton %lf\n", !((*startGuess) - currentGuess)));
    *startGuess = currentGuess;
    return true;

}

template<typename InterpolationType, typename FloatType>
bool KLTGenerator<InterpolationType, FloatType>::kltIterationSubpixel(const KLTCalculationContext &calculationContext, const Vector2d<FloatType> &point, Vector2d<FloatType> *startGuess, float nullThreshold) const
{
    CORE_ASSERT_TRUE(calculationContext.first != NULL, "NULL parameter fisrt");
    CORE_ASSERT_TRUE(calculationContext.second != NULL, "NULL parameter second");
    CORE_ASSERT_TRUE(calculationContext.gradient != NULL, "NULL parameter gradient");
    G12Buffer *first  = calculationContext.first;
    G12Buffer *second = calculationContext.second;

    Vector2d<FloatType> currentGuess = *startGuess;
    InterpolationType interpolator;

    /* We use iterative method and stop if number of iterations is above the threshold*/
    for (int l = 0; l < newtonIterations; l++)
    {
        if (trace) SYNC_PRINT(("Starting iteration %d\n", l));
        if (trace) SYNC_PRINT(("\tEntering point: [%lf %lf]\n", point.x(), point.y()));
        if (trace) SYNC_PRINT(("\tEntering guess: [%lf %lf]\n", currentGuess.x(), currentGuess.y()));
        /* The position in the other image */
        Vector2d<FloatType> prediction = point + currentGuess;
        if (trace) SYNC_PRINT(("\tPreditiction (%lf %lf)\n", prediction.x(), prediction.y()));


        /* If we hit outside the second image we should give up*/
        if (!calculationContext.second->isValidCoord(Vector2d32(prediction.x(), prediction.y())))
            return false;

        //calculating mismatch vector
        Vector3d<FloatType> mSG = Vector3d<FloatType>(0.0);
        Vector2d<FloatType> b   = Vector2d<FloatType>(0.0);


        int window_y_min_border_firstImg = windowSize.y() + CORE_MIN(0, point.y()- windowSize.y());
        int window_y_max_border_firstImg = windowSize.y() - CORE_MAX(0, ( point.y()+ windowSize.y() ) -first->h -1 );
        int window_x_min_border_firstImg = windowSize.x() + CORE_MIN(0, point.x()- windowSize.x());
        int window_x_max_border_firstImg = windowSize.x() - CORE_MAX(0, ( point.x()+ windowSize.x() ) -first->w -1 );

        int window_y_min_border_secondImg = windowSize.y() + CORE_MIN(0, point.y() + currentGuess.y()- windowSize.y());
        int window_y_max_border_secondImg = windowSize.y() - CORE_MAX(0, ( point.y() + currentGuess.y() + windowSize.y() ) -first->h -1 );
        int window_x_min_border_secondImg = windowSize.x() + CORE_MIN(0, point.x() + currentGuess.x() - windowSize.x());
        int window_x_max_border_secondImg = windowSize.x() - CORE_MAX(0, ( point.x() + currentGuess.x() + windowSize.x() ) -first->w -1 );

        int window_y_min_border = CORE_MIN(window_y_min_border_firstImg, window_y_min_border_secondImg);
        int window_y_max_border = CORE_MIN(window_y_max_border_firstImg, window_y_max_border_secondImg);
        int window_x_min_border = CORE_MIN(window_x_min_border_firstImg, window_x_min_border_secondImg);
        int window_x_max_border = CORE_MIN(window_x_max_border_firstImg, window_x_max_border_secondImg);


        for (int i = -window_y_min_border; i <= window_y_max_border; i++)
        {
            for (int j = -window_x_min_border; j <= window_x_max_border; j++)
            {
                FloatType x =  point.x() + (FloatType)j;
                FloatType y =  point.y() + (FloatType)i;

                //                    if (!first->isValidCoordBl(y, x)) {
                //                        continue;
                //                    }

                //                    if (!second->isValidCoordBl(y + currentGuess.y(), x + currentGuess.x())) {
                //                        continue;
                //                    }

                FloatType dI = interpolator.interpolate(y, x, first) -
                        interpolator.interpolate(y + currentGuess.y(), x + currentGuess.x(), second);

                FloatType Ix = (FloatType)(
                            interpolator.interpolate(y      ,  x + 1.0, first) -
                            interpolator.interpolate(y      , x - 1.0, first))/ 2.0;

                FloatType Iy = (FloatType)(
                            interpolator.interpolate(y + 1.0, x, first) -
                            interpolator.interpolate(y - 1.0, x, first))/2.0;

                if (trace) SYNC_PRINT(("%d %d ( %lf %lf %lf)\n", i, j, dI, Ix, Iy));

                mSG += Vector3d<FloatType>(Ix * Ix, Ix * Iy, Iy * Iy);
                b   += Vector2d<FloatType>(Ix, Iy) * dI;
            }
        }

        FloatType det = mSG.x() * mSG.z() - mSG.y() * mSG.y();

        if (fabs(det) < nullThreshold)
            return false;

        FloatType inv11 =  mSG.z() / det;
        FloatType inv12 = -mSG.y() / det;
        FloatType inv22 =  mSG.x() / det;

        if (trace) SYNC_PRINT(("\n"));
        if (trace) SYNC_PRINT(("G  (%lf %lf)\n", mSG.x(), mSG.y()));
        if (trace) SYNC_PRINT(("   (%lf %lf)\n", mSG.y(), mSG.z()));
        if (trace) SYNC_PRINT(("\tB vector (%lf %lf)\n", b.x(), b.y()));
        Vector2d<FloatType> t = Vector2d<FloatType>( inv11 * b.x() + inv12 * b.y(), inv12 * b.x() + inv22 * b.y());
        if (trace) SYNC_PRINT(("\tT vector (%lf %lf)\n", t.x(), t.y()));

        currentGuess += t;
    }

    if (trace) SYNC_PRINT(("Correcton %lf\n", !((*startGuess) - currentGuess)));
    *startGuess = currentGuess;
    return true;
}



template<typename InterpolationType, typename FloatType>
bool KLTGenerator<InterpolationType, FloatType>::kltIterationSubpixelFast(
        const KLTCalculationContext &calculationContext,
        const Vector2df &pointf,
        Vector2df *startGuess,
        float nullThreshold) const
{
    CORE_ASSERT_TRUE(calculationContext.isFilled(), "CalculationContext is not Filled");

    G12Buffer *first  = calculationContext.first;
    G12Buffer *second = calculationContext.second;

    Vector2df currentGuess = *startGuess;

    Rectangle32 bufferRect = Rectangle32::FromSize(first->getSize() - Vector2d32(-1,-1));


    Vector2d32 point(fround(pointf.x()), fround(pointf.y()));
    Rectangle32 rect = Rectangle32::SquareFromCenter(point, windowSize);
    if (trace) SYNC_PRINT(("\tEntering point: [%lf %lf]\n", pointf.x(), pointf.y()));

    if (!bufferRect.contains(rect))
        return false;

    Vector3dd mSG = calculationContext.gradient->rectangle(rect);

    FloatType det = mSG.x() * mSG.z() - mSG.y() * mSG.y();

    if (fabs(det) < nullThreshold)
        return false;

    FloatType inv11 =  mSG.z() / det;
    FloatType inv12 = -mSG.y() / det;
    FloatType inv22 =  mSG.x() / det;

    /* We use iterative method and stop if number of iterations is above the threshold*/
    for (int l = 0; l < newtonIterations; l++)
    {
        if (trace) SYNC_PRINT(("Starting iteration %d\n", l));
        /* The position in the other image */
        Vector2df prediction = pointf + currentGuess;
        if (trace) SYNC_PRINT(("\tPrediction (%lf %lf)\n", prediction.x(), prediction.y()));


        Rectanglef trect = Rectanglef::SquareFromCenter(prediction, Vector2df(windowSize.x(), windowSize.y()));
        if (!bufferRect.contains(trect)) {
            continue;
        }

        Vector2df b = Vector2df::Zero();

        float dx = currentGuess.x() - floor(currentGuess.x());
        float dy = currentGuess.y() - floor(currentGuess.y());

        float odx = 1 - dx;
        float ody = 1 - dy;

        int cx = prediction.x();
        int cy = prediction.y();

        for (int i = -windowSize.y(); i <= windowSize.y(); i++)
        {
            for (int j = -windowSize.x(); j <= windowSize.x(); j++)
            {
                int x1 = cx + j;
                int y1 = cy + i;

                float value =
                    (second->element(y1,     x1) * odx + second->element(y1    , x1 + 1) * dx) * ody +
                    (second->element(y1 + 1, x1) * odx + second->element(y1 + 1, x1 + 1) * dx) * dy;


                int x0 = point.x() + j;
                int y0 = point.y() + i;

                float dI = first->element(y0, x0) - value;

                int Iy  = (first->element(y0 + 1, x0    ) - first->element(y0 - 1, x0    )) / 2.0;
                int Ix  = (first->element(y0    , x0 + 1) - first->element(y0    , x0 - 1)) / 2.0;

                if (trace) SYNC_PRINT(("%d %d ( %f %d %d)\n", i, j, dI, Ix, Iy));

                b   += Vector2df(Ix, Iy) * dI;
            }
        }

        if (trace) SYNC_PRINT(("\n"));
        if (trace) SYNC_PRINT(("G  (%lf %lf)\n", mSG.x(), mSG.y()));
        if (trace) SYNC_PRINT(("   (%lf %lf)\n", mSG.y(), mSG.z()));
        if (trace) SYNC_PRINT(("\tB vector (%lf %lf)\n", b.x(), b.y()));
        Vector2df t = Vector2df( inv11 * b.x() + inv12 * b.y(), inv12 * b.x() + inv22 * b.y());
        if (trace) SYNC_PRINT(("\tT vector (%lf %lf)\n", t.x(), t.y()));

        currentGuess += t;
    }

    if (trace) SYNC_PRINT(("Correcton %lf\n", !((*startGuess) - currentGuess)));
    *startGuess = currentGuess;
    return true;
}


/**
 * This function does the classical KLT step
 *
 *
 *  \f{eqnarray*}
 * G &=&
 *         \sum_{ x = p_x - w_x}^{p_x + w_x} \sum_{y = p_y - w_y}^{p_y + w_y}
 *         \pmatrix{
 *             I_x^2   &  I_x I_y \cr
 *             I_x I_y &  I_y^2   \cr
 *         } \\
 *
 * b &=&
 *         \sum_{ x = p_x - w_x}^{p_x + w_x} \sum_{y = p_y - w_y}^{p_y + w_y}
 *         \pmatrix{
 *             \delta I I_x \cr
 *          \delta I I_y \cr
 *         } \\
 *
 *  v_{opt} &=& G^{-1}b
 *
 * \f}
 *
 * */


template class KLTGenerator<BilinearInterpolator , float>;
template class KLTGenerator<BilinearInterpolatorD, double>;

template class KLTGenerator<Splain3Interpolator, double>;
template class KLTGenerator<PolynomInterpolator, double>;

template class KLTGenerator<Splain3Interpolator, float>;
template class KLTGenerator<PolynomInterpolator, float>;


} //namespace corecvs

