#pragma once
#include "function.h"
#include "radialCorrection.h"

namespace corecvs {


class ModelToRadialCorrection {
public:
    RadialCorrection mLockedDimentions;
    bool mGuessCenter;
    bool mGuessTangent;
    int  mPolynomialDegree;
    bool mEvenDegreeOnly;


#if 0
    enum InputVector2Model {
         CENTER_X = 0, /*0*/
         CENTER_Y,     /*1*/
         TANGENT_P1,   /*2*/
         TANGENT_P2,   /*3*/
         FIRST_POLYNOM /*4*/
     };
#endif

    ModelToRadialCorrection(
            const RadialCorrection &lockedDimentions,
            bool guessCenter,
            bool guessTangent,
            int  polynomialDegree,
            bool evenDegreeOnly
    ) :
        mLockedDimentions(lockedDimentions),
        mGuessCenter(guessCenter),
        mGuessTangent(guessTangent),
        mPolynomialDegree(polynomialDegree),
        mEvenDegreeOnly(evenDegreeOnly)
    {}

    RadialCorrection getRadial(const double in[]) const;
    void getModel(const RadialCorrection &correction, double in[]) const;

    int getModelSize() const
    {
        int degrees = mPolynomialDegree;
        if (mEvenDegreeOnly) {
            // 0 - 0
            // 1 - 0
            // 2 - 1
            // 3 - 1
            degrees = mPolynomialDegree / 2;
        }

        return degrees +
               (mGuessCenter  ? 2 : 0) +
               (mGuessTangent ? 2 : 0);
    }

};

#define OUTDATED
#ifdef OUTDATED

/**
 * \ingroup distcorrect
 * \brief function to optimize
 *
 **/
class RadialFunc : public FunctionArgs
{
public:
    RadialFunc(const vector<Vector2dd> &undistortedPoints, const Vector2dd &center, int polynomDegree);
    virtual void operator()(const double in[], double out[]);

    virtual Matrix getJacobian(const double in[], double delta = 1e-15);
    void setScaleFactor(double scaleFactor);

    enum InputVector2Model {
//           CENTER_X = 0, /*0*/
//           CENTER_Y,     /*1*/
           TANGENT_P1,   /*2*/
           TANGENT_P2,   /*3*/
//           SCALE,
           FIRST_POLYNOM /*4*/
       };

private:
    const vector<Vector2dd> &mUndistortedPoints;
    Vector2dd mCenter;
    int mPolynomDegree;
    double mScaleFactor;
};
#endif

}
