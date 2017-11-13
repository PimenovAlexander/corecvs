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
#include "core/buffers/mipmapPyramid.h"
#include "core/buffers/integralBuffer.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/flow/flowBuffer.h"
#include "core/math/vector/vector3d.h"
#include "core/math/vector/vector2d.h"
#include "core/buffers/interpolator.h"
#include "core/buffers/kernels/spatialGradient.h"
#include "core/utils/global.h"
#include "core/math/mathUtils.h"
#include "core/buffers/mipmapPyramid.h"

#ifdef _MSC_VER
# pragma warning(disable: 4101)  // 'interpolator' : unreferenced local variable
#endif

namespace corecvs {

class KLTCalculationContext
{
public:
    G12Buffer *first;
    G12Buffer *second;
    SpatialGradientIntegralBuffer *gradient;
};

template <typename InterpolationType>
class KLTGenerator
{
public:
    Vector2d32 windowSize;
    int newtonIterations;
    double *maxThresholds;
    double *minThresholds;
    int maxLevels;


    KLTGenerator():
        windowSize(Vector2d32(5,5)),
        newtonIterations(7),
        maxLevels(5)
    {}
    KLTGenerator(Vector2d32 _windowSize, int _newtonIterations = 7, int _maxLevels = 5) :
        windowSize(_windowSize),
        newtonIterations(_newtonIterations),
        maxLevels(_maxLevels)
    {}

    FlowBuffer *calculateFlowFromPrevLevelBuffer(
            G12Buffer *first,
            G12Buffer *second,
            FlowBuffer *prevLevelFlow,
            bool isHighestFlowBuffer)
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

        Vector2dd g;

        for(i = 0; i < h; i++)
        {
            for(j = 0; j < w; j++)
            {
                Vector2d32 currCoord = Vector2d32(j, i);
                Vector2d32 prevCoord = currCoord / 2;
                bool status;

                if (isHighestFlowBuffer)
                {
                    g = Vector2dd(0.0);
                }
                else
                {
                    if (!(prevLevelFlow->isElementKnownVec(prevCoord)))
                    {
                        goto flowUnknownLabel;
                    }

                    g = Vector2dd(prevLevelFlow->element(prevCoord));
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


    FlowBuffer *calculateHierarchicalKLTFlow(
            G12Buffer *first,
            G12Buffer *second
            )
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

    bool kltIteration (
            const KLTCalculationContext &calculationContext,
            const Vector2d32 &point,
            Vector2dd *startGuess,
            double nullThreshold) const
    {

        CORE_ASSERT_TRUE(calculationContext.first != NULL, "NULL parameter fisrt");
        CORE_ASSERT_TRUE(calculationContext.second != NULL, "NULL parameter second");
        CORE_ASSERT_TRUE(calculationContext.gradient != NULL, "NULL parameter gradient");
        G12Buffer *first  = calculationContext.first;
        G12Buffer *second = calculationContext.second;

        uint32_t h = first->h;
        uint32_t w = first->w;

        Vector2dd currentGuess = *startGuess;

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
            Vector2dd b = Vector2dd(0.0);
            DOTRACE(("\n"));
            double sum11 = 0;
            double sum12 = 0;
            double sum22 = 0;
            InterpolationType interpolator;
            for (int i = y0; i <= y1; i++)
            {
                for (int j = x0; j <= x1; j++)
                {
                    double targetX = j + currentGuess.x();
                    double targetY = i + currentGuess.y();

                    if (!second->isValidCoordBl(targetY, targetX))
                        return false;

                    double dI = (double)first ->element  (i      , j      ) -
                                        interpolator.interpolate(targetY, targetX, second);

                    double Ix = (double)(
                            first->element(i, j + 1) -
                            first->element(i, j - 1)
                            ) / 2.0;
                    double Iy = (double)(
                            first->element(i + 1, j) -
                            first->element(i - 1, j)
                            ) / 2.0;
                    DOTRACE(("%d %d ( %lf %lf %lf) ", i, j, dI, Ix, Iy));

                    sum11 += Ix * Ix;
                    sum22 += Iy * Iy;
                    sum12 += Iy * Ix;
                    b += Vector2dd(Ix, Iy) * dI;
                }
            }
            double det = sum11 * sum22 - sum12 * sum12;

            if (fabs(det) < nullThreshold)
                return false;

            double inv11 =  sum22 / det;
            double inv12 = -sum12 / det;
            double inv22 =  sum11 / det;

            DOTRACE(("\n"));
            DOTRACE(("\tB vector (%lf %lf)\n", b.x(), b.y()));
            Vector2dd t = Vector2dd( inv11 * b.x() + inv12 * b.y(), inv12 * b.x() + inv22 * b.y());
            DOTRACE(("\tT vector (%lf %lf)\n", t.x(), t.y()));

            currentGuess += t;
         }

        DOTRACE(("Correcton %lf\n", !((*startGuess) - currentGuess)));
        *startGuess = currentGuess;
        return true;

    };

    bool kltIterationSubpixel (
            const KLTCalculationContext &calculationContext,
            const Vector2dd &point,
            Vector2dd *startGuess,
            double nullThreshold) const
    {
        CORE_ASSERT_TRUE(calculationContext.first != NULL, "NULL parameter fisrt");
        CORE_ASSERT_TRUE(calculationContext.second != NULL, "NULL parameter second");
        CORE_ASSERT_TRUE(calculationContext.gradient != NULL, "NULL parameter gradient");
        G12Buffer *first  = calculationContext.first;
        G12Buffer *second = calculationContext.second;

    //    uint32_t h = first->h;
    //    uint32_t w = first->w;

        Vector2dd currentGuess = *startGuess;

        /* We use iterative method and stop if number of iterations is above the threshold*/
        for (int l = 0; l < newtonIterations; l++)
        {
            DOTRACE(("Starting iteration %d\n", l));
            DOTRACE(("\tEntering point: [%lf %lf]\n", point.x(), point.y()));
            DOTRACE(("\tEntering guess: [%lf %lf]\n", currentGuess.x(), currentGuess.y()));
            /* The position in the other image */
            Vector2dd prediction = point + currentGuess;
            DOTRACE(("\tPreditiction (%lf %lf)\n", prediction.x(), prediction.y()));


            /* If we hit outside the second image we should give up*/
            if (!calculationContext.second->isValidCoord(Vector2d32(prediction)))
                return false;

            //Vector2dd low  = point - Vector2dd(windowSize);
            //Vector2dd high = point + Vector2dd(windowSize);
            //low.mapToRect(Vector2dd(0,0), Vector2dd(w - 1, h - 1));
            //high.mapToRect(Vector2dd(0,0), Vector2dd(w - 1, h - 1));

            //int x0 =  low.x();
            //int y0 =  low.y();
            //int x1 = high.x();
            //int y1 = high.y();

            //DOTRACE(("\tWorking zone (%d %d)x(%d %d)\n", x0, y0, x1, y0));

            /*
            Vector3dd mSG = calculationContext.gradient->rectangle(y0,x0,y1,x1);
            DOTRACE(("\tG matrix:\n"));
            DOTRACE(("\t[%lf %lf]\n", mSG.x(), mSG.y()));
            DOTRACE(("\t[%lf %lf]\n", mSG.y(), mSG.z()));
    */

            //calculating mismatch vector
            Vector2dd b = Vector2dd(0.0);
            Vector3dd mSG = Vector3dd(0.0);

            DOTRACE(("\n"));

            InterpolationType interpolator;
            for (int i = -windowSize.y(); i <= windowSize.y(); i++)
            {
                for (int j = -windowSize.x(); j <= windowSize.x(); j++)
                {
                    double x =  point.x() + (double)j;
                    double y =  point.y() + (double)i;

                    double dI = interpolator.interpolate(y, x, first) -
                                interpolator.interpolate(y + currentGuess.y(), x + currentGuess.x(), second);

                    double Ix = (double)(
                            interpolator.interpolate(y      , CORE_MIN(first->w - 1.0, x + 1.0), first) -
                            interpolator.interpolate(y      , CORE_MAX(0.0, x - 1.0), first)
                                ) / (CORE_MIN(first->w - 1.0, x + 1.0) - CORE_MAX(0.0, x - 1.0));

                    double Iy = (double)(
                                interpolator.interpolate(CORE_MIN(first->h - 1.0, y + 1.0), x, first) -
                            interpolator.interpolate(CORE_MAX(0.0, y - 1.0), x, first)
                                ) / (CORE_MIN(first->h - 1.0, y) - CORE_MAX(0.0, y - 1.0));
                    DOTRACE(("%d %d ( %lf %lf %lf)\n", i, j, dI, Ix, Iy));

                    mSG += Vector3dd(Ix * Ix, Ix * Iy, Iy * Iy);
                    b += Vector2dd(Ix, Iy) * dI;
                }
            }

            double det = mSG.x() * mSG.z() - mSG.y() * mSG.y();

            if (fabs(det) < nullThreshold)
                return false;

            double inv11 =  mSG.z() / det;
            double inv12 = -mSG.y() / det;
            double inv22 =  mSG.x() / det;

            DOTRACE(("\n"));
            DOTRACE(("\tB vector (%lf %lf)\n", b.x(), b.y()));
            Vector2dd t = Vector2dd( inv11 * b.x() + inv12 * b.y(), inv12 * b.x() + inv22 * b.y());
            DOTRACE(("\tT vector (%lf %lf)\n", t.x(), t.y()));

            currentGuess += t;
         }

        DOTRACE(("Correcton %lf\n", !((*startGuess) - currentGuess)));
        *startGuess = currentGuess;
        return true;
    }
    virtual ~KLTGenerator(){}
};

} //namespace corecvs

#ifdef _MSC_VER
# pragma warning(default: 4101)  // 'interpolator' : unreferenced local variable
#endif

/* EOF */
