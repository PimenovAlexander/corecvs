#ifndef POLYGONS_H_
#define POLYGONS_H_

/**
 * \file polygons.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 14, 2010
 * \author alexander
 */


#include <vector>
#include <algorithm>
#include <ostream>

#include "core/math/vector/vector3d.h"
#include "core/xml/generated/axisAlignedBoxParameters.h"
#include "core/geometry/line.h"
#include "core/math/affine.h"
#include "core/geometry/rectangle.h"
#include "core/geometry/convexPolyhedron.h"
#include "core/geometry/planeFrame.h"

namespace corecvs {

using std::vector;






class PointPath : public vector<Vector2dd>
{
public:
    PointPath(){}

    PointPath(std::initializer_list<Vector2dd> list) : vector<Vector2dd>(list) {}

    PointPath(int len) : vector<Vector2dd>(len)
    {}

    friend std::ostream & operator <<(std::ostream &out, const PointPath &pointPath)
    {
        out << "[";
        for (size_t i = 0; i < pointPath.size(); i++)
           out << (i == 0 ? "" : ", ") << pointPath.at(i) << std::endl;
        out << "]";
        return out;
    }

    bool hasVertex(const Vector2dd &point)
    {
        return std::find(begin(), end(), point) != end();
    }

    /* This function checks if the polygon is inside the buffer.
       It assumes that the polygon coordinate can be rounded to upper value
     */
    bool isInsideBuffer(const Vector2d<int> &bufferSize)
    {
        for (Vector2dd point : *this)
        {
            if (point.x() < 0 || point.y() < 0)
                return false;
            if (point.x() + 1 > bufferSize.x() || point.y() + 1 > bufferSize.y())
                return false;

        }
        return true;
    }

    Vector2dd center();


    void transform(const Matrix33 &transform)
    {
        for (Vector2dd &p: *this ) {
            p = transform * p;
        }
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        int pointsSize = (int)size();
        visitor.visit(pointsSize, 0, "points.size");

        resize(pointsSize);

        for (size_t i = 0; i < (size_t)pointsSize; i++)
        {
            char buffer[100];
            snprintf2buf(buffer, "points[%d]", i);
            visitor.visit(operator [](i), Vector2dd::Zero(), buffer);
        }

    }

};

/**
 *  Polygon
 **/
class Polygon : public PointPath
{
public:
    Polygon(){}

    Polygon(std::initializer_list<Vector2dd> list) : PointPath(list) {}

    Polygon(const Vector2dd *points, int len) : PointPath(len)
    {
        for (unsigned i = 0; i < size(); i++) {
           this->operator[](i) = points[i];
        }
    }

    /**
     * Checks if the point is inside of the convex polygon
     *
     * \attention convex only
     **/
    int  isInsideConvex(const Vector2dd &point) const;


    /**
     *  Winding number is the number of loops polygon makes around the point
     **/
    int windingNumber(const Vector2dd &point) const;
    int  isInside(const Vector2dd &point) const;


    /**
     *   direction - true for clockwize orientation in CG axis.
     *    X - to the right
     *    Y - down
     **/
    bool isConvex(bool *direction = NULL) const;
    bool hasSelfIntersection() const;

    Vector2dd &getPoint (int i) {
        return operator [](i);
    }

    const Vector2dd &getPoint (int i) const {
        return operator [](i);
    }

    Vector2dd &getNextPoint(int idx)
    {
       return operator []((idx + 1) % size());
    }

    int getNextIndex(int idx) const
    {
       return (idx + 1) % size();
    }

    const Vector2dd &getNextPoint(int idx) const
    {
       return operator []((idx + 1) % size());
    }

    const Vector2dd &getPrevPoint(int idx) const
    {
       return operator []((idx - 1 + size()) % size());
    }


    int getNextDifferentIndex(int idx) const
    {
        int idx1 = getNextIndex(idx);
        Vector2dd next = getPoint(idx1);
        while (next == getPoint(idx) && idx1 != idx)
        {
            idx1 = getNextIndex(idx1);
            next = getPoint(idx1);
        }
        return idx1;

    }


    const Vector2dd &getNext2Point(int idx) const
    {
       return operator []((idx + 2) % size());
    }

    /** **/
    Ray2d getRay(int i)  const {
        return Ray2d::FromPoints(getPoint(i), getNextPoint(i));
    }

    Segment2d getSegment(int i)  const {
        return Segment2d(getPoint(i), getNextPoint(i));
    }

    Line2d getLine(int i) const {
        return Line2d(getSegment(i));
    }

    /** This method uses the index by module of size() **/
    Vector2dd &getPointM(int idx)
    {
       return operator [](idx % size());
    }

    Vector2dd getNormal(int i) const
    {
        Vector2dd r1 = getPoint(i);
        Vector2dd r2 = getNextPoint(i);
        return (r2 - r1).rightNormal();
    }

    static Polygon FromRectagle(const Rectangled &rect) {
        Polygon toReturn;
        toReturn.reserve(4);
        toReturn.push_back(rect.ulCorner());
        toReturn.push_back(rect.urCorner());
        toReturn.push_back(rect.lrCorner());
        toReturn.push_back(rect.llCorner());
        return toReturn;
    }

    static Polygon RegularPolygon(int sides, const Vector2dd &center, double radius, double startAngleRad = 0.0);

    static Polygon Reversed(const Polygon &p);

    static Polygon FromConvexPolygon(const ConvexPolygon& polygon);
    static Polygon FromHalfplanes   (const std::vector<Line2d> &halfplanes);

    static Polygon FromImageSize    (const Vector2d<int> &size);

    ConvexPolygon toConvexPolygon() const;

    /** This thing is mostly for debug */
    ConvexPolygon toConvexPolygonSubdivide(int subdivision = 20) const;


    Polygon transformed(const Matrix33 &transform) const
    {
        Polygon toReturn;
        toReturn.reserve(size());
        for (Vector2dd p: *this ) {
            toReturn.push_back(transform * p);
        }
        return toReturn;
    }

    /* Additional helper for shift only */
    void translate(const Vector2dd &translation)
    {
        for (Vector2dd &p: *this ) {
            p += translation;
        }
    }

    Polygon translated(const Vector2dd &translation) const
    {
        Polygon p(*this);
        p.translate(translation);
        return p;
    }

    /* non const versions */
    double &x(int idx) {
        return operator [](idx).x();
    }

    double &y(int idx) {
        return operator [](idx).y();
    }

    /* const versions*/
    const double &x(int idx) const {
        return operator [](idx).x();
    }

    const double &y(int idx) const {
        return operator [](idx).y();
    }

    Ray2d edgeAsRay(int idx)
    {
        Vector2dd &start = getPoint(idx);
        Vector2dd &end   = getNextPoint(idx);
        return Ray2d::FromPoints(start, end);
    }

    /* Valid for any type of simple polygon*/
    double signedArea();

    double area() {
        return fabs(signedArea());
    }

    //bool clipRay(const Ray2d &ray, double &t1, double &t2);
    void reverse();
};

class ProjectivePolygon : public vector<Vector3dd>
{
public:
    ProjectivePolygon() {}

    ProjectivePolygon(const vector<Vector3dd> &in)
    {
        *(static_cast<vector<Vector3dd> *>(this)) = in;
    }

    ProjectivePolygon(const vector<Vector2dd> &in)
    {
        reserve(in.size());
        for (const Vector2dd &p : in) {
            push_back(Vector3dd(p, 1.0));
        }
    }

    void transform(const Matrix33 &transform)
    {
        for (Vector3dd &p: *this ) {
            p = transform * p;
        }
    }


    ProjectivePolygon transformed(const Matrix33 &transform) const
    {
        ProjectivePolygon toReturn;
        toReturn.reserve(size());
        for (Vector3dd p: *this ) {
            toReturn.push_back(transform * p);
        }
        return toReturn;
    }

    /* Returns approxiamiton */
    Polygon getApproximation(double farDist);

    friend std::ostream & operator <<(std::ostream &out, const ProjectivePolygon &pPolygon)
    {
        out << "[";
        for (size_t i = 0; i < pPolygon.size(); i++) {
            if (pPolygon.at(i).z() == 0) {
                out << (i == 0 ? "" : ", ") << "infto:" << pPolygon.at(i).xy() <<  std::endl;
            } else {
                out << (i == 0 ? "" : ", ") << pPolygon.at(i).normalisedProjective().xy() << std::endl;
            }
        }
        out << "]";
        return out;
    }

    static ProjectivePolygon FromConvexPolygon(const ConvexPolygon &cp);
    ConvexPolygon toConvexPolygon () const;


};


class FlatPolygon
{
public:
    PlaneFrame frame;
    Polygon polygon;

    template <class Transformer>
    void transform(const Transformer &M)
    {
        frame.transform<Transformer>(M);
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(frame,   "frame");
        visitor.visit(polygon, "polygon");
    }
};



class RGB24Buffer;

/**
 *  This class implements Weiler–Atherton algotithm
 *
 **/
class PolygonCombiner
{
public:
    Polygon pol[2]; /* We actually don't need to copy polygon, but for sake of simplicity we reverse them to positive orientation*/

    enum VertexType {
        INSIDE,
        COMMON,
        OUTSIDE
    };

    static std::string getName(const VertexType type)
    {
        switch (type) {
        case INSIDE:
            return "inside";
            break;
        case OUTSIDE:
            return "outside";
            break;
        case COMMON:
            return "common";
            break;
        default:
            break;
        }
        return "";
    }

    struct VertexData {
        size_t orgId;
        Vector2dd pos;     /* We don't need this, just a cache*/
        VertexType flag;
        double t;
        size_t intersection;
        size_t other;

        VertexData(size_t orgId, Vector2dd pos, VertexType inside, double t, size_t intersection = 0) :
           orgId(orgId),
           pos(pos),
           flag(inside),
           t(t),
           intersection(intersection),
           other(0)
        {}
    };

    typedef std::vector<VertexData> ContainerType; /* This type should better be list */

    /** These are two lists for each of the polygons including there own and common vertexes **/
    ContainerType c[2];

    /** These structures store the common vertexes **/
    int intersectionNumber;
    std::vector<std::pair<size_t, size_t>> intersections;

    /* */
    bool trace = false;

    /* Method that initialise internal data structures of the polygonCombiner*/
    void prepare(void);

    /* */
    bool validateState(void) const;

    Rectangled getDebugRectangle() const;
    void drawDebug(RGB24Buffer *buffer) const;
    void drawDebugAutoscale(RGB24Buffer *buffer, int margin = 100) const;

    Polygon followContour(int startIntersection, bool inner, vector<bool> *visited = NULL) const;

    Polygon intersection() const;
    vector<Polygon> intersectionAll() const; /**< Not yet implemented **/

    Polygon combination() const;
    Polygon difference() const;    /**< Not yet implemented */

    PolygonCombiner()
    {}

    PolygonCombiner(Polygon &p1, Polygon &p2)
    {
         pol[0] = p1;
         pol[1] = p2;
    }

    friend std::ostream & operator <<(std::ostream &out, const PolygonCombiner &combiner)
    {
        out << "A: [" << std::endl;
        for (size_t i = 0; i < combiner.c[0].size(); i++)
        {
            const VertexData &vd = combiner.c[0][i];
            out << " " << i << ":  (" << vd.pos << " " << getName(vd.flag) << " " << vd.t << " " << vd.other << ") " << std::endl;
        }
        out << "]" << std::endl;
        out << "B: [" << std::endl;
        for (size_t i = 0; i < combiner.c[1].size(); i++)
        {
            const VertexData &vd = combiner.c[1][i];
            out << " " << i << ":  (" << vd.pos << " " << getName(vd.flag) << " " << vd.t << " " << vd.other << ") " << std::endl;
        }
        out << "]" << std::endl;

        out << "Intersections:" << std::endl;
        for (auto &pair : combiner.intersections)
        {
            out << pair.first << " - " << pair.second << std::endl;
        }
        return out;
    }
};




} //namespace corecvs

#endif /* POLYGONS_H_ */
