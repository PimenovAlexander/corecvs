#ifndef ELLIPSEFIT_H
#define ELLIPSEFIT_H

#include "core/geometry/ellipse.h"


namespace corecvs {

/**
 *  http://cseweb.ucsd.edu/~mdailey/Face-Coord/ellipse-specific-fitting.pdf
 *
 **/
class EllipseFit
{
public:
    EllipseFit();
    vector<Vector2dd> points;

    void addPoint    (const Vector2dd &point);
    void setPointList(const vector<Vector2dd> &pointList);

    /** Simplest way that involves only in'da'house methods and uses ellipse resctriction **/
    SecondOrderCurve solveDummy();
#ifdef WITH_BLAS
    SecondOrderCurve solveBLAS();
#endif

    /*
    void operator ()() {
#ifdef WITH_BLAS
    solveBLAS();
#else
    solveDummy();
#endif
    }
    */

    /** Generic Quadric without ellipse restrictions **/
    SecondOrderCurve solveQuadricDummy();


};

} // namespace corecvs

#endif // ELLIPSEFIT_H
