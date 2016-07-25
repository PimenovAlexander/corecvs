#include "conic.h"

namespace corecvs {

bool Circle2d::intersectWith(const Circle2d &other, Vector2dd &point1, Vector2dd &point2)
{
    double a;
    double x;

    if (!intersectHelper(other, x, a))
        return false;

    Vector2dd dir = (other.c - c);
    dir.normalise();
    Vector2dd norm = dir.leftNormal();
    point1 = c + dir * x + norm * a;
    point2 = c + dir * x - norm * a;
    return true;
}

bool Sphere3d::intersectWith(const Sphere3d &other, Circle3d &result)
{
    double a;
    double x;

    if (!intersectHelper(other, x, a)) {
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


/**
 *
 *
 *
 *
 *
 *
 *
 *
 *   *----------------------------
 *
 *
 *
 *
 **/
bool corecvs::Sphere3d::intersectWith(const corecvs::Ray3d &ray, double &t1, double &t2)
{
    Vector3dd toCenter  = c  - ray.p;
    double toCen2 = toCenter & toCenter;
    double proj  = ray.a & toCenter;
    double hdist  = (r * r) - toCen2 + proj * proj;

    if (hdist < 0) {
        return false;
    }

    hdist = sqrt (hdist);

    t1 =  proj - hdist;
    t2 =  proj + hdist;
    return true;
}



} // namespace corecvs

