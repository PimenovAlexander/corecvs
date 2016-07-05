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
int Polygon::isInside(const Vector2dd &a) const
{
    double oldsign = 0;
    int len = (int)size();
    for (int i = 0; i < len; i++)
    {
        const Vector2dd &curr = operator [](i);
        const Vector2dd &next = operator []((i + 1) % len);
        const Vector2dd normal = (next - curr).rightNormal();
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

bool Polygon::isConvex(bool *direction) const
{
    double oldsign = 0;
    int len = (int)size();
    for (int i = 0; i < len; i++)
    {
        const Vector2dd &curr = operator [](i);
        const Vector2dd &next = operator []((i + 1) % len);
        const Vector2dd &nnext = operator []((i + 2) % len);

        double sign = (next - curr).rightNormal() & (nnext - next);

        if (oldsign > 0  && sign < 0)
            return false;
        if (oldsign < 0  && sign > 0)
            return false;
        oldsign = sign;
    }
    if (direction != NULL) {
        *direction =  (oldsign > 0);
    }
    return true;
}


#if 0
bool Polygon::clipRay(const Ray2d &ray, double &t1, double &t2)
{
    t1 = -numeric_limits<double>::max();
    t2 =  numeric_limits<double>::max();


    const Vector2dd &a = ray.a;
    const Vector2dd &p = ray.p;

    for (unsigned i = 0; i < size();  i++) {
        int j = (i + 1) % size();
        Vector2dd &r1 = operator [](i);
        Vector2dd &r2 = operator [](j);

        Vector2dd n = (r2 - r1).rightNormal();
        Vector2dd diff = r1 - p;

        double numen = diff & n;
        double denumen = a & n;
        double t = numen / denumen;

        if ((denumen > 0) && (t > t1)) {
            t1 = t;
        }
        if ((denumen < 0) && (t < t2)) {
            t2 = t;
        }

        cout << "Intersection " << t << " at " << ray.getPoint(t) << " is " << (numen > 0 ? "enter" : "exit") << std::endl;

    }
    return t2 > t1;
}
#endif


} //namespace corecvs

