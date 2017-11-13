#ifndef TRIANGULATION_H_
#define TRIANGULATION_H_
/**
 * \file triangulation.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author eseppel
 * \author alexander
 */


#include "core/utils/global.h"

#include <math.h>
#include <assert.h>
#include <limits>
#include <list>
#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <map>

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"

// allow of using "Fast" split algorithm which is better for huge ammount of points
#define FAST_SPLIT
// allow of using GTS triangulation

#ifdef USE_GTS
#define FAST_TRIANGULATION
#endif

namespace Triangulation
{

using std::list;
using std::set;

using namespace corecvs;

const double DEFAULT_MAX_DISTANCE = 200.0;
const double EPSILON = 0.00001;

//typedef Vector2dd Point;

typedef double ElementType;
class Point: public Vector2d<ElementType>
{
    typedef Vector2d<ElementType> BaseClass;
private:
    int mColor;
    static const int defaultColor = 0;

public:
    Point() :
        BaseClass()
      , mColor(defaultColor)
    {
    }

    Point(const Point &p) :
        BaseClass(p)
      , mColor(p.color())
    {
    }

    Point(const BaseClass &p) :
            BaseClass(p), mColor(defaultColor)
    {
    }
    Point(const BaseClass::BaseClass &p) :
            BaseClass(p), mColor(defaultColor)
    {
    }
    Point(const ElementType x, const ElementType y) :
            BaseClass(x, y), mColor(defaultColor)
    {
    }
    Point(const ElementType x, const ElementType y, const int color) :
            BaseClass(x, y), mColor(color)
    {
    }

    /*inline operator Point() const
     {
     return Point((*this)[0],(*this)[1], _color);
     }*/

    // In case you need to collect some stats
    inline Point& operator =(const Point &p)
    {
        this->x() = p.x();
        this->y() = p.y();
        this->mColor = p.color();
        return *this;
    }


    friend ostream & operator <<(ostream &out, const Point &point)
    {
        //out.precision(20);
        //out << "points[j++] = Point(";
        out << "[";
        for (int i = 0; i < 2; i++)
            out << (i == 0 ? "" : ", ") << point[i];
        out /*<< "; " << point.color()*//*<< ");\n"*/<< "]";
        return out;
    }

    inline int operator<(const Point &p) const
    {
        return ((x() < p.x()) || ((x() == p.x()) && (y() < p.y())));
    }
    inline int operator>(const Point &p) const
    {
        return ((x() > p.x()) || ((x() == p.x()) && (y() > p.y())));
    }

    ElementType length() const
    {
        return getLengthStable();
    }

    inline ElementType remoteness(const Point &dest) const
    {
        return !((Vector2d<double> ) (dest - *this));
        //return sqrt((dest.x() - x()) * (dest.x() - x()) + (dest.y() - y()) * (dest.y() - y()));
    }

    inline int &color()
    {
        return mColor;
    }
    inline const int &color() const
    {
        return mColor;
    }

    inline int red() const
    {
        return (mColor >> 16) & 0xFF;
    }
    inline int green() const
    {
        return (mColor >> 8) & 0xFF;
    }
    inline int blue() const
    {
        return (mColor) & 0xFF;
    }
};

/** Position of the point with respect to the directed edge */
enum
{
    LEFT = 0, /**< Point is to the left of the edge */
    RIGHT = 1, /**< Point is to the right of the edge */
    BEYOND = 2, /**< Point is on the same line as the edge but beyond the end */
    BEHIND = 3, /**< Point is on the same line as the edge but before the start */
    BETWEEN = 4, /**< The point is on the edge */
    ORIGIN = 5, /**< The point is the origin */
    DESTINATION = 6
/**< The point is the destination */
};

enum
{
    COLLINEAR = 0, PARALLEL = 1, SKEW = 2, SKEW_CROSS = 3, SKEW_NO_CROSS = 4
};

int classify(const Point &dest, const Point &org, const Point &p2);

class Edge
{
public:
    Point org;
    Point dest;

    Edge(const Point &_org, const Point &_dest) :
            org(_org), dest(_dest)
    {
    }
    Edge(void) :
            org(Point(0, 0)), dest(Point(0, 0))
    {
    }
    Edge &rot(void);
    inline Edge &flip(void);
    inline Point point(const double) const;
    int intersect(const Edge&, double&) const;
    inline double cosAngle(const Edge &e2);

    inline int classify(const Point &p2) const
    {
        return Triangulation::classify(dest, org, p2);
    }

    friend ostream & operator <<(ostream &out, const Edge &edge)
    {
        out << "{" << edge.org << "=>" << edge.dest << "}";
        return out;
    }

    inline bool operator ==(const Edge &e1) const
    {
        if (org == e1.org && dest == e1.dest)
            return true;
        return false;
    }

    // TODO: в .cpp
    /*bool tooFar(const Point &p) const
     {
     const double boundary = MAX_LENGTH;
     if (org.remoteness(p) > boundary && dest.remoteness(p) > boundary)
     return true;
     return false;
     }*/
};

/*struct Triangle
 {
 Vector3d32 p1;
 Vector3d32 p2;
 Vector3d32 p3;
 };*/

/*struct Polinom
 {
 vector<Vector3d32> point;
 };*/

struct TriangulationBeginnings
{
    int numOfGroups;
    std::vector<int> hullEdges;
};

struct EdgeCmp
{
    // Strict_weak_ordering
    bool operator()(const Edge &a, const Edge &b)
    {
        if (a.org < b.org)
            return true;
        if (a.org > b.org)
            return false;
        // if origins are equal
        if (a.dest < b.dest)
            return true;
        if (a.dest > b.dest)
            return false;

        return false;
    }
};

typedef list<Point>::const_iterator Lpci;
typedef list<Point>::iterator Lpi;

struct PointGroup
{
public:
    double minX, minY;
    double maxX, maxY;
    list<Point> mList;

    PointGroup(/*const double _maxLength = MAX_LENGTH*/) :
            minX(std::numeric_limits<double>::max()), minY(std::numeric_limits<double>::max()), maxX(
                    -std::numeric_limits<double>::max()), maxY(-std::numeric_limits<double>::max())
    {
    }

    inline void push_back(const Point &newPoint)
    {
        mList.push_back(newPoint);

        if (newPoint.x() > maxX)
            maxX = newPoint.x();
        if (newPoint.y() > maxY)
            maxY = newPoint.y();
        if (newPoint.x() < minX)
            minX = newPoint.x();
        if (newPoint.y() < minY)
            minY = newPoint.y();
    }

    inline bool isNear(const Point &target, const int maxDistance) const
    {
        if (target.x() > maxX + maxDistance || target.x() < minX - maxDistance
                || target.y() > maxY + maxDistance || target.y() < minY - maxDistance)
        {
            //cout << "quick found works\n";
            return false;
        }

        for (Lpci secondPoint = mList.begin(); secondPoint != mList.end(); secondPoint++)
        {
            if (target.remoteness(*secondPoint) < maxDistance)
                return true;
        }
        return false;
    }
};

#ifndef FAST_SPLIT
typedef list<list<Point> > ListOfLists;
#else
typedef list<PointGroup> ListOfLists;
#endif

inline bool doubleEqual(const double a, const double b);
int updateFrontier(set<Edge, EdgeCmp> &frontier, const Point &a, const Point &b
        , set<Edge, EdgeCmp> &died);
void printPoints(const vector<Point> &points);

bool mate(const Edge &e, const vector<Point> &points, Point &p);
int orientation(const Point &pO, const Point &pl, const Point &p2);
std::vector<Edge> vertexHullFromTriangulation(const std::vector<Edge> &edges, const int pointsCount,
        const TriangulationBeginnings &tb);

Point findTriangleCenter(const Point &p1, const Point &p2, const Point &p3);

inline bool isInsideTriangle(const Edge &e1, const Edge &e2, const Edge &e3, const Point &point);

std::vector<Edge> triangulate(const vector<Point> &points); // with repetitions
//std::vector<Edge>    triangulate2(const vector<Point> &points); // without repetitions
std::vector<Edge> triangulate4(const vector<Point> &points, list<Point> &centers,
        TriangulationBeginnings &triangulationBeginnings, const int maxDistance =
                DEFAULT_MAX_DISTANCE);
std::vector<Edge> vertexHull(const vector<Point> &points);

#ifdef FAST_TRIANGULATION
#include "../../wrappers/gts/gtswrapper.h"
#define TRIANGULATE triangulateGts
#else
#define TRIANGULATE triangulate

#endif

} // end namespace Triangualtion

#endif // TRIANGULATION_H_

