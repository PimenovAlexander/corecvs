#include "core/geometry/convexHull.h"

using namespace corecvs;
using namespace std;


Polygon ConvexHull::GiftWrap(const std::vector<Vector2dd> &list)
{

    Polygon toReturn;
    if (list.empty())
    {
        // SYNC_PRINT(("ConvexHull::GiftWrap(): Input list in empty"));
        return toReturn;
    }

    if (list.size() == 1)
    {
        // SYNC_PRINT(("ConvexHull::GiftWrap(): Input list has only one point"));
        toReturn.push_back(list[0]);
        return toReturn;
    }

    /* Find one point in hull */
    size_t minYId = 0;
    double minY = list[0].y();
    for (size_t i = 1; i < list.size(); i++)
    {
        if (list[i].y() < minY)
        {
            minY = list[i].y();
            minYId = i;
        }
    }

    // SYNC_PRINT(("ConvexHull::GiftWrap(): starting with point %i (%lf %lf)\n", minYId, list[minYId].x(), list[minYId].y() ));
    toReturn.push_back(list[minYId]);
    Vector2dd direction = Vector2dd::OrtX();
    Vector2dd current = list[minYId];

    /* Wrap */
    do {
        Vector2dd next;
        Vector2dd nextDir(0.0);
        double vmax = -std::numeric_limits<double>::max();

        for (const Vector2dd &point : list)
        {
            if (point == current)
                continue;

            Vector2dd dir1 = (point - current).normalised();
            double v = direction & dir1;
            // SYNC_PRINT(("Checking asimuth %lf\n", v));
            if (v > vmax) {
                vmax = v;
                next = point;
                nextDir = dir1;
            }
        }

        /* That is exact same point. double equality is safe */
        if (next == toReturn.front())
        {
            break;
        }

        // SYNC_PRINT(("ConvexHull::GiftWrap(): next point (%lf %lf) with azimuth %lf\n", next.x(), next.y(), vmax));

        toReturn.push_back(next);
        current = next;
        direction = nextDir;

    } while (/*toReturn.size() < 10*/true);


    return toReturn;
}

double ccw(const Vector2dd& p1, const Vector2dd& p2, const Vector2dd& p3)
{
    //return (p2.x() - p1.x())*(p3.y() - p1.y()) - (p2.y() - p1.y())*(p3.x() - p1.x());
    return (p2 - p1) & (p3 - p1).leftNormal();
}


double ccwProjective(const Vector3dd& p1, const Vector3dd& p2, const Vector3dd& p3)
{
    return -(p1 ^ p2) & p3;
}

template<typename PointType>
Polygon ConvexHull::GrahamScan(std::vector<PointType> points)
{
    if (points.size() < 3)
        return Polygon();

    //find value with lowest y-coordinate
    size_t idx = 0;
    double yMin = std::numeric_limits<double>::max();
    for (size_t i = 1; i < points.size(); i++)
    {
        bool less = points[i].y() < yMin;
        less |= (points[i].y() == yMin) && (points[i].x() < points[idx].x());
        if (less)
        {
            yMin = points[i].y();
            idx = i;
        }
    }

    //sort ascending polar angle around lowest y-coordinate value
    std::swap(points[0], points[idx]);
    std::sort(points.begin() + 1, points.end(), [=](const Vector2dd& a, const Vector2dd& b) -> bool
        {
            Vector2dd ab = b - a;
            double polar = ccw(points[0], a, b);

            if (polar != 0.0)
                return (polar > 0);
            else
                return (ab.y() > 0);
        }
    );

    /* Dedup */
    auto last = std::unique(points.begin(), points.end());
    points.erase(last, points.end());

    //pass of Graham scan
    Polygon hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    for (size_t i = 2; i < points.size(); i++)
    {
        int M = (int)hull.size();
        while (ccw(hull[M - 2], hull[M - 1], points[i]) < 0)
        {
            hull.pop_back();
            M = (int)hull.size();
            if (hull.size() < 2)
                break;
        }
        hull.push_back(points[i]);
    }

    return hull;
}


Polygon ConvexHull::GrahamScan(std::vector<Vector2dd> points)
{
    if (points.size() < 3)
        return Polygon();

    //find value with lowest y-coordinate
    size_t idx = 0;
    double yMin = std::numeric_limits<double>::max();
    for (size_t i = 0; i < points.size(); i++)
    {
        bool less = points[i].y() < yMin;
        less |= (points[i].y() == yMin) && (points[i].x() < points[idx].x());
        if (less)
        {
            yMin = points[i].y();
            idx = i;
        }
    }

    //sort ascending polar angle around lowest y-coordinate value
    std::swap(points[0], points[idx]);
    std::sort(points.begin() + 1, points.end(), [=](const Vector2dd& a, const Vector2dd& b) -> bool
        {
            Vector2dd ab = b - a;
            double polar = ccw(points[0], a, b);

            if (polar != 0.0)
                return (polar > 0);
            else
                return (ab.y() > 0);
        }
    );

    /* Dedup */
    auto last = std::unique(points.begin(), points.end());
    points.erase(last, points.end());

    //pass of Graham scan
    Polygon hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    for (size_t i = 2; i < points.size(); i++)
    {
        int M = (int)hull.size();
        while (ccw(hull[M - 2], hull[M - 1], points[i]) < 0)
        {
            hull.pop_back();
            M = (int)hull.size();
            if (hull.size() < 2)
                break;
        }
        hull.push_back(points[i]);
    }

    return hull;
}

ProjectivePolygon ConvexHull::GrahamScan(std::vector<Vector3dd> points)
{
    /* Need to be corrected*/
    if (points.size() < 3)
        return ProjectivePolygon();

    //find value with lowest y-coordinate
    size_t iMin = 0;
    double yMin = std::numeric_limits<double>::max();

    for (size_t i = 0; i < points.size(); i++)
    {
        double ycost = points[i].normalised() & Vector3dd::OrtY();
        if (ycost < yMin)
        {
            yMin = ycost;
            iMin = i;
        }
    }

    //cout << "Chosen point " << points[iMin] << endl;

    //sort ascending polar angle around lowest y-coordinate value
    std::swap(points[0], points[iMin]);
    std::sort(points.begin() + 1, points.end(), [=](const Vector3dd& a, const Vector3dd& b) -> bool
        {
            Vector3dd ab = b - a;
            double polar = ccwProjective(points[0], a, b);

            if (polar != 0.0)
                return (polar > 0);
            else
                return (ab.y() > 0);
        }
    );

    /* Dedup */
    auto last = std::unique(points.begin(), points.end());
    points.erase(last, points.end());

    //pass of Graham scan
    ProjectivePolygon hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    for (size_t i = 2; i < points.size(); i++)
    {
        int M = (int)hull.size();
        while (ccwProjective(hull[M - 2], hull[M - 1], points[i]) < 0)
        {
            hull.pop_back();
            M = (int)hull.size();
            if (hull.size() < 2)
                break;
        }
        hull.push_back(points[i]);
    }

    return hull;
}


Polygon ConvexHull::ConvexHullCompute(std::vector<Vector2dd> points, ConvexHull::ConvexHullMethod &method)
{
    switch (method) {
    case ConvexHullMethod::GIFT_WARP:
        return ConvexHull::GiftWrap(points);
        break;
    default:
        return ConvexHull::GrahamScan(points);
        break;
    }
}


