#ifndef PLANEFRAME_H
#define PLANEFRAME_H

#include "core/math/vector/vector3d.h"
#include "core/geometry/plane.h"
#include "core/math/affine.h"

namespace corecvs {

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



} // namespace corecvs

#endif // PLANEFRAME_H
