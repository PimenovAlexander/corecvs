#ifndef CONVEXQUICKHULL_H
#define CONVEXQUICKHULL_H


#include <algorithm>
#include <array>
#include <queue>
#include <vector>


#include "core/math/vector/vector3d.h"
#include "core/geometry/polygons.h"

namespace corecvs {

class Mesh3D;

class ConvexQuickHull
{
public:
    typedef std::vector<Vector3dd> Vertices;

    struct HullFace {
        HullFace(const Vector3dd &p1, const Vector3dd &p2, const Vector3dd &p3) :
            plane(p1,p2,p3),
            normal(plane.getNormal().normalised())
        {}

        HullFace(const Triangle3dd &t) :
            plane(t),
            normal(plane.getNormal().normalised())
        {}

        HullFace(const std::initializer_list<Vector3dd> &list)
        {
            if (list.size() != 3)
                return;
            auto it = list.begin();

            plane.p[0] = *(it);
            plane.p[1] = *(it + 1);
            plane.p[2] = *(it + 2);
        }

        Triangle3dd plane;
        Vector3dd  normal;

        bool toDelete = false;
        Vertices points;

        double getBasedVolume(const Vector3dd &origin)
        {
            double par = ((origin - plane.p1()) ^ (plane.p2() - plane.p1())) & (plane.p3() - plane.p1());
            return par / 6.0;
        }

        Plane3d getPlane() const
        {
            return Plane3d::FromNormalAndPoint(normal, plane.p1());
        }
    };

    class HullFaces : public std::vector<HullFace> {
    public:
        HullFaces() {}

        HullFaces(std::initializer_list<HullFace> list) :
            std::vector<HullFace>(list)
        {
        }

        HullFaces(Mesh3D &mesh);

        double getVolume()
        {
            if (size() <= 4) {
                return 0.0;
            }

            double res = 0.0;
            Vector3dd origin = front().plane.p1();
            for (size_t  i = 1; i < size(); i++)
            {
                res += std::abs(operator [](i).getBasedVolume(origin));
            }
            return res;
        }

        void addToMesh(Mesh3D &mesh, bool drawNormals = false);

        bool isInside(const Vector3dd &p) const
        {
            for (const HullFace &face: *this )
            {
                if (face.getPlane().pointWeight(p) >= 0)
                    return false;
            }
            return true;
        }

        /* Points could be repeated */
        vector<Vector3dd> getPoints(bool dedup = true) const;

        /* */
        void reorientNormals(int targetSign = 0);

        /* Clip interface */
        Vector3dd getPoint(int i) const
        {
            return operator [](i).plane.p1();
        }

        Vector3dd getNormal(int i) const
        {
            return operator [](i).normal;
        }
    };
    static HullFaces quickHull(const Vertices& listVertices, double epsilon = 1e-7);

    static HullFaces intersect(const HullFaces &v1,  const HullFaces &v2);

protected:

    static double pointFaceDist(const Triangle3dd &face, const Vector3dd &point);

    static bool faceIsVisible(const Vector3dd &eyePoint, const HullFace &face, double eps);

    static void addPointsToFaces(HullFace* faces, unsigned long faces_count, const Vertices &listVertices, double eps);

};

} // namespace corecvs

#endif // CONVEXQUICKHULL_H
