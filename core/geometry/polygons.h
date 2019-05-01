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

namespace corecvs {

using std::vector;

/**
 * This class is a mapping of 2D plane (parallelogram coordinate system) in 3D
 **/
class PlaneFrame {
public:
    Vector3dd p1; /**< Point of origin */
    Vector3dd e1; /**< X ort of the plane */
    Vector3dd e2; /**< Y ort of the plane. It's generally not enforced X and Y to be ortogonal in 3D */

    PlaneFrame() {}

    PlaneFrame(Vector3dd p1, Vector3dd e1, Vector3dd e2)
        : p1(p1), e1(e1), e2(e2)
    {}

    Vector3dd getNormal() const
    {
        return e1 ^ e2;
    }

    Plane3d toPlane() {
        return Plane3d::FromPointAndVectors(p1, e1, e2);
    }

    Vector3dd getPoint(double x, double y) const
    {
        return p1 + x * e1 + y * e2;
    }

    Vector3dd getPoint(const Vector2dd &txy) const
    {
        return getPoint(txy.x(), txy.y());
    }

    template <class Transformer>
    void transform(const Transformer &M)
    {
        Vector3dd newP = M * p1;
        e1 = M * (e1 + p1) - newP;
        e2 = M * (e2 + p1) - newP;
        p1 = newP;
    }

    template <class Transformer>
    PlaneFrame transformed(const Transformer &M)
    {
        PlaneFrame toReturn;
        toReturn.transform<Transformer> (M);
        return toReturn;
    }


    Vector2dd projectTo(const Vector3dd &point)
    {      
        Ray3d normal = Ray3d(getNormal(), point);
        double t, u, v;
        if (!intersectWithRay(normal, t, u, v))
            return Vector2dd::Zero(); /* This is impossible */
        return Vector2dd(u, v);
    }

    /**
       This method intersects with triangle created on two orts of PlaneFrame
       additionally it searches for texture coordinates.

       This basically solves
          ray.p + t * ray.a = p + u * e1 + v * e2
       for t,u,v.

       This method tries to quickly exit if there is no intersection
     **/
    bool intersectWithOrtTriangle(Ray3d &ray, double &resT, double &u, double &v)
    {
        double EPSILON = 0.00001;
        Vector3dd p =  ray.a ^ e2;

        /** This is the volume of the parallepiped built on two edges and a ray origin **/
        double vol = e1 & p;

        /** If volume is zero this means e1,e2 and ray direction is coplanar so no intersection possible */
        if (vol > -EPSILON && vol < EPSILON)
            return false;

        double inv_vol = 1.0 / vol;


        Vector3dd T = ray.p - p1;

        u = (T & p) * inv_vol;
        if(u < 0.0 || u > 1.0) {
            return false;
        }

        Vector3dd Q = T ^ e1;

        v = (ray.a & Q) * inv_vol;

        if(v < 0.f || u + v  > 1.f) return false;

        double t = (e2 & Q) * inv_vol;

        resT = t;
        return true;
    }

    /**
       This method intersects with triangle created on two orts of PlaneFrame
       additionally it searches for texture coordinates.

       This basically solves
          ray.p + t * ray.a = p + u * e1 + v * e2
       for t,u,v.
       <pre>
                        (   t )        (           )
       (ray.a e1 e2)    ( - u )     =  ( p - ray.p )
                    3x3 ( - v ) 3x1    (           ) 3x1

       </pre>

    **/
    bool intersectWithRay(const Ray3d &ray, double &resT, double &u, double &v)
    {
        Vector3dd d = p1 - ray.p;
        Matrix33 M = Matrix33::FromColumns(ray.a, e1, e2);
        if (M.det() == 0.0)
            return false;
        M.invert();
        Vector3dd res = M * d;

        resT =  res.x();
        u    = -res.y();
        v    = -res.z();

        return true;
    }

    Vector2dd intersectWithRayPoint(const Ray3d &ray)
    {
        double t = 0;
        Vector2dd result(std::numeric_limits<double>::quiet_NaN());
        intersectWithRay(ray, t, result.x(), result.y());
        return result;
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(p1, Vector3dd(0.0, 0.0, 0.0) , "p");
        visitor.visit(e1, Vector3dd::OrtX() , "e1");
        visitor.visit(e2, Vector3dd::OrtY() , "e2");
    }


    static PlaneFrame PlaneXY() {
        return PlaneFrame(Vector3dd::Zero(), Vector3dd::OrtX(), Vector3dd::OrtY());
    }

    static PlaneFrame PlaneYZ() {
        return PlaneFrame(Vector3dd::Zero(), Vector3dd::OrtY(), Vector3dd::OrtZ());
    }

    static PlaneFrame PlaneXZ() {
        return PlaneFrame(Vector3dd::Zero(), Vector3dd::OrtX(), Vector3dd::OrtZ());
    }


    friend ostream & operator <<(ostream &out, const PlaneFrame &frame)
    {
        out << "(" << frame.p1 << ") " << frame.e1 << " " << frame.e2;
        return out;
    }
};


template <>
inline void PlaneFrame::transform<Affine3DQ>(const Affine3DQ &M)
{
    SYNC_PRINT(("PlaneFrame::transform<Affine3DQ>\n"));
    p1 = M * p1;
    e1 = M.rotor * e1;
    e2 = M.rotor * e2;
}


template<typename PointType>
class GenericTriangle
{
public:
    static const int SIZE = 3;
    PointType p[SIZE];

    PointType &p1() {return p[0];}
    PointType &p2() {return p[1];}
    PointType &p3() {return p[2];}

    const PointType &p1() const {return p[0];}
    const PointType &p2() const {return p[1];}
    const PointType &p3() const {return p[2];}


    Ray3d r1() const {return Ray3d::FromPoints(p[0], p[1]);}
    Ray3d r2() const {return Ray3d::FromPoints(p[1], p[2]);}
    Ray3d r3() const {return Ray3d::FromPoints(p[2], p[0]);}

    Segment<PointType> side1() const {return Segment<PointType>(p2(), p1()); }
    Segment<PointType> side2() const {return Segment<PointType>(p3(), p2()); }
    Segment<PointType> side3() const {return Segment<PointType>(p1(), p3()); }


    GenericTriangle() {}

    GenericTriangle(const PointType _p1, const PointType _p2, const PointType _p3)
    {
        p1() = _p1;
        p2() = _p2;
        p3() = _p3;
    }

    Plane3d getPlane() const
    {
        return Plane3d::FromPoints(p1(), p2(), p3());
    }

    Vector3dd getNormal() const
    {
        return Plane3d::NormalFromPoints(p1(), p2(), p3());
    }

    PlaneFrame toPlaneFrame() const
    {
        return PlaneFrame(p1(), p2() - p1(), p3() - p1());
    }

    PointType getCenter() const
    {
        return (p1() + p2() + p3()) / 3.0;
    }


    bool intersectWithP(Ray3d &ray, double &resT)
    {
        //double EPSILON = 0.00001;

        PlaneFrame frame = toPlaneFrame();
        double u, v;
        return frame.intersectWithOrtTriangle(ray, resT, u, v);
    }

    bool intersectWith(Ray3d &ray, Vector3dd &point)
    {
        double t;
        if (intersectWithP(ray, t))
        {
            point = ray.getPoint(t);
            return true;
        }
        return false;
    }

    double getArea()
    {
        Vector3dd f = p2() - p1();
        Vector3dd s = p3() - p1();
        return  0.5 * !(f ^ s);
    }

    /** NOTE: This could swap the normal **/
    void sortByY() {
        if (p1().y() > p2().y()) std::swap(p1(), p2());
        if (p2().y() > p3().y()) std::swap(p2(), p3());
        if (p1().y() > p2().y()) std::swap(p1(), p2());
    }


    void transform(const Matrix33 &transform)
    {
        for (int i = 0; i < SIZE; i++)
        {
            p[i] = transform * p[i];
        }
    }


    friend ostream & operator <<(ostream &out, const GenericTriangle &triangle)
    {
        out << "(" << triangle.p1() << triangle.p2() << " " << triangle.p3() << ") " << std::endl;
        return out;
    }

};

typedef GenericTriangle<Vector3d<int32_t> > Triangle32;
typedef GenericTriangle<Vector2d<double> > Triangle2dd;

typedef GenericTriangle<Vector3d<double> > Triangle3dd;


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


class ConvexHull
{
public:
    /**
     * Most trivial and slow algorithms
     *
     * These methods need a lot of additional testing
     ***/
    static Polygon GiftWrap(const std::vector<Vector2dd> &list);
    static Polygon GrahamScan(std::vector<Vector2dd> points);

    static ProjectivePolygon GrahamScan(std::vector<Vector3dd> points);


template<typename PointType>
    static Polygon GrahamScan(std::vector<PointType> points);



    enum ConvexHullMethod {
        GIFT_WARP,
        GRAHAM_SCAN,
        LAST
    };

    static Polygon ConvexHullCompute(std::vector<Vector2dd> points, ConvexHullMethod &method);
};

} //namespace corecvs

#endif /* POLYGONS_H_ */
