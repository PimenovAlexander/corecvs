/**
 * \file radialCorrection.h
 * \brief Header file for the radial lens distortion correction primitives
 *
 * \ingroup cppcorefiles
 *
 * \date Jun 22, 2010
 * \author alexander
 */

#ifndef RADIALCORRECTION_H_
#define RADIALCORRECTION_H_

#include "core/geometry/ellipticalApproximation.h"
#include "core/alignment/lensDistortionModelParameters.h"
#include "core/utils/global.h"
#include "core/math/vector/vector2d.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/function/function.h"
#include "core/buffers/deformMap.h"

namespace corecvs {


class RadialCorrection : public DeformMap<int32_t, double>, public FunctionArgs
{
public:
    explicit RadialCorrection(const LensDistortionModelParameters &params = LensDistortionModelParameters());

    virtual ~RadialCorrection();

    inline Vector2dd map(Vector2dd const & v) const
    {
        return map(v.y(), v.x());
    }

    inline double radialScaleNormalized(double r) const
    {
        double rpow = r;
        double radialCorrection = 0;
        const vector<double>& coeffs = mParams.koeff();

        for (unsigned i = 0; i < coeffs.size(); i++)
        {
            radialCorrection += coeffs[i] * rpow;
            rpow *= r;
        }
        return radialCorrection;
    }

    inline double radialScale(double r) const
    {
        double normalizedR = r / mParams.normalizingFocal();
        return radialScaleNormalized(normalizedR);
    }


    inline Vector2dd map(int y, int x) const
    {
        return map((double)y, (double)x);
    }

    /**
     * \brief Apply the lens distortion correction.
     *
     * For details see LensDistortionModelParameters
     *
     * \param y  y position of the point to be mapped
     * \param x  x position of the point to be mapped
     * \return A corrected buffer
     *
     **/
    inline Vector2dd map(double y, double x) const
    {
        double cx = mParams.principalX();
        double cy = mParams.principalY();
        double p1 = mParams.tangentialX();
        double p2 = mParams.tangentialY();

        double dx = (x - cx) / mParams.normalizingFocal() * mParams.aspect();
        double dy = (y - cy) / mParams.normalizingFocal();

        /*double dx = dpx / mParams.focal;
        double dy = dpy / mParams.focal;*/

        double dxsq = dx * dx;
        double dysq = dy * dy;
        double dxdy = dx * dy;

        double rsq = dxsq + dysq;
        double r = sqrt(rsq);

        double radialCorrection = radialScaleNormalized(r);
//        SYNC_PRINT(("RadialCorrection::map (): [%lf %lf ] %lf %lf\n", x, y, rsq, radialCorrection));


        double radialX = (double)dx * radialCorrection;
        double radialY = (double)dy * radialCorrection;

        double tangentX =    2 * p1 * dxdy      + p2 * ( rsq + 2 * dxsq );
        double tangentY = p1 * (rsq + 2 * dysq) +     2 * p2 * dxdy      ;

        return Vector2dd(
                cx + ((dx + radialX + tangentX) / mParams.aspect() * mParams.scale() * mParams.normalizingFocal()),
                cy + ((dy + radialY + tangentY)                    * mParams.scale() * mParams.normalizingFocal())
               ) + Vector2dd(addShiftX, addShiftY);
    }

    /**/
    virtual void operator()(const double in[], double out[])
    {
        Vector2dd result = map(in[1], in[0]);
        out[1] = result.y();
        out[0] = result.x();
    }


    //G12Buffer *correctLens(G12Buffer *input);
    G12Buffer   *doCorrectionTransform(G12Buffer *inputBuffer);
    RGB24Buffer *doCorrectionTransform(RGB24Buffer *inputBuffer);

    //Vector2dd getCorrectionForPoint(Vector2dd input);
    LensDistortionModelParameters mParams;

    /* TODO: This was not in original design. Most probably should be done by decorator.*/
    double addShiftX;
    double addShiftY;



    Vector2dd center() const
    {
        return Vector2dd(mParams.principalX(), mParams.principalY());
    }


    RadialCorrection invertCorrection(int h, int w, int step);
    EllipticalApproximation1d compareWith(const RadialCorrection &other, int h, int w, int steps);



    /* */
    void getCircumscribedImageRect(const int32_t &x1, const int32_t &y1, const int32_t &x2, const int32_t &y2,
                                   Vector2dd &min, Vector2dd &max)
    {
        min = map(y1,x1);
        max = map(y2,x2);

        for (int i = y1; i <= y2; i++)
        {
            Vector2dd mapLeft  = map(i, x1);
            Vector2dd mapRight = map(i, x2);
            if (mapLeft .x() < min.x()) min.x() = mapLeft.x();
            if (mapRight.x() > max.x()) max.x() = mapRight.x();
        }

        for (int j = x1; j <= x2; j++)
        {

            Vector2dd mapTop    = map(y1, j);
            Vector2dd mapBottom = map(y2, j);
            if (mapTop   .y() < min.y()) min.y() = mapTop.y();
            if (mapBottom.y() > max.y()) max.y() = mapBottom.y();
        }
    }

    void getInscribedImageRect(const int32_t &x1, const int32_t &y1, const int32_t &x2, const int32_t &y2,
                               Vector2dd &min, Vector2dd &max)
    {
        min = map(y1,x1);
        max = map(y2,x2);

        for (int i = y1; i <= y2; i++)
        {
            Vector2dd mapLeft  = map(i, x1);
            Vector2dd mapRight = map(i, x2);
            if (mapLeft .x() > min.x()) min.x() = mapLeft.x();
            if (mapRight.x() < max.x()) max.x() = mapRight.x();
        }

        for (int j = x1; j <= x2; j++)
        {

            Vector2dd mapTop    = map(y1, j);
            Vector2dd mapBottom = map(y2, j);
            if (mapTop   .y() > min.y()) min.y() = mapTop.y();
            if (mapBottom.y() < max.y()) max.y() = mapBottom.y();
        }
    }
};


} //namespace corecvs
#endif /* RADIALCORRECTION_H_ */

