/**
 * \file triangulation.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author eseppel
 * \author alexander
 */

#include <limits>
#include "preciseTimer.h"

#include "triangulation.h"
namespace Triangulation
{

using namespace std;

int orientation (const Point &p0, const Point &p1, const Point &p2)
{
    Point a = p1  - p0;
    Point b = p2  - p0;

    //double sa = a.x() * b.y() - b.x() * a.y();
  double sa = a.triangleOrientedAreaTo(b);

    if (sa > 0.0)
        return 1;
    else if (sa < 0.0)
        return -1;
    return 0;
}

inline Edge &Edge::flip(void)
{
  Point tmp = org;
    org = dest;
    dest = tmp;
    return *this;
}

inline Point Edge::point(const double t) const
{
    return Point(org + t * (dest - org));
}

/**
 * Having the edge rotated 90deg around the center
 **/

Edge &Edge::rot(void)
{
    Point m = 0.5 * (org + dest);
    Point v = dest - org;
    Point n(v.y(), -v.x());
    dest = m - 0.5 * n;
    org = m + 0.5 * n;

    return *this;
}

/** comparison precise up to epsilon
 *
 *
 */
inline bool doubleEqual(const double a, const double b)
{
    return (fabs(a-b) < EPSILON);
}

int Edge::intersect(const Edge &e, double &t) const
{

    Point c = e.org;
    Point d = e.dest;

    Vector2dd n = ((Vector2dd)(d - c)).rightNormal();

    double denom = n & (dest - org);

    if (/*denom == 0*/ doubleEqual(denom, 0.0))
    {
        int aclass = e.classify(org);
        if ((aclass == LEFT) || (aclass == RIGHT))
            return PARALLEL;
        else
            return COLLINEAR;
    }
    double num = n & (org - c);
    t = -num / denom;

    return SKEW;
}

inline double Edge::cosAngle(const Edge &e2)
{
    Point p0 = dest - org;
    Point p1 = e2.dest - e2.org;
    return p0.cosineTo(p1);
}

int classify(const Point &dest, const Point &org, const Point &p2)
{
    Point a = dest - org;
    Point b = p2 - org;

    double sa = a.x() * b.y() - b.x() * a.y();

    if (sa > 0.0)
        return RIGHT;
    else if (sa < 0.0)
        return LEFT;
    else if ((a.x() * b.x() < 0.0) || (a.y() * b.y() < 0.0))
        return BEHIND;
    else if (a.length() < b.length())
        return BEYOND;
    else if (org == p2)
        return ORIGIN;
    else if (dest == p2)
        return DESTINATION;
    return BETWEEN;
}

/**
 * Searches for the the closest point on the right of the edge.
 * Condition is that the circle around given 3 points should not contain other points
 *
 * */
bool mate (const Edge &e, const vector<Point> &points, Point &p)
{
    Point bestp(0,0);
        double t, bestt = std::numeric_limits<double>::max();
    Edge f = e;

    f.rot();      // f - perpendicular to the center of the segment е
    //cout << "enum {LEFT = 0,  RIGHT = 1,  BEYOND = 2,  BEHIND = 3, BETWEEN = 4, ORIGIN = 5, DESTINATION = 6};\n";
    for (size_t i = 0; i < points.size(); i++)
    {
        //cout << "    Classify: " << points[i] << " for " << e << " is: " << e.classify(points[i]) << endl;
        if (e.classify(points[i]) == RIGHT)
        {
            Edge g(e.dest, points[i]);
            g.rot();
            int intersect = f.intersect (g, t);
            //cout << "        enum {COLLINEAR = 0, PARALLEL = 1, SKEW = 2, SKEW_CROSS = 3, SKEW_NO_CROSS = 4 }\n ";
            //cout        << "intersecting: " << f << " and " << g << ". intersect = : " << intersect << ", t = " << t << endl;
            if (t < bestt && intersect == SKEW )
            {
                bestp = points[i];
                bestt = t;
            }
        }
    }

    if ((bestp.x() != 0) && (bestp.y() != 0))
    {
        p = bestp;
        return true;
    }

    return false;
}

// search of the "minimum edge" - the first edge for triangulation
Edge hullEdge (const vector<Point> &points)
{
    size_t m = 0;
    size_t i;

    for (i = 1; i < points.size(); i++)
    {
        if (points[i] < points[m])
            m = i;
    }
    size_t min = m; // "minimal point"
    m = 0;
    i = 1;

    // processing the marginal situations, when "minimal point" is one of the first ones
    if (min == 0)
    {
        m = 1;
        i = 2;
    }
    else if (min == 1)
        i = 2;

    // serching for the seconf point of the "minimum edge"
    for (; i < points.size(); i++)
    {
        if (i == min)
            continue;

        int c = classify(points[min], points[m], points[i]);
        if (c  ==  RIGHT || c  == BETWEEN)
            m = i;
    }

    return Edge(points[min], points[m]);
}

// updating the "hull"
// in attempt to add "dead" point, returns 1
int updateFrontier(set<Edge,    EdgeCmp> &frontier, const Point &a, const Point &b, set<Edge,    EdgeCmp> &died)
{
#if !defined(_MSC_VER) || (_MSC_VER > 1500)         // TODO: VS2008 doesn't compile this code!?
    Edge e(a,b);
    if (frontier.find(e) != frontier.end())
    {
        //cout << "[updateFrontier] erasing " << e << "from frontier\n";
        frontier.erase(e);
        died.insert(e);
    }
    else
    {
        e.flip();
        if (died.find(e) == died.end())
        {
            //cout << "[updateFrontier] inserting " << e << " with length: " << e.org.remoteness(e.dest)<< " to frontier\n";
            frontier.insert(e);
        }
        else
            return 1;
    }
#endif
    return 0;
}

bool isInsideTriangle(/*const vector<Point> &triangle*/
        const Edge &e1, const Edge &e2, const Edge &e3, const Point &point)
{
    //RIGHT BETWEEN ORIGIN DESTINATION
    if ( (e1.classify(point) == RIGHT || e1.classify(point) == BETWEEN || e1.classify(point) == ORIGIN) &&
             (e2.classify(point) == RIGHT || e2.classify(point) == BETWEEN || e2.classify(point) == ORIGIN) &&
             (e3.classify(point) == RIGHT || e3.classify(point) == BETWEEN || e3.classify(point) == ORIGIN))
        return true;
    return false;
}

Point findTriangleCenter(const Point &p1, const Point &p2, const Point &p3)
{
    Point result = (p1 + p2 + p3) * (1.0/3.0);
    double r1 = result.remoteness(p1);
    double r2 = result.remoteness(p2);
    double r3 = result.remoteness(p3);
    double r0 = r1 + r2 + r3;

    //result.color() = (p1.color() + p2.color() + p3.color())/3.0;
    result.color() = p1.color() * r1/r0 + p2.color() * r2/r0 + p3.color() * r3/r0;
    //cout << "Color1: " << p1.color() << " Color2: " << p2.color() << " Color3: " << p3.color()
    //        << " Result: " << result.color() << endl;
    return result;
}

//////////////////////////////////////////////////////////////////////////////////////
/*std::vector<Edge>    triangulate2(const vector<Point> &points)
{
    set<Edge,    EdgeCmp> edgeMap;
    std::vector<Edge> triangResult = triangulate(points);
    for (vector<Edge>::const_iterator i = triangResult.begin(); i != triangResult.end(); i++)
    {
        // normalize Edge
        if (i->org > i->dest)
            edgeMap.insert(Edge(i->dest, i->org));
        else
            edgeMap.insert(*i);
    }

    int j = 0;
    std::vector<Edge> result(edgeMap.size());
    for (set<Edge,    EdgeCmp>::const_iterator i = edgeMap.begin(); i != edgeMap.end(); i++)
    {
        //if (i->org.remoteness(i->dest) < 300.0)
            result[j++] = *i;
    }

    return result;
}*/

#ifndef FAST_SPLIT
static void splitPoints(const vector<Point> &points, TriangulationBeginnings &tb, ListOfLists &listOfLists,
                        const int maxDistance)
{
    list<Point> lpoints;
    for (vector<Point>::const_iterator i = points.begin(); i != points.end(); i++)
        lpoints.push_back(*i);

    // Creating separate groups to build triangulation for
    // O(N^2)
    while (!lpoints.empty())
    {
        Point current = *lpoints.begin();
        lpoints.pop_front();
        //cout << "Current: " << current << endl;

        {
            list<Point> currentList;
            currentList.push_back(current);
            listOfLists.push_back(currentList);
        }

        for (Lpci point = listOfLists.back().begin(); point != listOfLists.back().end(); point++)
        {
            //cout << " Current Point: " << *point << endl;
            for (Lpi secondPoint = lpoints.begin(); secondPoint != lpoints.end();)
            {
                //cout << "  <Point: " << *secondPoint << ">\n";     cout.flush();

        if (point->remoteness(*secondPoint) < maxDistance)
                {
                    listOfLists.back().push_back(*secondPoint);
                    //cout << "   Adding point " << *secondPoint << "and len is: " << point->remoteness(*secondPoint) <<  endl;
                    Lpi tmpPoint = secondPoint;
                    secondPoint++;
                    lpoints.erase(tmpPoint);
                }
                else
                    secondPoint++;
            }
        }
    }

    tb.numOfGroups = listOfLists.size();
    tb.hullEdges.resize(tb.numOfGroups);
}

std::vector<Edge> triangulate4(const vector<Point> &points, list<Point> &centers, TriangulationBeginnings &tb,
                               const int maxDistance)
{
    ListOfLists listOfLists;
    //uint64_t microseconds;
    //PreciseTimer timer = PreciseTimer::currentTime();

    cout << "Using slow split algorithm\n";

    // Creating separate groups to build triangulation for
    //cerr << "-----splitPoints started-----\n";
  splitPoints(points, tb, listOfLists, maxDistance);
    //cerr << "-----splitPoints finished-----\n";
    //microseconds = timer.usecsTo(PreciseTimer::currentTime());
    //cerr << "splitPoints Real time: " << microseconds << " microseconds, " << microseconds/1000000.0 << " secs\n";
    //return vector<Edge>();

    cout << "(triangulate4) Points: " << points.size() << endl;

    size_t countOfEdges = 0;

    // array of points to the triangulation
    vector<Edge> *edges = new vector<Edge> [listOfLists.size()];

    size_t t = 0;
    for (ListOfLists::const_iterator i = listOfLists.begin(); i != listOfLists.end(); i++, t++)
    {
        //cout << "    size:" << i->size() << endl;
        //all += i->size();

        vector<Point> newpoints(i->size());
        int k = 0;
        for (Lpci j = i->begin(); j != i->end(); j++)
            newpoints[k++] = *j;

        edges[t] = TRIANGULATE(newpoints);

        //edges[t] = vector<Edge>();

        if (edges[t].size() != 0)
            tb.hullEdges[t] = countOfEdges;
        else
            tb.hullEdges[t] = -1;

        countOfEdges += edges[t].size();
    }

    //cout << "Total edges: " << countOfEdges << endl;

    vector<Edge> result(countOfEdges);

    for (size_t l = 0, currentEdgesArray = 0, currentEdgesIndex = 0;
             currentEdgesArray < listOfLists.size() && l < countOfEdges;
             currentEdgesIndex++)
    {
        if (currentEdgesIndex >= edges[currentEdgesArray].size())
        {
      //cout << "!!! currentEdgesIndex:" << currentEdgesIndex << " currentEdgesArray:" << currentEdgesArray << endl;
            currentEdgesIndex = -1;
            currentEdgesArray++;
            continue;
        }
        result[l++] = edges[currentEdgesArray][currentEdgesIndex];
    }

    for (vector<Edge>::const_iterator it = result.begin(); it != result.end(); it += 3)
        centers.push_back(findTriangleCenter(it->org, it->dest,
                                             (it+1)->dest == it->dest ? (it+1)->org : (it+1)->dest));

    delete [] edges;

    // reterning the result with repetition of edegs
    return result;
}

#else
static void splitPoints(const vector<Point> &points, TriangulationBeginnings &tb, ListOfLists &listOfLists,
                        const int maxDistance)
{
    list<Point> lpoints;
    for (vector<Point>::const_iterator i = points.begin(); i != points.end(); i++)
        lpoints.push_back(*i);

    // O(N^2)
    while (!lpoints.empty())
    {
        Point current = *lpoints.begin();
        lpoints.pop_front();
        //cout << "Current: " << current << endl;

        {
            PointGroup currentList;
            currentList.push_back(current);
            listOfLists.push_back(currentList);
        }

        for(Lpi i = lpoints.begin(); i != lpoints.end();)
        {
      if (listOfLists.back().isNear(*i, maxDistance))
            {
                listOfLists.back().push_back(*i);
                Lpi tmpPoint = i;
                i++;
                lpoints.erase(tmpPoint);
            }
            else
                i++;
        }
    }

    tb.numOfGroups = (int)listOfLists.size();
    tb.hullEdges.resize(tb.numOfGroups);
}

std::vector<Edge>    triangulate4(const vector<Point> &points, list<Point> &centers, TriangulationBeginnings &tb,
                               const int maxDistance)
{
    ListOfLists listOfLists;
  //uint64_t microseconds;
  //PreciseTimer timer = PreciseTimer::currentTime();

  cout << "Using fast split algorithm\n";

  //cerr << "-----splitPoints started-----\n";
  splitPoints(points, tb, listOfLists, maxDistance);
  //cerr << "-----splitPoints finished-----\n";
  //microseconds = timer.usecsTo(PreciseTimer::currentTime());
  //cerr << "splitPoints Real time: " << microseconds << " microseconds, " << microseconds/1000000.0 << " secs\n";
    //return vector<Edge>();

    cout << "(triangulate4) Points: " << points.size() << std::endl;

    size_t countOfEdges = 0;

    vector<Edge> *edges = new vector<Edge> [listOfLists.size()];

    size_t t = 0;
    for (ListOfLists::const_iterator i = listOfLists.begin(); i != listOfLists.end(); i++, t++)
    {
    //cout << "    size:" << i->mList.size() << endl;
        //all += i->size();

    vector<Point> newpoints(i->mList.size());
        int k = 0;
    for (Lpci j = i->mList.begin(); j != i->mList.end(); j++)
            newpoints[k++] = *j;

    edges[t] = TRIANGULATE(newpoints);
        //edges[t] = vector<Edge>();

        if (edges[t].size() != 0)
            tb.hullEdges[t] = (int)countOfEdges;
        else
            tb.hullEdges[t] = -1;

        countOfEdges += edges[t].size();
    }

    cout << "Total edges: " << countOfEdges << std::endl;

    vector<Edge> result(countOfEdges);

    for (size_t l = 0, currentEdgesArray = 0, currentEdgesIndex = 0;
             currentEdgesArray < listOfLists.size() && l < countOfEdges;
             currentEdgesIndex++)
    {
        if (currentEdgesIndex >= edges[currentEdgesArray].size())
        {
      //cout << "!!! currentEdgesIndex:" << currentEdgesIndex << " currentEdgesArray:" << currentEdgesArray << endl;
            currentEdgesIndex = -1;
            currentEdgesArray++;
            continue;
        }
        result[l++] = edges[currentEdgesArray][currentEdgesIndex];
    }

  for (vector<Edge>::const_iterator it = result.begin(); it != result.end(); it += 3)
    centers.push_back(findTriangleCenter(it->org, it->dest,
      (it+1)->dest == it->dest ? (it+1)->org : (it+1)->dest));

    delete [] edges;


    return result;
}
#endif

void printPoints(const vector<Point> &points)
{
    cout << "printPoints: ";
    for (vector<Point>::const_iterator i = points.begin(); i != points.end(); i++)
        cout << *i << std::endl;
    cout << std::endl;
}

std::vector<Edge>    triangulate(const vector<Point> &points)
{
    //printPoints(points);
    Point p;
    set<Edge,    EdgeCmp> frontier;
    set<Edge,    EdgeCmp> died;
    list<vector<Point> > triangles;
    cout << "Using simple triangulation\n";

    if (points.size() < 3)
    {
        cout << "(triangulate): No enought data!\n";
        return std::vector<Edge>();
    }

    Edge e = hullEdge(points);
    //cout << "Hull Edge: " << e << endl;
    frontier.insert(e);

    while (!frontier.empty())
    {
        e = *(frontier.begin());
        frontier.erase(e);

        if (mate(e, points, p))
        {
            //cout << "!!!Found mate for " << e << " is   " << p << endl;
            died.insert(e);
            int r = updateFrontier(frontier, p, e.org, died);
            r +=    updateFrontier(frontier, e.dest, p, died);

            //assert (r == 0);
            if (!r)
            {
                vector<Point> triangle(3);
                triangle[0] = e.org;
                triangle[1] = e.dest;
                triangle[2] = p;
            //cout << "    New triangle: " <<p << ", " << e.org << ", " << e.dest << endl;
            triangles.push_back(triangle);
             }
        }
    }

    std::vector<Edge> newEdges(triangles.size() * 3);
    //list<Edge> newEdges();


    //  cout << "Returning triangles\n";
    int j = 0;
    for (list<vector<Point> >::const_iterator i = triangles.begin(); i != triangles.end(); i++)
    {
        /*newEdges.push_back(Edge((*i)[0], (*i)[1]));
        newEdges.push_back(Edge((*i)[1], (*i)[2]));
        newEdges.push_back(Edge((*i)[2], (*i)[0]));*/
        newEdges[j].org = (*i)[0];
        newEdges[j++].dest = (*i)[1];
        newEdges[j].org = (*i)[1];
        newEdges[j++].dest = (*i)[2];
        newEdges[j].org = (*i)[2];
        newEdges[j++].dest = (*i)[0];
    }

    cout << "Triangles:" << triangles.size() << " Edges: " << newEdges.size() << " Points: " << points.size() << endl;
    return newEdges;
}

std::vector<Edge> vertexHull(const vector<Point> &points)
{
    if (points.size() < 3)
    {
        cout << "No enought data!\n";
        return std::vector<Edge>();
    }

    list<Point> tmp;
    TriangulationBeginnings tb;

    std::vector<Edge> triangulations = triangulate4(points, tmp, tb);
    return vertexHullFromTriangulation(triangulations, (int)points.size(), tb);
}

std::vector<Edge> vertexHullFromTriangulation(const std::vector<Edge> &edges, const int pointsCount,
                                              const TriangulationBeginnings &tb)
{
    int i = 0;
    std::vector<Edge> result(pointsCount);
    multimap<Point, Edge> edgeList;
    multimap<Point, Edge>::const_iterator it;
    pair<multimap<Point, Edge>::iterator, multimap<Point, Edge>::iterator> ret;

    // creating data structure in which it is easy to find the edges originating from a given point
    for (vector<Edge>::const_iterator i = edges.begin() + 1; i != edges.end(); i++)
        edgeList.insert(make_pair(i->org, *i));


    //cout << "========================\n";
    Point point0 = edges[0].org;
    //cout << "Point0 :" << point0 << " Point1 :" << edges[0].dest << endl;


    Edge cur;
    bool firstIteration = true;
    int groupCounter = -1;

    //cout << "tb.numOfGroups: " << tb.numOfGroups << endl;

    while (groupCounter < tb.numOfGroups)
    {
        if (cur.dest == point0 || firstIteration)
        {
            firstIteration = false;
            groupCounter ++;

            if (groupCounter == tb.numOfGroups)
                break;

            if (tb.hullEdges[groupCounter] == -1)
            {
                groupCounter++;
                continue;
            }

            //cout << "|||Next group : " << tb.hullEdges[groupCounter] << endl;
            cur.org = point0 = edges[tb.hullEdges[groupCounter]].org;
            cur.dest = edges[tb.hullEdges[groupCounter]].dest;
            result[i++] = cur;
            continue;
        }

        // searching all outgoing edges form the given point
        ret = edgeList.equal_range(cur.dest);
        //cout << "    " << cur.dest << ":::: \n";
        double maxAngle = -1.1;
        Edge maxEdge;
        for (it = ret.first; it!=ret.second; ++it)
        {
            //     cout << "        " << (*it).second << endl;
                 double angle = cur.cosAngle((*it).second); // cosinus
                 if (angle > maxAngle)
                 {
                     maxAngle = angle;
                     maxEdge = (*it).second;
                 }
        }
        //cout << "    maxAngle: " << maxAngle << " maxEdge: " << maxEdge << endl;
        cur = maxEdge;
        result[i++] = maxEdge;
    }

    return result;
}

}


