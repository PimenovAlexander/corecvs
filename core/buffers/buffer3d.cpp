#include "buffer3d.h"

namespace corecvs {

SwarmPoint* Buffer3d::p0 = NULL;

double Buffer3d::getLength(vector<SwarmPoint> &cloud, Vector2d32 p1, Vector2d32 p2)
{
    unsigned i1 = 0; int d1 = std::numeric_limits<int>::max();
    unsigned i2 = 0; int d2 = std::numeric_limits<int>::max();

    for (unsigned i = 0; i < cloud.size(); i++)
    {
        Vector2dd tex = cloud[i].texCoor;
        Vector2d32 p = Vector2d32(fround(tex.x()), fround(tex.y()));

        if ((p - p1).l1Metric() < d1)
        {
            i1 = i;
            d1 = (p - p1).l1Metric();
        }
        if ((p - p2).l1Metric() < d2)
        {
            i2 = i;
            d2 = (p - p2).l1Metric();
        }
    }
    double r = fround((cloud[i1].point - cloud[i2].point).l2Metric());

    return r;
}

double Buffer3d::getLength(Vector2d32 p1, Vector2d32 p2) const
{
    if (getNearestPoint(p1) && getNearestPoint(p2)) {
        return fround((element(p1)->point - element(p2)->point).l2Metric());
    }
    else return 0.0;
}


} //namespace corecvs
