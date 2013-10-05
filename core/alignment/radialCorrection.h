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

#include "global.h"
#include "vector2d.h"
#include "g12Buffer.h"
#include "function.h"

namespace corecvs {

/**
 * \ingroup distcorrect
 * \brief This structure holds the parameters to correct the image.
 *
 * The intrinsic correction parameters form
 * http://www.vision.caltech.edu/bouguetj/calib_doc/papers/heikkila97.pdf
 *
 * Here are the correction formulas
 *  \f{eqnarray*}
 *  \pmatrix{dx \cr dy} &=& \pmatrix{x \cr y} - \pmatrix{x_c \cr y_c} \\
 *                    r &=& \sqrt{dx^2 + dy^2} \\
 *                 r_{corr} &=& k_1 r^2 + k_2 r^4 \\
 *           \hat{r}_{corr} &=& \sum_{i=1}^{n} k_i r^i \\
 *
 *   \pmatrix{x_{coor}^t \cr y_{coor}^t} &=&
 *   \pmatrix{p_1 dx dy + p_2 (r^2 + 2 dx^2)\cr p_1 (r^2 + 2 dy^2) + p_2 dx dy} \\
 *
 *
 *   \pmatrix{x \cr y} &=&
 *   \pmatrix{x_0 \cr y_0} +
 *   \pmatrix{dx \cr dy} * r_{corr} +
 *   \pmatrix{x_{coor}^t \cr y_{coor}^t}
 *
 *   \f}
 *
 *   For Marquardt-Levenberg algorithm we will need derivatives of the function
 *
 *   for more details please read the code of getCorrectionForPoint() or read the Heikkila paper
 *
 */
class LensCorrectionParametres {
public:
//    double k1;          /**< Second order radial correction coefficient - \f$k_1\f$*/
//    double k2;          /**< Fourth order radial correction coefficient - \f$k_2\f$*/
    vector<double> koeff; /**< Polynom to describe radial correction */

    double p1;       /**< First tangent correction coefficient - \f$p_1\f$*/
    double p2;       /**< Second tangent correction coefficient - \f$p_2\f$*/

    double aspect;    /**< y/x matrix ratio */
    double focal;     /**< focal */

    Vector2dd center; /**< The center of the distortion \f$(x_c,y_c)\f$*/

    LensCorrectionParametres(
            vector<double> _koeff,
            double _p1,
            double _p2,
            double _aspect,
            double _focal,
            Vector2dd _center) :
        p1(_p1),
        p2(_p2),
        aspect(_aspect),
        focal(_focal),
        center(_center)
    {
        koeff = _koeff;
        /*Temporary solution*/
        focal = center.l2Metric();
    }

    LensCorrectionParametres(vector<double> _koeff, double _p1, double _p2, Vector2d32 _center) :
        p1(_p1),
        p2(_p2),
        aspect(1.0),
        center(_center)
    {
        koeff = _koeff;
    }

    LensCorrectionParametres() :
        p1(0.0),
        p2(0.0),
        aspect(1.0),
        center(Vector2dd(320,240))
    {
    }


};


class RadialCorrection : public DeformMap<int32_t, double>, public FunctionArgs
{
public:
    explicit RadialCorrection(const LensCorrectionParametres &params = LensCorrectionParametres());

    virtual ~RadialCorrection();
    inline Vector2dd map(Vector2dd const & v) const
    {
        return map(v.y(), v.x());
    }

    double radialScale(double r) const
    {
        double rpow = r;
        double radialCorrection = 0;

        for (unsigned i = 0; i < mParams.koeff.size(); i ++)
        {
            radialCorrection += mParams.koeff[i] * rpow;
            rpow *= r;
        }
        return radialCorrection;
    }

    inline Vector2dd map(int y, int x) const
    {
        return map((double)y, (double)x);
    }

    /**
     * \brief Apply the lens distortion correction.
     *
     * For details see LensCorrectionParametres
     *
     * \param y  y position of the point to be mapped
     * \param x  x position of the point to be mapped
     * \return A corrected buffer
     *
     **/
    inline Vector2dd map(double y, double x) const
    {
        double dpx = (x - mParams.center.x()) * mParams.aspect;
        double dpy = (y - mParams.center.y());

        double dx = dpx / mParams.focal;
        double dy = dpy / mParams.focal;

        double dxsq = dx * dx;
        double dysq = dy * dy;
        double dxdy = dx * dy;

        double rsq = dxsq + dysq;
        double r = sqrt(rsq);

        double radialCorrection = radialScale(r);
//        SYNC_PRINT(("RadialCorrection::map (): [%lf %lf ] %lf %lf\n", x, y, rsq, radialCorrection));


        double radialX = (double)dx * radialCorrection;
        double radialY = (double)dy * radialCorrection;

        double tangentX = 2 * mParams.p1 * dxdy + mParams.p2 * ( rsq + 2 * dxsq );
        double tangentY = mParams.p1 * (rsq + 2 * dysq) + 2 * mParams.p2 * dxdy;

        return Vector2dd(
                x + ((radialX + tangentX) / mParams.aspect * mParams.focal),
                y + ((radialY + tangentY)                  * mParams.focal));
    }

    /**/
    virtual void operator()(const double in[], double out[])
    {
        Vector2dd result = map(in[1], in[0]);
        out[1] = result.y();
        out[0] = result.x();
    }


    //G12Buffer *correctLens(G12Buffer *input);
    G12Buffer *doCorrectionTransform(G12Buffer *inputBuffer);

    //Vector2dd getCorrectionForPoint(Vector2dd input);
    LensCorrectionParametres mParams;
};


} //namespace corecvs
#endif /* RADIALCORRECTION_H_ */

