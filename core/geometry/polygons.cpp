/**
 * \file polygons.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 14, 2010
 * \author alexander
 */

#include "polygons.h"
namespace corecvs {

/**
 *
 * Check if point is inside the polygon
 **/
int Poligon::isInside(const Vector2dd &a)
{
    double oldsign = 0;
    int len = (int)size();
    for (int i = 0; i < len; i++) {
        Vector2dd &curr = operator [](i);
        Vector2dd &next = operator []((i + 1) % len);
        Vector2dd normal = (next - curr).rightNormal();
        Vector2dd diff = a - curr;
        double sign = diff & normal;
        if (oldsign > 0  && sign < 0)
            return false;
        if (oldsign < 0  && sign > 0)
            return false;
        oldsign = sign;
    }
    return true;
}

/**
 *
 * This method implements Cyrus-Beck algorithm
 * http://en.wikipedia.org/wiki/Cyrus%E2%80%93Beck_algorithm
 *
 * */
bool Poligon::clipRay(const Ray2d &ray, double &t1, double &t2)
{
#if 0

    Vector2dd &dir   = ray.a;
    Vector2dd &point = ray.p;

    for (int i = 0; i < len; i++) {
        Vector2dd &curr = operator [](i);
        Vector2dd &next = operator []((i + 1) % len);
        Vector2dd normal = (next - curr).rightNormal();

        Vector2dd diff = point - curr;

        double sign = diff & normal;
        bool enter = sign > 0;
    }
#endif

}


} //namespace corecvs

