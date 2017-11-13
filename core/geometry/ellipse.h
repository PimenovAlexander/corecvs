#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "core/math/vector/vector2d.h"
#include "core/polynomial/polynomial.h"
#include "core/math/affine.h"
#include "core/geometry/line.h"
#include "core/geometry/conic.h"

namespace corecvs {


class Ellipse;

/**
    Second Order Polinomial Curve (should we call this Quadric? )

    \f[ a_{11}x^{2}+a_{22}y^{2}+2a_{12}xy+2a_{13}x+2a_{23}y+a_{33}=0 \f]

    Internal form stores a_ij directly without coefficient 2

**/
class SecondOrderCurve  : public FixedVector<double, 6>
{
public:
    Matrix33 toMatrixForm() const;

    /** different convensions on field naming */
      double A() const      { return element[0]; }
      double B() const      { return 2 * element[1]; }
      double C() const      { return element[2]; }
      double D() const      { return 2 * element[3]; }
      double E() const      { return 2 * element[4]; }
      double F() const      { return element[5]; }

    /** different convensions on field naming */
          double &a11()       { return element[0]; }
    const double &a11() const { return element[0]; }
          double &a12()       { return element[1]; }
    const double &a12() const { return element[1]; }
          double &a21()       { return element[1]; }
    const double &a21() const { return element[1]; }

          double &a22()       { return element[2]; }
    const double &a22() const { return element[2]; }

          double &a13()       { return element[3]; }
    const double &a13() const { return element[3]; }
          double &a31()       { return element[3]; }
    const double &a31() const { return element[3]; }

          double &a23()       { return element[4]; }
    const double &a23() const { return element[4]; }
          double &a32()       { return element[4]; }
    const double &a32() const { return element[4]; }

          double &a33()       { return element[5]; }
    const double &a33() const { return element[5]; }


    /* Transformation*/
    void transform(const Matrix33 & B);
    SecondOrderCurve transformed(const Matrix33 &B);



    /* Invariants */
    double invDelta() const;
    double invD() const;
    double invI() const;

    /* Classifiaction */
    enum CurveClass {
        ELLIPSE,
        CIRCLE,
        IMAGINARTY_ELLIPSE,
        HYPERBOLA,
        PARABOLA,
        DEGENERATE, /**< no you.*/
    };

    static inline const char *getName(const CurveClass &value)
    {
        switch (value)
        {
         case ELLIPSE            : return "ELLIPSE"           ; break ;
         case CIRCLE             : return "CIRCLE"            ; break ;
         case IMAGINARTY_ELLIPSE : return "IMAGINARTY_ELLIPSE"; break ;
         case HYPERBOLA          : return "HYPERBOLA"         ; break ;
         case PARABOLA           : return "PARABOLA"          ; break ;
         case DEGENERATE         : return "DEGENERATE"        ; break ;
         default : return "Not in range"; break ;
        }
        return "Not in range";
    }


    CurveClass classify(double epsilon = 1e-10);

    /* Tangents and polars */
    Vector2dd tangent(Vector2dd &point) const;
    Line2d    polar  (Vector2dd &point) const;

    /* Value */
    /* This need to be template function */
    double value (double x, double y) const;
    double value (const Vector2dd &point) const;

    /* Inside */
    bool isInside(const Vector2dd &point) const;
    bool isInside(double x, double y) const;

    /* Solving */
    bool solveForX(double y, double &x1, double &x2);
    bool solveForY(double x, double &y1, double &y2);


    /**
     * SecondOrderCurve possible creator from raw coefficients
     * assuming they are already divided by 2
     **/
    static SecondOrderCurve FromRawCoef(double xx, double xyh, double yy, double xh, double yh, double r );

    /**
     * SecondOrderCurve possible creator from polinom
     * assuming we need to divide non diagonal elements by 2
     **/
    static SecondOrderCurve FromPolynomCoef(double xx, double xy, double yy, double x, double y, double r);



    static SecondOrderCurve FromCircle(const Vector2dd &center, double r);
    static SecondOrderCurve FromCircle(const Circle2d &circle);

    /* This expects symmetric matrix */
    static SecondOrderCurve FromMatrix(const Matrix33 &M);

    /**
     * Special case of errorneous and/or empty curve
     * Please note that IMAGINARTY_ELLIPSE and DEGENERATE configurations could also have no real points
     **/
    static SecondOrderCurve Empty();

    /**
     *  Check for empty queue.
     *  \attention It only garantee that the SecondOrderCurve::Empty() would be empty
     **/
    bool isEmpty();

    void prettyPrint();

    /* Operations with AST */
};


class Ellipse
{
public:
    Vector2dd center;  /**< center of the ellipse */
    Vector2dd axis;    /**< two main axis length  */
    double angle;      /**< angle of the first axis */

    /* https://en.wikipedia.org/wiki/Ellipse#In_analytic_geometry */
    SecondOrderCurve toSecondOrderCurve() const;

    /* */
    static Ellipse FromQuadric(const SecondOrderCurve &p);
    static Ellipse FromPolinomial(const double p[6]);

    Vector2dd pointFromParam(double alpha) const;



    Ellipse();
};

/** Ellipse iterator */

class EllipseSpanIterator
{
public:
    const Ellipse &ellipse;
    SecondOrderCurve c;
    int currentY;
    int endY;


    /* FIX ASAP: storing the reference to object*/
    EllipseSpanIterator(const Ellipse &ellipse);

    void step()
    {
        //SYNC_PRINT(("CircleSpanIterator::step(): called %d\n", currentY));
        currentY++;
    }

    bool hasValue() {
        return currentY <= endY;
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        y  = currentY;
        double x1a, x2a;
        c.solveForX(y, x1a, x2a);
        x1 = fround(x1a);
        x2 = fround(x2a);
    }

    HLineSpanInt getSpan()
    {
        HLineSpanInt span;
        getSpan(span.cy, span.x1, span.x2);
        return span;
    }

    /**
     * C++ style iteration
     **/
    EllipseSpanIterator &begin() {
        return *this;
    }

    EllipseSpanIterator & end() {
        return *this;
    }

    bool operator !=(const EllipseSpanIterator & other) {
         return this->currentY <= other.endY;
    }

    HLineSpanInt operator *() {
        return getSpan();
    }

    void operator ++() {
        step();
    }
};



} // namespace corecvs

#endif // ELLIPSE_H
