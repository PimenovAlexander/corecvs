#ifndef CORE_PROJECTIVECONVEXQUICKHULL_H
#define CORE_PROJECTIVECONVEXQUICKHULL_H

#include <algorithm>
#include <array>
#include <queue>
#include <vector>
#include <numeric>

#include "core/math/vector/vector3d.h"
#include "core/math/vector/vector4d.h"
#include "core/geometry/polygons.h"

namespace corecvs {

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template<typename ElementType>
class ProjectiveCoord : public Vector4d<ElementType> {
    typedef Vector4d<ElementType> BaseClass;
public:
    ProjectiveCoord(const ElementType &_x, const ElementType &_y, const ElementType &_z, const ElementType &_w) : BaseClass(_x, _y, _z, _w) {}
    ProjectiveCoord(const BaseClass &V) : BaseClass(V) {}
    ProjectiveCoord(const ProjectiveCoord<ElementType> &V) : BaseClass(V) {}
    ProjectiveCoord(): BaseClass() {}
    ProjectiveCoord(const Vector3d<ElementType> &V, const ElementType &x) : BaseClass(V,x) {}

    bool operator==(const ProjectiveCoord &coord) const {
        if (this->w() == 0 && coord.w() == 0)
            return (sgn(this->x()) == sgn(coord.x())) && (sgn(this->y()) == sgn(coord.y())) && (sgn(this->z()) == sgn(coord.z()));
        if (this->w() != 0 && coord.w() != 0)
            return this->toVector() == coord.toVector();
        return false;
    }

    ElementType X() const {
        if (this->x() == 0)
            return 0;
        if (this->w() != 0)
            return this->x() / this->w();
        if (this->x() < 0)
            return std::numeric_limits<ElementType>::min();
        return std::numeric_limits<ElementType>::max();
    }

    ElementType Y() const {
        if (this->y() == 0)
            return 0;
        if (this->w() != 0)
            return this->y() / this->w();
        if (this->y() < 0)
            return std::numeric_limits<ElementType>::min();
        return std::numeric_limits<ElementType>::max();
    }

    ElementType Z() const {
        if (this->z() == 0)
            return 0;
        if (this->w() != 0)
            return this->z() / this->w();
        if (this->z() < 0)
            return std::numeric_limits<ElementType>::min();
        return std::numeric_limits<ElementType>::max();
    }

    Vector3d<ElementType> toVector() const {
        const auto inf = std::numeric_limits<ElementType>::max();
        if (this->w() == 0)
            return Vector3d<ElementType>(sgn(this->x()) * inf, sgn(this->y()) * inf, sgn(this->z()) * inf);
        return Vector3d<ElementType>(this->x() / this->w(),
                this->y() / this->w(), this->z() / this->w());
    }

    ElementType len() const {
        if (this->w() != 0)
            return this->project().l2Metric();
        return std::numeric_limits<ElementType>::max();
    }

    ElementType dist() const {
        if (this->w() == 0)
            return this->xyz().l2Metric();
        return std::numeric_limits<ElementType>::max();
    }
};

typedef ProjectiveCoord<double> ProjectiveCoord4d;


class Triangle4dd : public GenericTriangle<ProjectiveCoord4d>
{
public:
    Triangle4dd() = default;
    Triangle4dd(const ProjectiveCoord4d &_p1, const ProjectiveCoord4d &_p2, const ProjectiveCoord4d &_p3)
    : GenericTriangle(_p1, _p2, _p3) {}

    Vector3dd getNormal() const {
        return Plane3d::NormalFromPoints(p1().toVector(), p2().toVector(), p3().toVector());
    }

    bool operator ==(const Triangle4dd &b) const {
        vector<ProjectiveCoord4d> plane1 = { b.p1(), b.p2(), b.p3() };
        auto findX = find(plane1.begin(), plane1.end(), p1());
        auto findY = find(plane1.begin(), plane1.end(), p2());
        auto findZ = find(plane1.begin(), plane1.end(), p3());
        return (findX != plane1.end()) && (findY != plane1.end()) && (findZ != plane1.end());
    }

    bool operator !=(const Triangle4dd &b) const {
        return !(*this == b);
    }
};

class ProjectiveConvexQuickHull
{
public:
    typedef std::vector<ProjectiveCoord4d> Vertices;

    struct HullFace {
        Triangle4dd plane;
        bool toDelete = false;
        Vertices points;

        HullFace(const ProjectiveCoord4d &p1, const ProjectiveCoord4d &p2, const ProjectiveCoord4d &p3) : plane(p1,p2,p3) {}

        bool operator==(const HullFace &b) {
            return plane == b.plane;
        }
    };

    class HullFaces : public std::vector<HullFace> {
    public:
        HullFaces() = default;

        HullFaces(std::initializer_list<HullFace> list) :
                std::vector<HullFace>(list) {}
    };

    static HullFaces quickHull(const Vertices& listVertices, double epsilon = 1e-7);
};

} // namespace corecvs

#endif //CORE_PROJECTIVECONVEXQUICKHULL_H
