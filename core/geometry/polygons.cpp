/**
 * \file polygons.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 14, 2010
 * \author alexander
 */

#include "core/geometry/polygons.h"
#include "core/math/mathUtils.h"
#include "core/utils/global.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/abstractPainter.h"

namespace corecvs {

/**
 *
 * Check if point is inside the polygon
 **/
int Polygon::isInsideConvex(const Vector2dd &a) const
{
    double oldsign = 0;
    int len = (int)size();
    for (int i = 0; i < len; i++)
    {
        const Vector2dd &curr = getPoint(i);
        const Vector2dd &next = getNextPoint(i);
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

/**
 *  Using idea from  http://geomalgorithms.com/a03-_inclusion.html
 **/
int Polygon::windingNumber(const Vector2dd &point) const
{
    int    windingNumber = 0;

    for (int i = 0; i < (int)size(); i++)
    {
        Vector2dd cur  = getPoint(i);
        Vector2dd next = getNextPoint(i);
        Vector2dd dir  = next - cur;

        bool startsNotAbove = (cur.y() <= point.y());
        bool startsAbove    = !startsNotAbove;

        bool endsAbove      = (next.y() > point.y());
        bool endsNotAbove   = !endsAbove;


        if (startsNotAbove && endsAbove) { // Crossing up
            if (dir.parallelogramOrientedAreaTo(point - next) > 0)
                 windingNumber++;
        }

        if (startsAbove && endsNotAbove)  { // Crossing down
            if (dir.parallelogramOrientedAreaTo(point - next) < 0)
                 windingNumber--;
        }
    }
    return windingNumber;
}

int  Polygon::isInside(const Vector2dd &point) const
{
    return windingNumber(point) != 0;
}

bool Polygon::isConvex(bool *direction) const
{
    double oldsign = 0;

    const Vector2dd *curr = &getPoint(0);
    int idx1 = getNextDifferentIndex(0);
    if (idx1 == 0)
        return true;

    const Vector2dd *next = &getPoint(idx1);

    int idx2 = getNextIndex(idx1);
    const Vector2dd *nnext = NULL;

    //cout << "We have taken as first ones: " << *curr << " " << *next << endl;

    while (idx2 != idx1)
    {
        double sign = 0.0;

        nnext = &getPoint(idx2);
        sign = (*next - *curr).rightNormal() & (*nnext - *next);

        if (sign != 0.0) {
            if (oldsign > 0  && sign < 0)
                return false;
            if (oldsign < 0  && sign > 0)
                return false;
            oldsign = sign;

            curr = next;
            next = nnext;
        }
        idx2 = getNextIndex(idx2);
    }

    if (direction != NULL) {
        *direction =  (oldsign > 0);
    }
    return true;
}

/** Rewrite this with sweep line **/
bool Polygon::hasSelfIntersection() const
{
    size_t len = size();
    for (size_t i = 0; i < len; i++)
    {
       Segment2d s1 = getSegment((int)i);
       for (size_t j = i + 2; j < len - 1; j++)
       {
          Segment2d s2 = getSegment((int)j);
          bool intersect;
          Segment2d::intersect(s1, s2, intersect);

          //cout << "Polygon::hasSelfIntersection():" << s1 << " and " << s2 << (intersect ? " y" : " n") << endl;

          if (intersect)
              return true;
       }
    }
    return false;
}

Polygon Polygon::RegularPolygon(int sides, const Vector2dd &center, double radius, double startAngleRad)
{
    Polygon toReturn;
    toReturn.reserve(sides);

    double step = degToRad(360.0 / sides);

    for (int i = 0; i < sides; i++)
    {
        toReturn.emplace_back(center + Vector2dd::FromPolar(step * i + startAngleRad, radius));
    }
    return toReturn;
}

Polygon Polygon::Reverse(const Polygon &p)
{
    Polygon toReturn;
    toReturn.reserve(p.size());

    for (auto ri = p.rbegin(); ri != p.rend(); ri++)
    {
        toReturn.emplace_back(*ri);
    }
    return toReturn;
}

Polygon Polygon::FromConvexPolygon(const ConvexPolygon &polygon)
{
    vector<Vector2dd> points;
    for (size_t i = 0; i < polygon.faces.size(); i++)
    {
        for (size_t j = i + 1; j < polygon.faces.size(); j++)
        {
            Line2d in1 = polygon.faces[i];
            Line2d in2 = polygon.faces[j];

            bool hasIntersection = false;
            Vector2dd intersect = in1.intersectWith(in2, &hasIntersection);
            if (!hasIntersection) {
                continue;
            }

            bool inside = true;
            for (size_t k = 0; k < polygon.faces.size(); k++)
            {
                if (k == i || k == j)
                    continue;

                if (polygon.faces[k].pointWeight(intersect) < 0) {
                    inside = false;
                    break;
                }
            }
            if (inside) {
                points.emplace_back(intersect);
            }
        }
    }
    return ConvexHull::GrahamScan(points);
}


/* Sweep line makes this much faster, we need to implement it someday */
Polygon Polygon::FromHalfplanes(const std::vector<Line2d> &halfplanes)
{
    ConvexPolygon polygon;
    polygon.faces = halfplanes;
    return Polygon::FromConvexPolygon(polygon);
}

Polygon Polygon::FromImageSize(const Vector2d<int> &size)
{
    Polygon toReturn;
    toReturn.push_back(Vector2dd(       0,        0));
    toReturn.push_back(Vector2dd(size.x(),        0));
    toReturn.push_back(Vector2dd(size.x(), size.y()));
    toReturn.push_back(Vector2dd(       0, size.y()));
    return toReturn;
}

ConvexPolygon Polygon::toConvexPolygon() const
{
    ConvexPolygon result;
    result.faces.reserve(size());

    for (size_t i = 0; i < size(); i++)
    {
        result.faces.emplace_back(Line2d(Segment2d(getSegment((int)i))));
    }

    return result;
}

double Polygon::signedArea()
{
    Vector2dd c = center();
    double sum = 0.0;
    int len = (int)size();

    for (int i = 0; i < len; i++)
    {
        const Vector2dd &curr = operator [](i);
        const Vector2dd &next = operator []((i + 1) % len);

        Vector2dd v1 = curr - c;
        Vector2dd v2 = next - c;

        double part = v1.triangleOrientedAreaTo(v2);
        sum += part;
    }
    return sum;
}

void PolygonCombiner::prepare()
{        
    intersectionNumber = 0;
    intersections.clear();
    c[0].clear();
    c[1].clear();

    for (int p = 0; p < 2; p++)
    {
        if (pol[p].signedArea() > 0) pol[p] = Polygon::Reverse(pol[p]);
    }

    for (int p = 0; p < 2; p++)
    {
        for (size_t i = 0; i < pol[p].size(); i++) {
            Vector2dd point = pol[p][i];
            VertexData vd = {i, point, (pol[1-p].isInside(point) ? INSIDE : OUTSIDE), 0};
            c[p].push_back(vd);
        }
    }

    for (int i = 0; i < (int)pol[0].size(); i++)
    {
        for (int j = 0; j < (int)pol[1].size(); j++)
        {
            Ray2d r1 = pol[0].getRay(i);
            Ray2d r2 = pol[1].getRay(j);
            /* Could make a fast check here before any divisions */

            double t1 = 0;
            double t2 = 0;
            Vector2dd x = Ray2d::intersection(r1, r2, t1, t2);

            if (t1 == std::numeric_limits<double>::infinity())
            {
                /**
                 * Sides are parallel. We assume nothing is to be done so far.
                 **/
                continue;
            }


            if ((t1 < 0.0 || t1 > 1.0) ||
                (t2 < 0.0 || t2 > 1.0))
            {
                continue;
            }

            VertexData vd1(i, x, COMMON, t1, intersectionNumber);
            c[0].push_back(vd1);

            VertexData vd2(j, x, COMMON, t2,  intersectionNumber);
            c[1].push_back(vd2);

            intersectionNumber++;
        }
    }

    auto comparator = [](VertexData &vd1, VertexData &vd2)
    {
            if (vd1.orgId == vd2.orgId)
                return vd1.t < vd2.t;
            return vd1.orgId < vd2.orgId;
    };

    std::sort(c[0].begin(), c[0].end(), comparator); // TODO: First array can be created presorted
    std::sort(c[1].begin(), c[1].end(), comparator);

    intersections.resize(intersectionNumber);
    for (size_t i = 0; i < c[0].size(); i++)
    {
        if (c[0][i].flag == COMMON)
            intersections[c[0][i].intersection].first = i;
    }
    for (size_t i = 0; i < c[1].size(); i++)
    {
        if (c[1][i].flag == COMMON)
            intersections[c[1][i].intersection].second = i;
    }
    for (size_t i = 0; i < intersections.size(); i++)
    {
        c[0][intersections[i].first ].other = intersections[i].second;
        c[1][intersections[i].second].other = intersections[i].first;
    }
}

bool PolygonCombiner::validateState() const
{
    bool ok = true;
    for (int p = 0; p < 2; p++)
    {
        size_t start = -1;
        size_t oldId = -1;

        int skipped = 0;

        VertexData vc = c[p][0];

        for (size_t i = 0; i < c[p].size(); i++)
        {
            vc = c[p][i];
            if (vc.flag == INSIDE || vc.flag == OUTSIDE)
            {
                start = i;
                break;
            }
        }

        if (start == (size_t)-1)
        {
            cout << "No inside or outside nodes... " << endl;
            ok = false;
        }

        oldId = start;

        for (size_t i = 0; i <= c[p].size(); i++)
        {
            int num = (int)((i + start) % c[p].size());

            const VertexData &vn = c[p][num];
            if (vn.flag == COMMON)
            {
                skipped++;
                continue;
            }

            if (vn.flag == INSIDE  && vc.flag == OUTSIDE)
            {
                if (skipped % 2 != 1) {
                    printf("We have a problematic span [%c%d - out %c%d - in]: crossings %d\n", p == 0 ? 'A' : 'B', (int)oldId, p == 0 ? 'A' : 'B', num, skipped);
                    ok = false;
                }
            }

            if (vn.flag == OUTSIDE && vc.flag == INSIDE)
            {
                if (skipped % 2 != 1) {
                    printf("We have a problematic span [%c%d - in %c%d - out]: crossings %d\n", p == 0 ? 'A' : 'B', (int)oldId, p == 0 ? 'A' : 'B', num, skipped);
                    ok = false;
                }
            }

            if (vn.flag == INSIDE  && vc.flag == INSIDE)
            {
                if (skipped % 2 == 1) {
                    printf("We have a problematic span [%c%d - in %c%d - in] : crossings %d\n", p == 0 ? 'A' : 'B', (int)oldId, p == 0 ? 'A' : 'B', num, skipped);
                    ok = false;
                }
            }

            if (vn.flag == OUTSIDE && vc.flag == OUTSIDE)
            {
                if (skipped % 2 == 1) {
                    printf("We have a problematic span [%c%d - out %c%d - out] : crossings %d\n", p == 0 ? 'A' : 'B', (int)oldId, p == 0 ? 'A' : 'B', num, skipped);
                    ok = false;
                }
            }

            oldId = num;
            skipped = 0;
            vc = vn;
        }
    }
    return ok;
}

void PolygonCombiner::drawDebug(RGB24Buffer *buffer) const
{
    AbstractPainter<RGB24Buffer> painter(buffer);

    painter.drawPolygon(pol[0], RGBColor::Yellow());
    painter.drawPolygon(pol[1], RGBColor::Cyan  ());

    for (int p = 0; p < 2; p++)
    {
        for (int i = 0; i < (int)c[p].size(); i++)
        {
            const VertexData &v = c[p][i];
            Vector2dd pos = v.pos;

            if (v.flag == INSIDE)
                buffer->drawCrosshare3(pos.x(), pos.y(), RGBColor::Red());
            if (v.flag == OUTSIDE)
                buffer->drawCrosshare3(pos.x(), pos.y(), RGBColor::Green());
            if (v.flag == COMMON)
                buffer->drawCrosshare3(pos.x(), pos.y(), RGBColor::Blue());

            painter.drawFormat(pos.x(), pos.y() + p * 10, RGBColor::White(), 1, "%c%d (%0.2lf) [%d]", p == 0 ? 'A' : 'B', i, v.t, v.other);
            printf("(%lf %lf) %c%d (%0.2lf) [%" PRISIZE_T "]\n", pos.x(), pos.y() , p == 0 ? 'A' : 'B', i, v.t, v.other);
        }
    }
}

Rectangled PolygonCombiner::getDebugRectangle() const
{
    Rectangled r = Rectangled::Empty();

    for (int p = 0; p < 2; p++)
    {
        for (int i = 0; i < (int)c[p].size(); i++)
        {
            const VertexData &v = c[p][i];
            Vector2dd pos = v.pos;
            r.extendToFit(pos);
        }
    }
    cout << "Overall rectangle:" << r << endl;
    return r;
}

void PolygonCombiner::drawDebugAutoscale(RGB24Buffer *buffer, int margin) const
{
    Rectangled r = getDebugRectangle();

    Matrix33 transform =   Matrix33::Scale2((buffer->w - 2 * margin) / r.size.x(), (buffer->h - 2 * margin) / r.size.y()) * Matrix33::ShiftProj(-r.corner.x(), -r.corner.y());;
    transform = Matrix33::ShiftProj(margin, margin) * transform;

    AbstractPainter<RGB24Buffer> painter(buffer);

    for (int p = 0; p < 2; p++)
    {

       /*for (int i = 0; i < (int)c[p].size(); i++)
        {
            Vector2dd v1 = transform * pol[p].getPoint(i);
            Vector2dd v2 = transform * pol[p].getNextPoint(i);

            buffer->drawLine(v1, v2, p == 0 ? RGBColor::Yellow() : RGBColor::Cyan());
        }*/

        for (int i = 0; i < (int)c[p].size(); i++)
        {
            const VertexData &v = c[p][i];
            Vector2dd posOrig = v.pos;
            Vector2dd pos = transform * posOrig;

            if (v.flag == INSIDE)
                buffer->drawCrosshare3(pos.x(), pos.y(), RGBColor::Red());
            if (v.flag == OUTSIDE)
                buffer->drawCrosshare3(pos.x(), pos.y(), RGBColor::Green());
            if (v.flag == COMMON)
                buffer->drawCrosshare3(pos.x(), pos.y(), RGBColor::Blue());

            painter.drawFormat(pos.x(), pos.y() + p * 10, RGBColor::White(), 1, "%c%d (%0.2lf) [%d]", p == 0 ? 'A' : 'B', i, v.t, v.other);
            printf("(%lf %lf) %c%d (%0.2lf) [%" PRISIZE_T "]\n", posOrig.x(), posOrig.y() , p == 0 ? 'A' : 'B', i, v.t, v.other);
        }
    }
}



Polygon PolygonCombiner::followContour(int startIntersection, bool inner, vector<bool> *visited) const
{
    Polygon result;

    if (trace) SYNC_PRINT(("PolygonCombiner::followContour(%d, %s)\n", startIntersection, inner ? "inner" : "outer"));

    VertexType flagToFollow = inner ?  INSIDE : OUTSIDE;
    VertexType flagToAvoid  = inner ? OUTSIDE : INSIDE ;


    const std::pair<size_t, size_t> &fst = intersections[startIntersection];

    size_t currentId = fst.first;
    size_t currentChain = 0;

    if (trace) SYNC_PRINT(("Exit condition A%d or B%d\n", (int)fst.first, (int)fst.second));

    int limit = 0;
    while (limit ++ < 30 /*true*/) {
        VertexData v = c[currentChain][currentId];
        if ((visited != NULL) && (v.flag == COMMON)) {
            visited->operator [](v.intersection) = true;
        }

        result.push_back(v.pos);
        if (trace) SYNC_PRINT(("Adding vertex c: %c%" PRISIZE_T " point: %" PRISIZE_T " (%lf %lf)\n", currentChain == 0 ? 'A' : 'B', currentId,
               v.orgId,
               c[currentChain][v.orgId].pos.x(),
               c[currentChain][v.orgId].pos.y()));

        if (v.flag == flagToFollow)
        {
            /* Moving at the right side of the other polygon */
            currentId = (currentId + 1) % c[currentChain].size();
        }
        if (v.flag == flagToAvoid)
        {
            cout << "Internal Error" << endl;
            break;
        }

        if (v.flag == COMMON)
        {
            size_t otherChain  = 1 - currentChain;
            size_t nextCurrent = ((currentId  + 1) % c[currentChain].size());
            size_t nextOther   = ((v.other    + 1) % c[otherChain  ].size());

            struct {
                const VertexData *node;
                size_t chain;
                size_t pos;
            } candidates[2];

            candidates[0] = {&c[currentChain][nextCurrent], currentChain, nextCurrent};
            candidates[1] = {&c[otherChain  ][nextOther  ], otherChain  , nextOther  };

            if (trace) SYNC_PRINT(("Branching (%c%" PRISIZE_T ") (%c%" PRISIZE_T ")\n", currentChain == 0 ? 'A' : 'B' , nextCurrent , otherChain == 0 ? 'A' : 'B', nextOther));

            /*if ( ( inner && (candidate1.flag != OUTSIDE)) ||
                 (!inner && (candidate1.flag == OUTSIDE)))*/

            int cand = 0;
            /* Prefer going your direction */
            for (cand = 0; cand < 2; cand++)
            {
                if (candidates[cand].node->flag == flagToFollow)
                {
                    if (trace) cout << "Choosing:" << cand << " because flag" << endl;
                    currentChain = candidates[cand].chain;
                    currentId    = candidates[cand].pos  ;
                    break;
                }
            }

            if (cand == 2) {
                /* Always change the chain if both ends are common */
                if (candidates[0].node->flag == COMMON && candidates[1].node->flag == COMMON)
                {
                    if (trace) cout << "Choosing:" << cand << " because chain change" << endl;
                    currentChain = candidates[1].chain;
                    currentId    = candidates[1].pos  ;
                    cand = 0;
                }
            }

            if (cand == 2) {
                for (cand = 0; cand < 2; cand++)
                {
                    if (candidates[cand].node->flag != flagToAvoid)
                    {
                        if (trace) cout << "Choosing:" << cand << " because avoid" << endl;
                        currentChain = candidates[cand].chain;
                        currentId    = candidates[cand].pos  ;
                        break;
                    }
                }
            }

            if (cand == 2) {
                cout << "Internal Error2" << endl;
                break;
            }
        }

        /* Check for exit condition */
        VertexData v1 = c[currentChain][currentId];
        if (v1.flag == COMMON)
        {
            if (trace) SYNC_PRINT(("Checking for exit on (%c%" PRISIZE_T ")\n", currentChain == 0 ? 'A' : 'B' , currentId));

            if ((currentChain == 0) && (currentId == fst.first))
                break;
            if ((currentChain == 0) && (v1.other  == fst.second))
                break;

            if ((currentChain == 1) && (currentId == fst.second))
                break;
            if ((currentChain == 1) && (v1.other   == fst.first))
                break;
        }
    }
    return result;
}

Polygon PolygonCombiner::intersection() const
{
    Polygon result;
    if (intersectionNumber == 0) /* There are no contur intersection */
    {
        /* First poligon is inside the second one */
        if (!c[0].empty() && c[0].front().flag == INSIDE)
            return Polygon(pol[0]);

        /* second poligon is inside the first one */
        if (!c[1].empty() && c[1].front().flag == INSIDE)
            return Polygon(pol[1]);

        return result;
    }

    return followContour(0, true);
}

vector<Polygon> PolygonCombiner::intersectionAll() const
{
    vector<Polygon> result;
    cout << "PolygonCombiner::intersectionAll(): intesections: " << intersectionNumber << endl;
    if (intersectionNumber == 0) /* There are no contur intersection */
    {
        cout << "PolygonCombiner::intersectionAll(): no intersections" << endl;

        /* First poligon is inside the second one */
        if (!c[0].empty() && c[0].front().flag == INSIDE) {
            result.push_back(pol[0]);
        }

        /* second poligon is inside the first one */
        if (!c[1].empty() && c[1].front().flag == INSIDE) {
            result.push_back(pol[1]);
            return result;
        }
        return result;
    }

    vector<bool> visited(intersectionNumber, false);

    while (true) {
        int p0 = -1;
        for (size_t v = 0; v < visited.size(); v++)
        {
            if (!visited[v]) {
                p0 = (int)v;
            }
        }
        if (p0 == -1)
            return result;

        result.push_back(followContour(p0, true, &visited));
    }
}

Polygon PolygonCombiner::combination() const
{
    Polygon result;
    if (intersectionNumber == 0) /* There are no contur intersection */
    {
        // So far we return empty poligon
        return result;
    }

    return followContour(0, false);
}

Polygon PolygonCombiner::difference() const
{
    SYNC_PRINT(("PolygonCombiner::difference(): Not yet implemented\n"));
    return Polygon();
}

Vector2dd PointPath::center()
{
    Vector2dd mean(0.0);
    for (Vector2dd point : *this)
    {
        mean += point;
    }
    if (!this->empty())
    {
        mean /= this->size();
    }
    return mean;
}

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

Polygon ProjectivePolygon::getApproximation(double farDist)
{
    Polygon toReturn;
    toReturn.reserve(this->size());

    for (const Vector3dd &proj : *this)
    {
        if (proj.z() == 0) {
            toReturn.push_back(proj.xy() * farDist);
        } else {
            toReturn.push_back(proj.normalisedProjective().xy());
        }
    }
    return toReturn;
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

