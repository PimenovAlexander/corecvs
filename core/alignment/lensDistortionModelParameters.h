#ifndef LENS_DISTORTION_MODEL_PARAMETERS_H_
#define LENS_DISTORTION_MODEL_PARAMETERS_H_
/**
 * \file lensDistortionModelParameters.h
 * \attention This file was automatically generated and now we modify it manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 */

#include "core/xml/generated/lensDistortionModelParametersBase.h"
#include "core/reflection/reflection.h"
#include "core/reflection/defaultSetter.h"
#include "core/reflection/printerVisitor.h"
#include "core/math/levenmarq.h"
#include "core/math/matrix/matrix22.h"
//#include "core/polynomial/polynomialSolver.h"     // including this dues to a compilation error with msvc2013/2015, see details at the polynomial.h

/*
 *  Embed includes.
 */
/*
 *  Additional includes for Composite Types.
 */

using namespace corecvs;

/*
 *  Additional includes for Pointer Types.
 */

namespace corecvs {

/*
 *  Additional includes for enum section.
 */

/**
 * \brief Lens Distortion Model Parameters
 *
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

 **/
class LensDistortionModelParameters : public LensDistortionModelParametersBase
{

public:

    Vector2dd principalPoint() const
    {
        return Vector2dd(principalX(), principalY());
    }

    void setPrincipalPoint(const Vector2dd &point )
    {
        setPrincipalX(point.x());
        setPrincipalY(point.y());
    }

    Vector2dd shift() const
    {
        return Vector2dd(shiftX(), shiftY());
    }

    void setShift(const Vector2dd &point )
    {
        setShiftX(point.x());
        setShiftY(point.y());
    }

public:

    /** Static fields init function, this is used for "dynamic" field initialization */
    static int staticInit(corecvs::Reflection *toFill);

    using LensDistortionModelParametersBase::mapForward;

    corecvs::Vector2dd mapForward(const corecvs::Vector2dd &v) const
    {
        return mMapForward ? map(v) : invMap(v);
    }

    corecvs::Vector2dd mapBackward(const corecvs::Vector2dd &v) const
    {
        return mMapForward ? invMap(v) : map(v);
    }

    corecvs::Vector2dd map(const corecvs::Vector2dd &v) const
    {
        double x = v[0];
        double y = v[1];
        if (mMapForward)
        {
            x -= mShiftX;
            y -= mShiftY;
            x /= mScale;
            y /= mScale;
        }
        double cx = mPrincipalX;
        double cy = mPrincipalY;
        double p1 = mTangentialX;
        double p2 = mTangentialY;

        double dx = (x - cx) / mNormalizingFocal * mAspect;
        double dy = (y - cy) / mNormalizingFocal;

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

        auto res = Vector2dd(
                ((dx + radialX + tangentX) / mAspect),
                ((dy + radialY + tangentY))
               ) * mNormalizingFocal;
        if (!mMapForward)
        {
            res *= mScale;
            res += Vector2dd(mShiftX, mShiftY);
        }
        return res + Vector2dd(cx, cy);
    }

    /** To satisfy deform map static interface **/
    corecvs::Vector2dd map(int &y, int &x) const
    {
        return map(Vector2dd(x,y));
    }


    // the body of these methods have been moved to the cpp file to don't require including "polynomialSolver.h"
    bool check(const double r);
    bool check(const Vector2dd &tl, const Vector2dd &br);

    // These functions always return jacobian/derivatives for "native" application
    Matrix22 jacobian(double x, double y) const;
    // du_i/dc_j
    Matrix22  principalJacobian(double x, double y) const;
    // du_i/dp_j
    Matrix22 tangentialJacobian(double x, double y) const;
    // du_i/da_j
    Matrix   polynomialJacobian(double x, double y) const;

    void solveRadial(const std::vector<Vector2dd> &src, const std::vector<Vector2dd> &dst);

    struct InverseFunctor : FunctionArgs
    {
        Matrix getJacobian(const double *in, double = 2) override
        {
            auto m = params->jacobian(in[0], in[1]);
            Matrix mm(2, 2);
            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 2; ++j)
                    mm.a(i, j) = m.a(i, j);
            return mm;
        }
        void operator() (const double* in, double *out)
        {
            Vector2dd x(in[0], in[1]);
            auto err = params->map(x) - target;
            out[0] = err[0];
            out[1] = err[1];
        }
        InverseFunctor(Vector2dd target, const LensDistortionModelParameters* params) : FunctionArgs(2, 2), target(target), params(params)
        {
        }
        Vector2dd target;
        const LensDistortionModelParameters* params;
    };
    corecvs::Vector2dd invMap(const corecvs::Vector2dd &v, const corecvs::Vector2dd &guess) const
    {
        InverseFunctor functor(v, this);
        LevenbergMarquardt lm(1000);
        lm.f = &functor;
        lm.traceProgress = false;
        std::vector<double> in = {guess[0], guess[1]}, out(2);
        auto res = lm.fit(in, out);
        Vector2dd resV(res[0], res[1]);
        return resV;
    }
    corecvs::Vector2dd invMap(const corecvs::Vector2dd &v) const
    {
        return invMap(v, v);
    }
    inline double radialScaleNormalized(double r) const
    {
        double rpow = r;
        double radialCorrection = 0;

        for (unsigned i = 0; i < mKoeff.size(); i++)
        {
            radialCorrection += mKoeff[i] * rpow;
            rpow *= r;
        }
        return radialCorrection;
    }
    inline double radialScale(double r) const
    {
        double normalizedR = r / mNormalizingFocal;
        return radialScaleNormalized(normalizedR);
    }

    void getInscribedImageRect    (const Vector2dd &tlDistorted, const Vector2dd &drDistorted, Vector2dd &tlUndistorted, Vector2dd &drUndistorted) const;
    void getCircumscribedImageRect(const Vector2dd &tlDistorted, const Vector2dd &drDistorted, Vector2dd &tlUndistorted, Vector2dd &drUndistorted) const;



    /* Section with embedded classes */
    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
       LensDistortionModelParametersBase::accept(visitor);
    }

    LensDistortionModelParameters()
    {
        DefaultSetter setter;
        accept(setter);
    }

    LensDistortionModelParameters(
          double principalX
        , double principalY
        , double tangentialX
        , double tangentialY
        , vector<double> koeff
        , double aspect
        , double scale
        , double normalizingFocal
        , double shiftX
        , double shiftY
        , bool   mapForward = false
    ) : LensDistortionModelParametersBase(
          principalX ,
          principalY ,
          tangentialX ,
          tangentialY ,
          koeff ,
          aspect ,
          scale ,
          normalizingFocal ,
          shiftX ,
          shiftY ,
          mapForward
       )
    {}

    friend ostream& operator << (ostream &out, LensDistortionModelParameters &toSave)
    {
        PrinterVisitor printer(out);
        toSave.accept<PrinterVisitor>(printer);
        return out;
    }

    void print ()
    {
        cout << *this;
    }

private:
    void getRectMap(const Vector2dd &tl, const Vector2dd &dr, std::vector<Vector2dd> boundaries[4]) const;

};

} // namespace

#endif  //LENS_DISTORTION_MODEL_PARAMETERS_H_
