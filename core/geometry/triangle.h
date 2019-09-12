#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "core/geometry/line.h"
#include "core/geometry/planeFrame.h"

namespace corecvs {


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

}
#endif // TRIANGLE_H
