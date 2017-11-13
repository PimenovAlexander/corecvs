#include "core/math/matrix/matrix22.h"
#include "core/geometry/ellipse.h"
#include "core/polynomial/polynomialSolver.h"

namespace corecvs {





SecondOrderCurve Ellipse::toSecondOrderCurve() const
{

    double sa = sin(angle);
    double ca = cos(angle);

    double a2 = axis.x() * axis.x();
    double b2 = axis.y() * axis.y();

    double xx = a2 * sa * sa + b2 * ca * ca;
    double yy = a2 * ca * ca + b2 * sa * sa;
    double xy = 2 * (b2 - a2) * sa * ca;

    return SecondOrderCurve::FromPolynomCoef(
        xx,
        xy,
        yy,
        -2 * xx * center.x() -     xy * center.y(),
        -    xy * center.x() - 2 * yy * center.y(),
        xx * center.x() * center.x() + xy * center.x() * center.y() + yy * center.y() * center.y() - a2 * b2
    );
}

Ellipse Ellipse::FromQuadric(const SecondOrderCurve &p)
{
    double poly[6] = {p.A(), p.B(), p.C(), p.D(), p.E(), p.F()};
    return FromPolinomial(&poly[0]);
}

Ellipse Ellipse::FromPolinomial(const double p[])
{
    double A = p[0];
    double B = p[1];
    double C = p[2];
    double D = p[3];
    double E = p[4];
    double F = p[5];

    cout << A << " " << B << " " << C << " " << D << " "<< E << " " << F << " " << std::endl;

    double Det = B * B - 4 * A * C;
    double T = (A * E * E + C * D * D - B * D * E + Det * F);
    double R = sqrt((A - C) * (A - C)  + B * B);
    double a = - sqrt(2 * T * (A + C + R)) / Det;
    double b = - sqrt(2 * T * (A + C - R)) / Det;

    double xc = (2 * C * D - B * E) / Det;
    double yc = (2 * A * E - B * D) / Det;
    double tetta = atan2(C - A - R, B);

#if DEBUG
    cout << "xc = " << xc << endl;
    cout << "yc = " << yc << endl;

    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << "tetta = " << tetta << endl;
#endif

    Ellipse result;
    result.center = Vector2dd(xc, yc);
    result.axis   = Vector2dd( a,  b);
    result.angle = tetta;
    return result;
}

Vector2dd Ellipse::pointFromParam(double alpha) const
{
    Vector2dd point = axis * Vector2dd::FromPolar(alpha);
    point = Matrix33::RotationZ(angle) * point;
    return point + center;
}

Ellipse::Ellipse()
{

}

Matrix33 SecondOrderCurve::toMatrixForm() const
{
    return Matrix33(
        a11(), a12(), a13(),
        a21(), a22(), a23(),
        a31(), a32(), a33()
                );
}

SecondOrderCurve SecondOrderCurve::FromMatrix(const Matrix33 &M)
{
    SecondOrderCurve result;
    result.a11() = M.a(0,0);
    result.a12() = M.a(0,1);
    result.a22() = M.a(1,1);
    result.a13() = M.a(0,2);
    result.a23() = M.a(1,2);
    result.a33() = M.a(2,2);

    return result;
}

SecondOrderCurve SecondOrderCurve::Empty()
{
    return SecondOrderCurve::FromRawCoef(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

bool SecondOrderCurve::isEmpty()
{
    if ((a11() == 0.0) && (a12() == 0.0) && (a22() == 0.0) && (a13() == 0.0) && (a23() == 0.0) && (a33() != 0.0))
        return true;
    return false;
}


/**

  x^T M x = 0
  x -> B x = y

  (B x)^T B^T^(-1) M B^(-1) (B x) = 0


  ( b11 b21  0 ) -1 ( a11 a12 a13 )  ( b11 b12  b_x )
  ( b12 b22  0 )    ( a12 a22 a23 )  ( b21 b22  b_y )  = 0
  ( b_x b_y  1 )    ( a13 a23 a33 )  (  0   0    1  )

 **/

void SecondOrderCurve::transform(const Matrix33 &B)
{
    (*this) = SecondOrderCurve::FromMatrix(B.t().inv() * toMatrixForm() * B.inv());
}

SecondOrderCurve SecondOrderCurve::transformed(const Matrix33 &B)
{
    return SecondOrderCurve::FromMatrix(B.t().inv() * toMatrixForm() * B.inv());
}

double SecondOrderCurve::invDelta() const
{
    return toMatrixForm().det();
}

double SecondOrderCurve::invD() const
{
    Matrix22 minor = Matrix22(
            a11(), a12(),
            a21(), a22()
    );
    cout <<  "Minor" << minor << endl;

    return minor.det();
}

double SecondOrderCurve::invI() const
{
    return a11() + a22();
}

SecondOrderCurve::CurveClass SecondOrderCurve::classify(double epsilon)
{
    double deltaInv = invDelta();
    if (fabs(deltaInv) < epsilon ) {
        return DEGENERATE;
    } else {
        double dInv = invD();
        if (fabs(dInv) < epsilon ) {
            return PARABOLA;
        }

        if (dInv < 0) {
            return HYPERBOLA;
        } else {
            double iInv = invI();
            if (iInv * deltaInv < 0)
            {
                if (fabs(iInv * iInv - 4 * dInv) < epsilon) {
                    return CIRCLE;
                } else {
                    return ELLIPSE;
                }
            }
            else
                return IMAGINARTY_ELLIPSE;
        }
    }
    return DEGENERATE;
}

Vector2dd SecondOrderCurve::tangent(Vector2dd &point) const
{
    return Vector2dd(
        a11() * point.x() +  a12() * point.y() + a13(),
        a12() * point.x() +  a22() * point.y() + a23()
                );
}

double SecondOrderCurve::value(const Vector2dd &p) const
{

    return  value(p.x(), p.y());
}

double SecondOrderCurve::value(double x, double y) const
{
    return       a11() * x * x + a22() * y * y +
           2 * ( a12() * x * y +
                 a13() * x + a23() * y )  +
                 a33();
}

bool SecondOrderCurve::isInside(double x, double y) const
{
    return value(x, y) < 0;
}

bool SecondOrderCurve::solveForX(double y, double &x1, double &x2)
{
    double coef[3];

    coef[2] = a11();
    coef[1] = 2 * (a12() * y + a13());
    coef[0] = a22() * y  * y + 2 * a23() * y + a33();

    double roots[2] = {0.0, 0.0 };
    size_t  num = PolynomialSolver::solve_imp<2>(coef, roots, 2);
    if (num == 0) {
        return false;
    }

    if (num == 1) {
        x1 = roots[0];
        x2 = roots[0];
        return true;
    }

    if (num == 2) {
        x1 = roots[0];
        x2 = roots[1];
        if (x1 > x2) std::swap(x1,x2);
        return true;
    }
    return false;
}

bool SecondOrderCurve::solveForY(double x, double &y1, double &y2)
{
    double coef[3];

    coef[2] = a22();
    coef[1] = 2 * (a12() * x + a23());
    coef[0] = a11() * x  * x + 2 * a13() * x + a33();

    double roots[2] = {0.0, 0.0 };
    size_t  num = PolynomialSolver::solve_imp<2>(coef, roots, 2);
    if (num == 0) {
        return false;
    }

    if (num == 1) {
        y1 = roots[0];
        y2 = roots[0];
        return true;
    }

    if (num == 2) {
        y1 = roots[0];
        y2 = roots[1];
        return true;
    }
    return false;
}

SecondOrderCurve SecondOrderCurve::FromRawCoef(double xx, double xyh, double yy, double xh, double yh, double r)
{
    SecondOrderCurve result;
    result.a11() = xx;
    result.a12() = xyh;
    result.a22() = yy;
    result.a13() = xh;
    result.a13() = yh;
    result.a33() = r;
    return result;
}

SecondOrderCurve SecondOrderCurve::FromPolynomCoef(double xx, double xy, double yy, double x, double y, double r)
{
    SecondOrderCurve result;
    result.a11() = xx;
    result.a12() = xy / 2.0;
    result.a22() = yy;
    result.a13() = x / 2.0;
    result.a23() = y / 2.0;
    result.a33() = r;
    return result;
}

bool SecondOrderCurve::isInside(const Vector2dd &point) const
{
    return isInside(point.x(), point.y());
}

/**
  \f[ (x - x_0)^2 + (y - y_0)^2 = R^2 \f]
  \f[ (x^2 - 2 * x_0 * x + x_0^2) + (y^2 - 2 * y_0 * x + y_0^2) = R^2 \f]


 **/
SecondOrderCurve SecondOrderCurve::FromCircle(const Vector2dd &center, double r)
{
    SecondOrderCurve circle;
    circle.a11() = 1.0; // xx
    circle.a12() = 0.0; // xy
    circle.a22() = 1.0; // yy

    circle.a13() = -center.x(); // x
    circle.a23() = -center.y(); // y

    circle.a33() = center.sumAllElementsSq() - r * r; // free
    return circle;
}

SecondOrderCurve SecondOrderCurve::FromCircle(const Circle2d &circle)
{
    return FromCircle(circle.c, circle.r);
}


void SecondOrderCurve::prettyPrint()
{
    cout << a11() << "x^2 + " << a22() << "y^2 + 2 *" << a12() << "xy + 2 *" << a13() << "x + 2 * " << a23() << "y +" << a33() << endl;
    cout << getName(classify()) << endl;
    cout << "Delta :" << invDelta()  << endl;
    cout << "D     :" << invD()      << endl;
    cout << "I     :" << invI()      << endl;

}

EllipseSpanIterator::EllipseSpanIterator(const Ellipse &ellipse) : ellipse(ellipse)
{
    c = ellipse.toSecondOrderCurve();

    double coef[3]; /* Computing min/max Y*/
    coef[2] = c.a12() * c.a12() - c.a11() * c.a22();
    coef[1] = 2 * (c.a12() * c.a13() - c.a11() * c.a23());
    coef[0] = c.a13() * c.a13() - c.a11() * c.a33();

    double roots[2];

    size_t v = PolynomialSolver::solve_imp<2>(coef, roots, 2);
    if (v == 2) {
        currentY = fround(std::min(roots[0], roots[1]));
        endY     = fround(std::max(roots[0], roots[1]));
        return;
    }

    currentY = 1;
    endY     = 0;
}


} // namespace corecvs
