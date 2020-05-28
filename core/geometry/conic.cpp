#include "geometry/conic.h"

namespace corecvs {

Circle2d Circle2d::Circumcircle(const Triangle2dd &triangle) {
    Vector2dd p1 = triangle.p1();
    Vector2dd p2 = triangle.p2();
    Vector2dd p3 = triangle.p3();

    Matrix33 Sx
            (p1 & p1, p1.y(), 1,
             p2 & p2, p2.y(), 1,
             p3 & p3, p3.y(), 1);

    Matrix33 Sy
            (p1.x(), p1 & p1, 1,
             p2.x(), p2 & p2, 1,
             p3.x(), p3 & p3, 1);

    Matrix33 ma
            (p1.x(), p1.y(), 1,
             p2.x(), p2.y(), 1,
             p3.x(), p3.y(), 1);

    Matrix33 mb
            (p1.x(), p1.y(), p1 & p1,
             p2.x(), p2.y(), p2 & p2,
             p3.x(), p3.y(), p3 & p3);

    Vector2dd S(Sx.det() / 2, Sy.det() / 2);
    double a = ma.det();

    Vector2dd center = S / a;
    double    raduis = sqrt(mb.det() / a + ((S & S) / (a * a)));
    return Circle2d(center, raduis);
}

bool Circle2d::intersectWith(const Circle2d &other, Vector2dd &point1, Vector2dd &point2)
{
    double a;
    double x;

    if (!intersectConicHelper(other, x, a))
        return false;

    Vector2dd dir = (other.c - c);
    dir.normalise();
    Vector2dd norm = dir.leftNormal();
    point1 = c + dir * x + norm * a;
    point2 = c + dir * x - norm * a;
    return true;
}

bool Circle2d::intersectWith(const Ray2d &ray, double &t1, double &t2) const
{
    return intersectRayHelper(ray, t1, t2);
}

bool Sphere3d::intersectWith(const Sphere3d &other, Circle3d &result)
{
    double a;
    double x;

    if (!intersectConicHelper(other, x, a)) {
        return false;
    }

    Vector3dd dir = (other.c - c);
    dir.normalise();
    result.r = a;
    result.normal = dir;
    result.c = c + dir * x;
    return true;
}

bool Sphere3d::intersectWith(const Plane3d &plane, Circle3d &intersection)
{
    Vector3dd projection = plane.projectPointTo(c);
    Circle3d result;
    result.normal = plane.normal();
    result.c = projection;

    double xs = (projection - c).sumAllElementsSq(); /* this seem to be faster then sqDistanceTo since we have already got projection*/
    double as = (r * r) - xs;
    if (as < 0) {
        return false;
    }
    result.r = sqrt(as);
    intersection = result;
    return true;
}

void Circle3d::getPoints(Vector3dd &out1, Vector3dd &out2)
{
    Vector3dd ort1;
    Vector3dd ort2;
    normal.orthogonal(ort1, ort2);
    out1 = c + ort1 * r;
    out2 = c + ort2 * r;
}

Vector3dd Circle3d::getPoint(double angle)
{
    Vector3dd ort1;
    Vector3dd ort2;
    normal.orthogonal(ort1, ort2);
    return (c + r * (ort1 * sin(angle) + ort2 * cos(angle)));
}


bool Sphere3d::intersectWith(const corecvs::Ray3d &ray, double &t1, double &t2)
{
    return UnifiedSphere::intersectRayHelper(ray, t1, t2);
}



} // namespace corecvs

