#ifndef LINE_H_
#define LINE_H_

#include <cmath>

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"
#include "core/math/matrix/matrix44.h"


namespace corecvs {
/**
 * \file line.h
 * \brief a header for line.c
 *
 * \date Oct 1, 2011
 * \author alexander
 */


/**
 * Line segment based on 2 points
 *
 */
template<class VectorType>
class Segment
{
public:
    VectorType a;
    VectorType b;

    Segment(const VectorType &_a, const VectorType &_b) :
        a(_a),
        b(_b)
    {}

    VectorType getCenter() const
    {
        return (a + b) / 2.0;
    }

    typename VectorType::InnerElementType getLength() const {
        return (a-b).l2Metric();
    }

    friend std::ostream & operator <<(std::ostream &out, const Segment &ray)
    {
        out << "[" << ray.a << " - " << ray.b << "]";
        return out;
    }
};


/**
 * Line segment in 2D and 3D
 *
 */
//typedef Segment<Vector2dd> Segment2d;
typedef Segment<Vector3dd> Segment3d;

class Segment2d : public Segment<Vector2dd>
{
public:
    Segment2d(const Vector2dd &_a, const Vector2dd &_b) :
        Segment<Vector2dd>(_a,_b)
    {}

   static Vector2dd intersect(const Segment2d &s1, const Segment2d &s2, bool &hasIntersection);

};



/**
 * Ray2d is a template of the class that holds the description of the ray in the \f$ R^{dimention} \f$ space.
 *
 * \f[ \overrightarrow {P(t)} = \overrightarrow {a} *t  + \overrightarrow {p} \f]
 *
 * You would occasionally want to hold \f$\overrightarrow {a}\f$ normalized
 *
 *
 **/
template<class RealType, class VectorType>
class BaseRay
{
public:
    VectorType a; /**< Directional vector */
    VectorType p; /**< Starting point     */

    BaseRay() {}

    BaseRay(const VectorType &_a, const VectorType & _p) :
        a(_a),
        p(_p)
    {}

          VectorType &direction()       {return a;}
    const VectorType &direction() const {return a;}

          VectorType &origin()       {return p;}
    const VectorType &origin() const {return p;}


    VectorType getPoint(double t) const
    {
        return p + a * t;
    }

    VectorType getStart() const
    {
        return p;
    }

    VectorType getEnd() const
    {
        return getPoint(1.0);
    }

    VectorType projectOnRay(const VectorType &q)
    {
        return getPoint((q - p) & a.normalised());
    }

    /**
     *   Normalizes the direction vector without changing the ray itself
     *
     *
     **/
    void normalise(void)
    {
        double l = a.l2Metric();
        if (l == 0.0)
            return;
        a /= l;
    }

    RealType normalised(void)
    {
        RealType toReturn(*this);
        toReturn.normalise();
        return toReturn;
    }

    static RealType FromOriginAndDirection(const VectorType &origin, const VectorType &direction)
    {
        return RealType(direction, origin);
    }

    static RealType FromDirectionAndOrigin(const VectorType &direction, const VectorType &origin)
    {
        return RealType(direction, origin);
    }

    static RealType FromPoints(const VectorType &origin, const VectorType &end)
    {
        return RealType::FromOriginAndDirection(origin, end - origin);
    }
    /**
     *
     * This method implements Cyrus-Beck algorithm
     * http://en.wikipedia.org/wiki/Cyrus%E2%80%93Beck_algorithm
     *
     * This method expects normals to be internal
     *
     * Intersection with each line is computed and classified as an enter, or exit to the polygon halfspace.
     * An internal normal is computed - n. On a ray there is a parametrised point.
     *
     * Criteria for intersection is
     * \f[
     *    \overrightarrow {P(t)} = \overrightarrow {p} + \overrightarrow {a} t
     * \f]
     * \f[
     *    ( \overrightarrow {P(t)} - \overrightarrow {r} ) \overrightarrow {n} = 0
     * \f]
     * \f[
     *    ( \overrightarrow {p} + \overrightarrow {a} t - \overrightarrow {r} ) \overrightarrow {n} = 0
     * \f]
     * \f[
     *    ( \overrightarrow {p} - \overrightarrow {r} ) \overrightarrow {n} + \overrightarrow {a} \overrightarrow {n} t = 0
     * \f]
     * \f[
     *    t = - {( \overrightarrow {p} - \overrightarrow {r} ) \overrightarrow {n} \over \overrightarrow {a} \overrightarrow {n} }
     * \f]
     * \f[
     *    t = {( \overrightarrow {r} - \overrightarrow {p} ) \overrightarrow {n} \over \overrightarrow {a} \overrightarrow {n} }
     * \f]
     *
     *
     * If the point is an entry one the sign of \f$\overrightarrow {a} \overrightarrow {n}\f$ will denote if it is an entry or exit
     * What interests us is a maximum entry point and minimum exit point
     *
     *
     *
     * */
    template<typename ConvexType>
    bool clip(const ConvexType &convex, double &t1, double &t2) const
    {
        t1 = -numeric_limits<double>::infinity();
        t2 =  numeric_limits<double>::infinity();

        for (unsigned i = 0; i < convex.size();  i++) {
            VectorType r = convex.getPoint(i);
            VectorType n = convex.getNormal(i);

            VectorType diff = r - p;

            double numen = diff & n;
            double denum = a & n;
            if (denum == 0.0) { /* We are parallel */
                if (numen > 0) /* We are parallel and outside. Fail. Leaving. */
                {
                    t1 = -numeric_limits<double>::infinity();
                    t2 =  numeric_limits<double>::infinity();
                    return false;
                }

                /* We were on the right side. Still a chance for intersection */
                continue;
            }
            double t = numen / denum;

            if ((denum > 0) && (t > t1)) {
                t1 = t;
            }
            if ((denum < 0) && (t < t2)) {
                t2 = t;
            }

            //cout << "Intersection " << t << " at " << getPoint(t) << " is " << (numen > 0 ? "enter" : "exit") << std::endl;

        }
        return t2 > t1;
    }

    friend std::ostream & operator <<(std::ostream &out, const BaseRay &ray)
    {
        out << ray.p << "->" << ray.a;
        return out;
    }

};

/**
 * Ray2d is 2D version of Ray
 **/
class Ray2d : public BaseRay<Ray2d, Vector2dd>
{
public:
    Ray2d(const Vector2dd &_a, const Vector2dd & _p) :
        BaseRay<Ray2d, Vector2dd>(_a, _p)
    {}

    Ray2d(const Segment2d &s) :
        BaseRay<Ray2d, Vector2dd>(s.b - s.a, s.a)
    {}

    static Ray2d FromPoints(const Vector2dd &start, const Vector2dd &end)
    {
        return Ray2d(end - start, start);
    }

    static Ray2d FromPointAndDirection(const Vector2dd &point, const Vector2dd &direction)
    {
        return Ray2d(direction, point);
    }

    static bool hasIntersection(const Ray2d &ray1, const Ray2d &ray2);


    static Vector2dd intersectionPoint(const Ray2d &ray1, const Ray2d &ray2, bool *hasIntersection);
    static Vector2dd intersection(const Ray2d &ray1, const Ray2d &ray2, double &t1, double &t2);


};

/**
 * Ray3d is 3D version of Ray
 **/
class Ray3d : public BaseRay<Ray3d, Vector3dd>
{
public:
    Ray3d() {}

    Ray3d(const Vector3dd &direction, const Vector3dd & origin) :
        BaseRay<Ray3d, Vector3dd>(direction, origin)
    {}

    Ray3d(const BaseRay<Ray3d, Vector3dd> &base) : BaseRay<Ray3d, Vector3dd>(base)
    {}

    double distanceTo(const Ray3d &other ) const
    {
        Vector3dd denum = a ^ other.a;
        Vector3dd dp = p - other.p;

        double l = denum.l2Metric();
        if (l == 0.0)
        {
            return (dp ^ a).l2Metric() / a.l2Metric();
        }

        return fabs(dp & denum) / l;
    }

    double projectionP(const Vector3dd &point) const
    {
        Vector3dd d  = point - p;
        return (d & a) / a.l2Metric();

    }

    Vector3dd projection(const Vector3dd &point) const
    {
        return getPoint(projectionP(point));
    }

    double distanceTo(const Vector3dd &point) const
    {
        return (point - projection(point)).l2Metric();
    }

    /**
     *  This is a helpful method to estimate properties of skew lines
     *  Assuming A and B - are points closest to each other on *this and other
     *  respectively.
     *
     *  As a result there will be a vector that holds
     *
     *   x() - the parameter value to travel *this to get to A
     *   y() - the parameter value to travel other to get to B
     *   z() - the distance between A and B
     *
     **/
    Vector3dd intersectCoef(Ray3d &other)
    {
        Vector3dd normal = a ^ other.a;
        normal.normalise();
        Matrix33 m = Matrix33::FromColumns(a, -other.a, normal);
        return m.inv() * (other.p - p);
    }

    /**
     * not optimal so far
     **/
    Vector3dd closestPoint(Ray3d &other)
    {
        return getPoint(intersectCoef(other).x());
    }

    Vector3dd intersect(Ray3d &other)
    {
        Vector3dd coef = intersectCoef(other);
        return (getPoint(coef.x()) + other.getPoint(coef.y())) / 2.0;
    }
    
    std::pair<Vector3dd, Vector3dd> pluckerize() const
    {
        Vector3dd an = a.normalised();
        return std::make_pair(an, p ^ an);
    }

    template <class Transformer>
    void transform(const Transformer &M)
    {
        a = M * a;
        p = M * p;
    }

    template <class Transformer>
    Ray3d transformed(const Transformer &M)
    {
        return Ray3d((M * (p + a)) - (M * p), M * p);
    }


    /**
     *   \f[
     *     H=\frac{\|(\mathbf{c}-\mathbf{a})\times\mathbf{d}\|}{\|\mathbf{d}\|}
     *     (\mathbf{a}\times\mathbf{b})\cdot(\mathbf{a}\times\mathbf{b})=\|\mathbf{a}\|^2\|\mathbf{b}\|^2-(\mathbf{a}\cdot\mathbf{b})^2
     *     H^2=\frac{\|\mathbf{c}-\mathbf{a}\|^2\|\mathbf{d}\|^2-\|(\mathbf{c}-\mathbf{a})\cdot\mathbf{d}\|^2 }{\|\mathbf{d}\|^2}
     *
     *     H^2=\frac{\|\mathbf{c}-\mathbf{a}\|^2\|\mathbf{d}\|^2-\|(\mathbf{c}-\mathbf{a})\cdot\mathbf{d}\|^2 }{\|\mathbf{d}\|^2}
     *
     *
     *     \frac{d(H^2)}{d\mathbf{c}}=2(\mathbf{c}-\mathbf{a})-2\mathbf{d}\frac{(\mathbf{c}-\mathbf{a})\cdot\mathbf{d}}{\|\mathbf{d}\|^2}
     *
     *     0=\sum_{i=0}^m{\mathbf{c}-\mathbf{a}^{(i)}-\mathbf{d}^{(i)}\frac{(\mathbf{c}-\mathbf{a}^{(i)})\cdot\mathbf{d}^{(i)}}{\|\mathbf{d}^{(i)}\|^2}}
     *   \f]
     *
     *
     **/
    static Vector3dd bestFit(Ray3d * /*rays*/, unsigned /*number*/)
    {
        return Vector3dd(0.0);
    }

    /**
     *  This is a getPoint variant that respects infinitely distant points
     **/
    FixedVector<double, 4> getProjectivePoint(double t)
    {
        if (std::isinf(t)) {
            if (t > 0) {
                return FixedVector<double, 4>(a, 0.0);
            } else {
                return FixedVector<double, 4>(-a, 0.0);
            }
        }
        return FixedVector<double, 4>(getPoint(t), 1.0);
    }
};

#if 0
/**
 *  This class template represents the hyperplane of the \f$ R^n \f$ dimention
 *  in \f$ R^{n+1} \f$ space
 *
 * The most obvious presentation of the hyperplane:
 *  \f[ (\overrightarrow {x} - \overrightarrow {a}) \overrightarrow {n} = 0 \f]
 * in this case \f$a\f$ is some point, and \f$n\f$ a normal for the hyperplane
 *
 * But this is not the most compact presentation.
 * This class uses the following presentation:
 *
 *  \f[ \overrightarrow {x} \overrightarrow {n} + C = 0\f]
 *
 * This can be reformulated as:
 *
 * \f[ \sum_{i=0}^n ({a_i x_i}) + C = 0\f]
 *
 * In 2D case that is
 * \f[A x + B y + C = 0\f]
 *
 * Which allows to think of the \f$ R^{dimention} \f$ hyperplane, as an element in a f$ \mathbb(R)^{dimention + 1} f$ projective space.
 * That is how we would think of the line form time to time.
 *
 * \f[ \sum_{i=0}^n ({a_i x_i}) + C = \sum_{i=0}^n ({a_i x_i}) + 1 \cdot x_{n+1} = 0 \f]
 *
 * \f$\vec {x}\f$ - is a  vector. In 3D \f$\overrightarrow {(A,B,C)} \overrightarrow{X} = 0\f$
 *  denotes a plane that pass throw the 0 point.
 *
 *  The only difference form FixedVector<double, dimention + 1> is that this class has other notion of
 *  normalisation
 *
 **/

template<int dimention>
class Hyperplane : public FixedVector<double, dimention + 1>
{
public:

    Hyperplane() {}

    Hyperplane(const Vector2dd &_n, const double &_c = 1.0) :
        Vector3dd(_n, _c)
    {}

    /**
     *   Normalizes the normal vector without changing the line itself
     *
     *   With such a line it is easier to compute distance to the line
     **/
    void normalise( void )
    {
        double sum(0);
        for (int i = 0; i < size() - 1; i++)
            sum += at(i) * at(i);
        double l = sqrt(sum);
        if (l == 0.0) return;
        operator /=(l);
    }

    Hyperplane normalised( void ) const
    {
        Hyperplane result = *this;
        result.normalise();
        return result;
    }

    /**
     *   Projective coordinates of the lines intersection is a cross product
     *   of the projective presentation of the line.
     *
     *   Actually line is an intersection of the planes
     *     \f$\overrightarrow{(a_1,b_1,c_1)} \overrightarrow{x} = 0\f$ and
     *     \f$\overrightarrow{(a_2,b_2,c_2)} \overrightarrow{x} = 0\f$ is
     *
     *     \f$\overrightarrow{(a_1,b_1,c_1)} \times \overrightarrow{(a_1,b_1,c_1)}\f$
     *
     *   Which make obvious the way the intersection is computed.
     *
     **/
    Vector3dd intersectWith(const Line2d &other) const
    {
        return (*this) ^ other;
    }

    /**
     *  Intersect result of two lines is a point
     **/
    Vector2dd intersectWith(const Line2d &other, bool *hasIntersection = NULL) const
    {
        double last = x() * other.y() - y() * other.x();
        if (last == 0.0)
        {
            if (hasIntersection) *hasIntersection = false;
            return Vector2dd(0);
        }

        double dx = x() * other.z() - other.x() * z();
        double dy = y() * other.z() - other.y() * z();
        if (hasIntersection) *hasIntersection = true;

        return Vector2dd(dx, dy) / last;
    }

    /**
     *
     * \f$d = \frac {\left| a x_0 + b y_0 + c \right|}  {\sqrt (a^2 + b^2) }\f$
     *
     **/
    double distanceTo (const Vector2dd &point)
    {
        double last = point.x() * x()  + point.y() * y() + z();
        if (last < 0) last = -last;
        double l = sqrt (x() * x() + y() * y());
        return (last / l);
    }


    double sqDistanceTo (const Vector2dd &point)
    {
        double last = point.x() * x()  + point.y() * y() + z();
        last *= last;
        double lsq = x() * x() + y() * y();
        return (last / lsq);
    }

    double pointWeight(const Vector2dd & point) const
    {
        return (*this) & Vector3dd(point, 1.0);
    }

    static Line2d FormNormal(const Vector2dd &normal)
    {
        return Line2d(normal);
    }

    static Line2d FormNormalAndPoint(const Vector2dd &normal, const Vector2dd &point)
    {
        return Line2d(normal, -(normal & point));
    }

private:

    void _init(const Vector2dd &_n, const double &_c = 1.0)
    {
        (*this)[0] = _n.x();
        (*this)[1] = _n.y();
        (*this)[2] = _c;
    }

};

#endif



/**
 *  This class represents the line in 2D space
 *
 * The most obvious presentation of the line:
 *  \f[ (\overrightarrow {x} - \overrightarrow {a}) \overrightarrow {n} = 0 \f]
 * in this case \f$a\f$ is some point, and \f$n\f$ a normal for the line
 *
 * But this is not the most compact presentation.
 * This class uses the following presentation:
 *
 *  \f[ \overrightarrow {x} \overrightarrow {n} + C = 0\f]
 *
 * This can be reformulated as:
 *
 * \f[A x + B y + c = 0\f]
 *
 * Which allows to think of the 2D line, as a line in a projective space. That is how we would
 * think of the line.
 *
 * \f$\overrightarrow {(A,B,C)}\f$ - is a 3D vector. In 3D \f$\overrightarrow {(A,B,C)} \overrightarrow{X} = 0\f$
 *  denotes a plane that pass throw the 0 point.
 *
 *
 **/

class Line2d : public Vector3dd
{
public:
    Line2d() {}

    Line2d(const double &_a, const double &_b, const double &_c)
    {
        (*this)[0] = _a;
        (*this)[1] = _b;
        (*this)[2] = _c;
    }

    Line2d(const Vector3dd &_line) : Vector3dd(_line) {}

    Line2d(const Vector2dd &_n, const double &_c = 0.0) :
        Vector3dd(_n, _c)
    {}

    /**
     *  Construct the Line form 2 points
     **/
    Line2d(const Segment2d &segment)
    {
       (*this) = FromSegment(segment);
    }

    Line2d(const Vector2dd &point1, const Vector2dd &point2)
    {
       (*this) = FromSegment(Segment2d(point1, point2));
    }

    /**
     *  Construct the Line form 2 points
     **/
    Line2d(const Ray2d &ray)
    {
       (*this) = FromRay(ray);
    }

    inline double &a()
    {
        return (*this)[0];
    }

    inline double &b()
    {
        return (*this)[1];
    }

    inline double &c()
    {
        return (*this)[2];
    }

    // Const versions
    inline const double &a() const
    {
        return (*this)[0];
    }

    inline const double &b() const
    {
        return (*this)[1];
    }

    inline const double &c() const
    {
        return (*this)[2];
    }

    static Line2d FromSegment(const Segment2d &segment)
    {
        return FromRay(Ray2d(segment));
    }

    Vector2dd normal(void) const
    {
        return Vector2dd(element[0], element[1]);
    }

    double &last(void)
    {
        return element[2];
    }

    const double &last(void) const
    {
        return element[2];
    }


    /**
     *   Normalizes the normal vector without changing the line itself
     *
     *   With such a line it is easier to compute distance to the line
     **/
    void normalise(void)
    {
        double l = normal().l2Metric();
        if (l == 0.0) return;
        operator /=(l);
    }

    Line2d normalised(void) const
    {
        Line2d result = *this;
        result.normalise();
        return result;
    }

    /**
     *   Projective coordinates of the lines intersection is a cross product
     *   of the projective presentation of the line.
     *
     *   Actually line is an intersection of the planes
     *     \f$\overrightarrow{(a_1,b_1,c_1)} \vec{x} = 0\f$ and
     *     \f$\overrightarrow{(a_2,b_2,c_2)} \vec{x} = 0\f$ is
     *
     *     \f$\overrightarrow{(a_1,b_1,c_1)} \times \overrightarrow{(a_1,b_1,c_1)}\f$
     *
     *   Which make obvious the way the intersection is computed.
     *
     **/
    Vector3dd intersectWithP(const Line2d &other) const
    {
        return (*this) ^ other;
    }

    /**
     *  Intersect result of two lines is a point
     **/
    Vector2dd intersectWith(const Line2d &other, bool *hasIntersection = NULL) const
    {
        double d = x() * other.y() - y() * other.x();

        bool intersects = (d != 0.0);
        if (hasIntersection) *hasIntersection = intersects;

        if (!intersects)
            return Vector2dd(0);

//      double dx = x() * other.z() - other.x() * z();
//      double dy = y() * other.z() - other.y() * z();

        double dx = y() * other.z() - other.y() * z();
        double dy = other.x() * z() - x() * other.z();


        return Vector2dd(dx, dy) / d;
    }

    /**
     * \f$ a x_0 + b y_0 + c \f$
     *
     **/
    double pointWeight(const Vector2dd & point) const
    {
        return (normal() & point) + last();
    }

    /**
     *   Returns -1, 0, 1 depending on the size on which point lies relative to the line
     **/
    int side(const Vector2dd & point)
    {
        double weight = pointWeight(point);
        if (abs(weight) <= 1.0e-11) return 0;
        return weight > 0 ? 1 : -1;
    }

    /**
     *
     * \f$d = \frac {\left| a x_0 + b y_0 + c \right|}  {\sqrt (a^2 + b^2) }\f$
     *
     **/
    double distanceTo (const Vector2dd &point) const
    {
        double d = pointWeight(point);
        if (d < 0) d = -d;
        double l = normal().l2Metric();
        return (d / l);
    }

    double sqDistanceTo (const Vector2dd &point) const
    {
        double d = pointWeight(point);
        d *= d;
        double lsq = normal().sumAllElementsSq();
        return (d / lsq);
    }

    static Line2d FromRay(const Ray2d &ray)
    {
       Vector2dd a = ray.a;
       Vector2dd p = ray.p;
       Vector2dd n = a.leftNormal();
       return Line2d(n, -(p & n));
    }

    /**
     *   projecting zero to the current line
     **/
    Vector2dd projectZeroTo() const
    {
        double l2 = normal().sumAllElementsSq();
        double t = (last() / l2);
        return - t * normal();
    }

    Ray2d toRay() const
    {
        return Ray2d::FromOriginAndDirection(projectZeroTo(), normal().leftNormal());
    }

    bool isVertical() const
    {
        return (y() == 0.0);
    }

    bool isHorizontal() const
    {
        return (x() == 0.0);
    }

    static Line2d FormNormal(const Vector2dd &normal)
    {
        return Line2d(normal);
    }

    static Line2d FormNormalAndPoint(const Vector2dd &normal, const Vector2dd &point)
    {
        return Line2d(normal, -(normal & point));
    }

    Vector2dd toDual() const {
        double m = -x() / y();
        double b = -z() / y();

        return Vector2dd(m, -b);
    }

    Vector2dd toDualS() const {
        double l = z() + l2Metric();
        double x1 = x() / l;
        double y2 = y() / l;
        return Vector2dd(x1, y2);
    }

    Vector3dd toDualP() const {
        return Vector3dd(a(), b(), c());
    }


    static Line2d FromDual(const Vector2dd &p) {
        return Line2d(-p.x(), 1.0, p.y());
    }

    static Line2d FromDualP(const Vector3dd &p) {
        return Line2d(p.x(), p.y(), p.z());
    }

private:
    void _init(const Vector2dd &_n, const double &_c = 1.0)
    {
        (*this)[0] = _n.x();
        (*this)[1] = _n.y();
        (*this)[2] = _c;
    }

};


inline bool Ray2d::hasIntersection(const Ray2d &ray1, const Ray2d &ray2)
{
    Line2d l1 = Line2d::FromRay(ray1);
    Line2d l2 = Line2d::FromRay(ray2);

    int flag01 = l1.side(ray2.getStart());
    int flag02 = l1.side(ray2.getEnd());

    int flag11 = l2.side(ray1.getStart());
    int flag12 = l2.side(ray1.getEnd());

    return (flag01 != flag02) && (flag11 != flag12);
}

inline Vector2dd Ray2d::intersectionPoint(const Ray2d &ray1, const Ray2d &ray2, bool *hasIntersection = NULL)
{
    double t1, t2;
    Vector2dd v  = intersection(ray1, ray2, t1, t2);

    if (hasIntersection != NULL)
    {
        *hasIntersection = (t1 != std::numeric_limits<double>::infinity());
    }

    return v;
}

 /**
  *  This is basically Kramer method
  *
  * \f[
  *  a_{1x} * t_1 - a_{2x} * t_2 = p_{1x} - p_{2x};
  *  a_{1y} * t_1 - a_{2y} * t_2 = p_{1y} - p_{2y};
  * \f]
  *
  * however, because dist has geometric meaning as the parallelogram area
  * we can easily find geomtic interpertation
  *
  *
  **/
inline Vector2dd Ray2d::intersection(const Ray2d &ray1, const Ray2d &ray2, double &t1, double &t2)
{
/*    SYNC_PRINT(("Ray2d::intersection((%lf %lf -> %lf %lf), (%lf %lf -> %lf %lf)): called\n",
        ray1.a.x(), ray1.a.y(),
        ray1.p.x(), ray1.p.y(),

        ray2.a.x(), ray2.a.y(),
        ray2.p.x(), ray2.p.y()
    ));*/

    Vector2dd dist = ray2.p - ray1.p;

    double ortog = ray2.a & ray1.a.leftNormal(); /*  This is det of main matrix */
    if (ortog == 0) {
        t1 = std::numeric_limits<double>::infinity();
        t2 = std::numeric_limits<double>::infinity();
//        SYNC_PRINT(("Ray2d::intersection(): no intersection\n"));
        return Vector2dd(0, 0);
    }

    t2 = -(dist & ray1.a.leftNormal()) / ortog;
    t1 = -(dist & ray2.a.leftNormal()) / ortog;

//    SYNC_PRINT(("Ray2d::intersection(): t1=%lf t2=%lf\n", t1, t2 ));
    return ray1.getPoint(t1);
}

inline Vector2dd Segment2d::intersect(const Segment2d &s1, const Segment2d &s2, bool &hasIntersection)
{
    Ray2d r1(s1);
    Ray2d r2(s2);

    double t1 = 0;
    double t2 = 0;

    //cout << "Segment2d::intersect(): " << r1 << " and " << r2 << std::endl;

    Vector2dd x = Ray2d::intersection(r1, r2, t1, t2);

    if (t1 == std::numeric_limits<double>::infinity())
    {
        hasIntersection = false;
        return Vector2dd::Zero();
    }

    if ((t1 < 0.0 || t1 > 1.0) ||
        (t2 < 0.0 || t2 > 1.0))
    {
        hasIntersection = false;
        return Vector2dd::Zero();
    }

    hasIntersection = true;
    return x;
}

} //namespace corecvs

#endif  //LINE_H_
