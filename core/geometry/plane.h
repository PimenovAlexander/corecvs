#ifndef PLANE_H
#define PLANE_H

#include "geometry/line.h"
#include "math/vector/vector4d.h"

namespace corecvs {

class Plane3d : public FixedVector<double, 4>
{
public:

    Plane3d() {}

    Plane3d(const double &_a, const double &_b, const double &_c, const double &_d)
    {
        (*this)[0] = _a;
        (*this)[1] = _b;
        (*this)[2] = _c;
        (*this)[3] = _d;
    }

    Plane3d(const Vector3dd &_n, const double &_c = 0.0) :
        FixedVector<double, 4>(_n, _c)
    {}

    //@{
    /**
     * Helper functions to access elements
     **/
    inline double &a()
    {
        return (*this)[0];
    }

    inline double &b()
    {
        return (*this)[1];
    }

    inline double &c()
    {
        return (*this)[2];
    }


    inline double &d()
    {
        return (*this)[3];
    }

    //@}

    //@{
    /**
     * Constant versions of helper functions for read-only form constant vectors
     **/
    inline const double &a() const
    {
        return (*this)[0];
    }

    inline const double &b() const
    {
        return (*this)[1];
    }

    inline const double &c() const
    {
        return (*this)[2];
    }

    inline const double &d() const
    {
        return (*this)[3];
    }
    //@}

    /**
     *   Normalizes the normal vector without changing the plane itself
     *
     *   With such a line it is easier to compute distance to the plane
     **/
    void normalise(void)
    {
        double sum(0);
        for (int i = 0; i < size() - 1; i++)
            sum += at(i) * at(i);
        double l = sqrt(sum);
        if (l == 0.0)
            return;
        operator /= (l);
    }

    Plane3d normalised(void) const
    {
        Plane3d result = *this;
        result.normalise();
        return result;
    }

    Plane3d flippedNormal(void) const
    {
        return Plane3d(-normal(), last());
    }

    void flipNormal(void)
    {
        for (int i = 0; i < size() - 1; i++)
            element[i] = - element[i];
    }

    Vector3dd normal(void) const
    {
        return Vector3dd(element[0], element[1], element[2]);
    }

    double last(void) const
    {
        return element[3];
    }

    /**
     *  Intersect result of two planes is a ray
     *
     *  \f[
     *     \vec n_1 \cdot \vec x + d_1 = 0
     *     \vec n_2 \cdot \vec x + d_2 = 0
     *  \f]
     *
     *  Ray
     *  \f[ P(t) = \vec a t + p \f]
     *
     *  Normal:
     *  \f[ a = \vec n_1 \times \vec n_2 \f]
     *
     *  Some point: closest to zero
     *  \f[ p = \frac {(d_2 n_1 - d_1 n_2) \times a } { |a|^2 } \f]
     *
     **/
    Ray3d intersectWith(const Plane3d &other, bool *hasIntersection = NULL) const
    {
        Vector3dd a = (this->normal() ^ other.normal());
        Vector3dd p1 = (other.last() * this->normal() - this->last() * other.normal());
        Vector3dd p = (p1 ^ a) / a.sumAllElementsSq();
        if (hasIntersection != NULL)
            *hasIntersection = (a == Vector3dd(0.0));

        return Ray3d(a, p);
    }

    /**
     *  Intersect result of plane and a ray is a point
     *
     *  point -
     *  \f[ P(t) = \vec a t + \vec p \f]
     *
     *  plane -
     *  \f[ \vec n \vec x + d = 0 \f]
     *
     *  Finding intersection
     *
     *  \f[ \vec n (\vec a t + \vec p) + d = 0 \f]
     *  \f[ t = - \frac {\vec n \vec p + d} { \vec n \vec a } \f]
     *
     *
     **/
    Vector3dd intersectWith(const Ray3d &ray, bool *hasIntersection = NULL) const
    {
        return ray.getPoint(intersectWithP(ray, hasIntersection));
    }

    double intersectWithP(const Ray3d &ray, bool *hasIntersection = NULL) const
    {
        double denum = normal() & ray.a;

        bool intersects = (denum != 0.0);
        if (hasIntersection) *hasIntersection = intersects;

        if (!intersects)
            return 0.0;

        return -((normal() & ray.p) + last()) / denum;
    }


     /**
      * \f$ a x_0 + b y_0 + c z_0 + d \f$
      *
      **/
     double pointWeight(const Vector3dd & point) const
     {
         return (normal() & point) + last();
     }

     /**
      *
      * \f$d = \frac {\left| a x_0 + b y_0 + c z_0 + d  \right|}  {\sqrt (a^2 + b^2 + c^2) }\f$
      *
      **/
     double distanceTo (const Vector3dd &point) const
     {
         double d = pointWeight(point);
         if (d < 0) d = -d;
         double l = normal().l2Metric();
         return (d / l);
     }

     /**
      *
      * \f$d = \frac {(a x_0 + b y_0 + c z_0 + d)^2 }  {a^2 + b^2 + c^2}\f$
      *
      **/
     double sqDistanceTo (const Vector3dd &point) const
     {
         double d = pointWeight(point);
         d *= d;
         double lsq = normal().sumAllElementsSq();
         return (d / lsq);
     }

     /**
      *
      * \f$d = \frac {a x_0 + b y_0 + c z_0 + d}  {\sqrt (a^2 + b^2 + c^2) }\f$
      *
      **/

     double deviationTo (const Vector3dd &point) const
     {
         double d = pointWeight(point);
         double l = normal().l2Metric();
         return (d / l);
     }

     /**
      *   projecting a point to the current plane
      *
      *   \f[ \vec R = \vec P - {{\vec n \vec P + d } \over {| \vec n |}}{{\vec n} \over {| \vec n |}}\f]
      **/
     Vector3dd projectPointTo(const Vector3dd &point) const
     {
         double d = pointWeight(point);
         double l2 = normal().sumAllElementsSq();
         double t = (d / l2);
         return point - t * normal();
     }


     Ray3d projectRayTo(const Ray3d &ray) const
     {
         double l2 = normal().sumAllElementsSq();

         double dp = pointWeight(ray.p);
         double tp = (dp / l2);

         double da = normal() & ray.a;
         double ta = (da / l2);

         return Ray3d(ray.a - ta * normal(), ray.p - tp * normal());
     }

     /**
      *   projecting zero to the current plane
      **/
     Vector3dd projectZeroTo() const
     {
         double l2 = normal().sumAllElementsSq();
         double t = (last() / l2);
         return - t * normal();
     }

    /**
     *  Construct a plane from normal
     **/
    static Plane3d FromNormal(const Vector3dd &normal)
    {
        return Plane3d(normal);
    }

    /**
     *  Construct the Plane from normal and a point
     *
     *  \f[
     *      (\vec x - \vec p) \vec n = \vec 0
     *  \f]
     *  \f[
     *      \vec x \vec n - \vec p \vec n = \vec 0
     *  \f]
     **/
    static Plane3d FromNormalAndPoint(const Vector3dd &normal, const Vector3dd &point)
    {
        return Plane3d(normal, -(normal & point));
    }

    /**
     *  This is a helper method that returns the normal of the points
     *
     **/
    static Vector3dd NormalFromPoints(const Vector3dd &p, const Vector3dd &q, const Vector3dd &r)
    {
        return (p - q) ^ (p - r);
    }

    /**
     *  Construct the Plane from 3 points
     *
     *  \f[
     *      (\vec x - \vec p)((\vec p - \vec q) \times (\vec p - \vec r)) = 0
     *  \f]
     **/
    static Plane3d FromPoints(const Vector3dd &p, const Vector3dd &q, const Vector3dd &r)
    {
        return FromNormalAndPoint( NormalFromPoints(p, q, r), p);
    }

    /**
     *  Construct the Plane from а point and two vectors inside a plane
     *
     **/
    static Plane3d FromPointAndVectors(const Vector3dd &p, const Vector3dd &v1, const Vector3dd &v2)
    {
        return FromNormalAndPoint( v1 ^ v2, p);
    }

    Vector3dd toDual() const {
        Vector3dd n = normal();

        double m0 = -n.x() / n.z();
        double m1 = -n.y() / n.z();
        double b = -last() / n.z();

        return Vector3dd(m0, m1, -b);
    }

    static Plane3d FromDual(const Vector3dd &p) {
        return Plane3d(-p.x(), -p.y(), 1.0, p.z());
    }

    Vector4dd toDualP() const {
        return Vector4dd(a(), b(), c(), d());
    }

    static Plane3d FromDualP(const Vector4dd &p) {
        return Plane3d(p.x(), p.y(), p.z(), p.w());
    }


};

} // namespace corecvs

#endif // PLANE_H
