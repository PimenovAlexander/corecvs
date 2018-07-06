#ifndef CONIC_H
#define CONIC_H

#include "core/geometry/line.h"
#include "core/geometry/plane.h"
#include "core/buffers/rgb24/lineSpan.h"

namespace corecvs {

template<class RealType, class VectorType>
class UnifiedSphere
{
public:
    VectorType c;
    double r;       /**< Good practice sometimes is to store \f$r^2\f$ instead or r **/

    UnifiedSphere() {}

    UnifiedSphere(const VectorType &_c, double _r) :
        c(_c),
        r(_r)
    {}

    double diameter()
    {
        return 2 * r;
    }

    bool hasPoint(const VectorType &p, double epsilon = 1e-7)
    {
        return (fabs((p - c).l2Metric() - r) < epsilon);
    }

    /**
     * This function intersects two circles / spheres (3D or more).
     *
     *  Decomposes the intersection point (if one exists) into the corrdinate frame with ox - on the line
     * connecting the centers. And ar is a radius of the intersection
     *
     **/
    bool intersectConicHelper(const RealType &other, double &xr, double &ar)
    {
        double ds = (other.c - c).sumAllElementsSq();
        double d =  sqrt(ds);

        double r1 = r      , r1s = r1*r1;
        double r2 = other.r, r2s = r2*r2;

        double t = (ds - r2s + r1s);
        double x = t / (2 * d);

        double D = 4*ds*r1s - t * t;
        if (D < 0) {
            return false;
        }
        ar = 1.0 / (2 * d) * sqrt(D);
        xr = x;
        return true;
    }

    /**
     * \brief This method intersects the ray with a sphere.
     *
     * \param ray - input ray. could be not normalised
     *
     **/
    template<class RayType>
    bool intersectRayHelper(const RayType &ray, double &t1, double &t2 )
    {

        VectorType toCenter  = c  - ray.p;
        double toCen2 = toCenter & toCenter;
        double proj  = (ray.a & toCenter) / ray.a.l2Metric();
        double hdist  = (r * r) - toCen2 + proj * proj;

        if (hdist < 0) {
            return false;
        }

        hdist = sqrt (hdist);

        t1 =  (proj - hdist) / ray.a.l2Metric();
        t2 =  (proj + hdist) / ray.a.l2Metric();
        return true;
    }



    friend std::ostream & operator <<(std::ostream &out, const UnifiedSphere &sphere)
    {
        out << "[" << sphere.c << "] (" << sphere.r << ")";
        return out;
    }


};

class Circle2d : public UnifiedSphere<Circle2d, Vector2dd>
{
public:

    Circle2d(const Vector2dd &_c, double _r) :
        UnifiedSphere(_c, _r)
    {}

    Circle2d(double _x, double _y , double _r) :
        UnifiedSphere(Vector2dd(_x, _y), _r)
    {}

    bool intersectWith(const Circle2d &other, Vector2dd &point1, Vector2dd &point2);
    bool intersectWith(const Ray2d &ray, double &t1, double &t2);

};

class Circle3d;

class Sphere3d : public UnifiedSphere<Sphere3d, Vector3dd>
{
public:
    Sphere3d(){}

    Sphere3d(const Vector3dd &c, double r) :
        UnifiedSphere(c, r)
    {}

    Sphere3d(double x, double y, double z, double r) :
        UnifiedSphere(Vector3dd(x,y,z), r)
    {}

    bool intersectWith(const Sphere3d &other, Circle3d &result);
    bool intersectWith(const  Plane3d &plane, Circle3d &intersection);
    bool intersectWith(const  Ray3d   &ray, double &t1, double &t2);

};


class Circle3d : public Sphere3d
{
public:
    Vector3dd normal;

    void getPoints(Vector3dd &out1, Vector3dd &out2);

    Vector3dd getPoint(double angle);

    Plane3d getPlane() {
        return Plane3d::FromNormalAndPoint(normal, c);
    }
};

class Cylinder3d : public Circle3d
{
public:
    double   height;

    Plane3d getBottomPlane() {
        return getPlane();
    }

    Ray3d getAxis()
    {
        return Ray3d(normal, c);
    }

    Plane3d getTopPlane() {
        return Plane3d::FromNormalAndPoint(-normal, c + height * normal.normalised());
    }
};

/* Circle 2d iterator */
class CircleSpanIterator
{
public:
    const Circle2d &circle;

    double radSQ;
    int currentY;
    int currentDX;

    /* FIX ASAP: storing the reference to object*/
    CircleSpanIterator(const Circle2d &circle) : circle(circle)
    {
        radSQ     = circle.r * circle.r;
        currentY  = (int)(-circle.r + circle.c.y() - 1);
        currentDX = 0;
    }

    void step()
    {
        //SYNC_PRINT(("CircleSpanIterator::step(): called %d\n", currentY));
        currentY++;
        double h = currentY - 0.5 - circle.c.y();
        double hsq = h * h;
        currentDX = (int)sqrt((float)(radSQ - hsq));
    }

    bool hasValue() {
        return currentY <= (circle.c.y() + circle.r);
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        y  = currentY;
        x1 = (int)circle.c.x() - currentDX;
        x2 = (int)circle.c.x() + currentDX;
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
    CircleSpanIterator &begin() {
        return *this;
    }

    CircleSpanIterator & end() {
        return *this;
    }

    bool operator !=(const CircleSpanIterator & other) {
        return this->currentY <= (other.circle.c.y() + other.circle.r);
    }

    HLineSpanInt operator *() {
        return getSpan();
    }

    void operator ++() {
        step();
    }
};



} // namespace corecvs

#endif // CONIC_H
