#include "core/geometry/convexHull.h"

#include "mesh3d.h"

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

    } while (true);
    return toReturn;
}

vector<uint> ConvexHull::GiftWrapId(const std::vector<Vector2dd> &list)
{
    vector<uint> toReturn;

    if (list.empty())
    {
        // SYNC_PRINT(("ConvexHull::GiftWrap(): Input list in empty"));
        return toReturn;
    }
    if (list.size() == 1)
    {
        // SYNC_PRINT(("ConvexHull::GiftWrap(): Input list has only one point"));
        toReturn.push_back(0);
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
    toReturn.push_back(minYId);
    Vector2dd direction = Vector2dd::OrtX();
    Vector2dd current = list[minYId];

    /* Wrap */
    do {
        size_t nextId = 0;
        Vector2dd next;
        Vector2dd nextDir(0.0);
        double vmax = -std::numeric_limits<double>::max();

        for (size_t i = 0; i < list.size(); i++)
        {
            const Vector2dd &point = list[i];

            if (point == current)
                continue;

            Vector2dd dir1 = (point - current).normalised();
            double v = direction & dir1;
            // SYNC_PRINT(("Checking asimuth %lf\n", v));
            if (v > vmax) {
                vmax = v;
                nextId =  i;
                next = point;
                nextDir = dir1;
            }
        }

        /* That is exact same point. double equality is safe */
        if (nextId == toReturn.front())
        {
            break;
        }
        // SYNC_PRINT(("ConvexHull::GiftWrap(): next point (%lf %lf) with azimuth %lf\n", next.x(), next.y(), vmax));

        toReturn.push_back(nextId);
        current = next;
        direction = nextDir;

    } while (true);
    return toReturn;
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

/* Temporary function */
int ConvexHull::initalPoint(ProjectivePolygon &points)
{
    size_t iMin = 0;
    double zMin = std::numeric_limits<double>::max();

    for (size_t i = 0; i < points.size(); i++)
    {
        double zcost = points[i].normalised() & Vector3dd::OrtZ();
        if (zcost < zMin) {
            zMin = zcost;
            iMin = i;
        }
    }
    //if (debug != NULL) debug->setColor(RGBColor::Red());
    //if (debug != NULL) debug->addIcoSphere(points[iMin].normalised(), 0.001);


    /* We have now selected direction let's check if we have lines futher away from  */
    Vector3dd orth  = Vector3dd::OrtZ() ^ points[iMin];
    Vector3dd plane = points[iMin] ^ orth; /* in euclidean coordinates this is a point */

    double cMin = std::numeric_limits<double>::max();
    for (size_t i = 0; i < points.size(); i++)
    {
        double dot = plane & points[i].normalised();
        if (dot < cMin)
        {
            cMin = dot;
            iMin = i;
        }
    }
    //if (debug != NULL) debug->setColor(RGBColor::Yellow());
    //if (debug != NULL) debug->addIcoSphere(points[iMin].normalised(), 0.001);

    // SYNC_PRINT(("ConvexHull::GiftWrap(): starting with point (%d of %d) (%lf %lf %lf)\n",
    //            (int)iMin, (int)points.size(),
    //            points[iMin].x(), points[iMin].y(), points[iMin].z()));

    return iMin;
}


int ConvexHull::secondPoint(ProjectivePolygon &points, int initalPoint)
{
    int sId = -1;

    Vector3dd prev = points[initalPoint];
    for (size_t i = 0; i < points.size(); i++)
    {
        if (i == initalPoint) {
            continue;
        }
        Vector3dd normal = prev ^ points[i];
        // cout << " " << i << " p:" << points[i] << " - " << normal << endl;

        size_t j = 0;
        for (; j < points.size(); j++)
        {
            // cout << "   " << j;
            if (j == i || j == initalPoint) {
                // cout << " skip" << endl;
                continue;
            }
            double v = (normal & points[j]);
            // cout << "  " << v << endl;
            if (v < 0) {
                break;
            }
        }
        if (j == points.size()) {
            sId = i;
            break;
        }
    }
    return sId;
}

bool ConvexHull::GiftWrap(ProjectivePolygon &points, ProjectivePolygon &output, Mesh3D *debug)
{
    output.clear();

    if (points.empty()) {
        return false;
    }
    if (points.size() == 1) {
        output.push_back(points[0]);
        return true;
    }

    /* Find one point in hull */
    size_t iMin = initalPoint(points);
    output.push_back(points[iMin]);
    Vector3dd prev = points[iMin];

    if (debug != NULL) debug->setColor(RGBColor::Amber());
    if (debug != NULL) debug->addIcoSphere(prev.normalised(), 0.001);

    /* Find second point in hull. A known interior point could save time here */
    int i2Min = secondPoint(points, iMin);
    if (i2Min == -1) {
        return false;
    }

    Vector3dd current = points[i2Min];
    output.push_back(points[i2Min]);
    if (debug) {
        SYNC_PRINT(("ConvexHull::GiftWrap(): second point (%d of %d) (%lf %lf %lf)\n",
                (int)iMin, (int)points.size(),
                current.x(), current.y(), current.z()));
    }

    /* Wrap */
    do {
        Vector3dd next = Vector3dd::Zero();
        double vmax = std::numeric_limits<double>::lowest();

        /* Search for best point */
        for (const Vector3dd &point : points)
        {
            if ((point.normalised() - current.normalised()).l2Metric() < 1e-7) {
                continue;
            }

            Vector3dd splane = current ^ prev;
            Vector3dd p = point.normalised();
            Vector3dd nplane = p ^ current;

            double v = splane.normalised() & nplane.normalised();

            // SYNC_PRINT(("(%lf %lf %lf) - %lf\n", point.x(), point.y(), point.z(), v));

            if (v > vmax)
            {
                vmax = v;
                next = point;
            }
        }

        if ((next.normalised() - output.front().normalised()).l2Metric() < 1e-7) {
            break;
        }
        // SYNC_PRINT(("ConvexHull::GiftWrap(): next point (%lf %lf %lf)\n", next.x(), next.y(), next.z()));

        prev = current;
        current = next;
        output.push_back(next);     
    } while (output.size() < 6);
    return true;
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


/** Template methods **/
/**
 *  Still under development
 **/

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

