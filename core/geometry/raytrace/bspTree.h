#ifndef BSPTREE_H
#define BSPTREE_H

#include "core/geometry/line.h"
#include "core/geometry/polygons.h"
#include "core/geometry/axisAlignedBox.h"
#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/conic.h"

namespace corecvs {

namespace  BSPSeparatorSide {

enum BSPSeparatorSide {
    LEFT_SIDE,   /**< Definetly left side */
    RIGHT_SIDE,  /**< Definetly right side */
    MIDDLE       /**< Possibly intersect the separator */
};

}


class RaytraceableNodeWrapper {
public:
    Raytraceable *wrapee = NULL;

    RaytraceableNodeWrapper(Raytraceable *wrapee) :
        wrapee(wrapee)
    {}

    bool intersect(RayIntersection &intersection)
    {
        return wrapee->intersect(intersection);
    }

    RaytraceableNodeWrapper toCache() const
    {
        return *this;
    }

    BSPSeparatorSide::BSPSeparatorSide side(const Plane3d &separator) const
    {
        AxisAlignedBox3d box = wrapee->getBoundingBox();
        vector<Vector3dd> points = box.getPointsVector();
        bool isLeft  = true;
        bool isRight = true;
        for (size_t i = 0; i < points.size(); i++)
        {
            double v = separator.pointWeight(points[i]);
            if (v <= 0) isLeft = false;
            if (v >= 0) isRight = false;
        }
        if (isLeft ) return BSPSeparatorSide::LEFT_SIDE;
        if (isRight) return BSPSeparatorSide::RIGHT_SIDE;
        return BSPSeparatorSide::MIDDLE;
    }

    void addToApproximation(EllipticalApproximation3d &v) const
    {
        AxisAlignedBox3d box = wrapee->getBoundingBox();
        vector<Vector3dd> points = box.getPointsVector();
        for (size_t i = 0; i < points.size(); i++)
        {
            v.addPoint(points[i]);
        }

    }

    AxisAlignedBox3d getBoundingBox() const
    {
        return wrapee->getBoundingBox();
    }
};

template<class GeometryType, class CacheType, class SeparatorType = Plane3d>
class BSPTreeNode {
public:
/*    typedef NumTriangle3dd GeometryType;
    typedef NumPlaneFrame  CacheType;
    typedef Plane3d        SeparatorType;*/

    vector<GeometryType> submesh;
    vector<CacheType>    cached;

    BSPTreeNode *left = NULL;
    BSPTreeNode *right = NULL;
    BSPTreeNode *middle = NULL;

    Sphere3d bound;
    AxisAlignedBox3d box;
    SeparatorType  plane;

    bool intersect(RayIntersection &intersection)
    {
        intersection.object = NULL;

        double d1,d2;
        if (!box.intersectWith(intersection.ray, d1, d2))
            return false;

        if (d2 <= 0)
            return false;


        RayIntersection best = intersection;
        best.t = std::numeric_limits<double>::max();

        for (CacheType &triangle : cached)
        {
            if (!triangle.intersect(intersection))
                continue;
            if (intersection.t < best.t) {
                best = intersection;
            }
        }

        if (middle != NULL) {
            bool result = middle->intersect(intersection);
            if (result) {
                if (intersection.t > 0.000001 && intersection.t < best.t) {
                    best = intersection;
                }
            }
        }

        bool side = plane.pointWeight(intersection.ray.p) > 0;
        BSPTreeNode *closer  = side ? left : right;
        BSPTreeNode *further = side ? right : left;


        if (closer != NULL) {
            bool result = closer->intersect(intersection);
            if (result) {
                if (intersection.t > 0.000001 && intersection.t < best.t) {
                    best = intersection;
                }
            }
        }

        if (further != NULL) {
            bool result = further->intersect(intersection);
            if (result) {
                if (intersection.t > 0.000001 && intersection.t < best.t) {
                    best = intersection;
                }
            }
        }

        if (best.t != std::numeric_limits<double>::max()) {
            intersection = best;
            return true;
        }
        return false;
    }

    void subdivide()
    {
        //SYNC_PRINT(("BSPTreeNode::subdivide() : %u nodes", middle.size()));
        if (submesh.size() == 0)
        {
            SYNC_PRINT(("BSPTreeNode::subdivide() : empty node\n"));
            return;
        }

        EllipticalApproximation3d approx;
        for (const GeometryType &element : submesh)
        {
            element.addToApproximation(approx);
        }
        Vector3dd center = approx.getCenter();
        approx.getEllipseParameters();
        Vector3dd normal = approx.mAxes[0];
        plane = SeparatorType::FromNormalAndPoint(normal, center);


        box = AxisAlignedBox3d::Empty();
        for (const GeometryType &element : submesh)
        {
            box = AxisAlignedBox3d(box, element.getBoundingBox());
        }

       /* double radius = 0;
        for (const GeometryType &triangle : submesh)
        {
            for (int p = 0; p < 3; p++)
            {
                double d = (triangle.p[p] - center).l2Metric();
                if (radius < d)
                    radius = d;
            }
        }
        bound = Sphere3d(center, radius + 0.000001);*/
        box.outset(0.000001);

        if (submesh.size() <= 3)
            return;

        vector<GeometryType> m;
        vector<GeometryType> l;
        vector<GeometryType> r;
        for (const GeometryType &element : submesh)
        {
            switch (element.side(plane)) {
                case BSPSeparatorSide::LEFT_SIDE:  l.push_back(element); break;
                case BSPSeparatorSide::RIGHT_SIDE: r.push_back(element); break;
                case BSPSeparatorSide::MIDDLE :    m.push_back(element); break;
            }
        }

        /* Check if there was a subdivison actually */
        if (m.size() == submesh.size())
            return;
        if (l.size() == submesh.size())
            return;
        if (r.size() == submesh.size())
            return;


        //submesh = m;
        delete_safe(middle);
        delete_safe(left);
        delete_safe(right);

        //SYNC_PRINT(("RaytraceableOptiMesh::TreeNode::subdivide() : groups %u (%u | %u) nodes\n", m.size(), l.size(), r.size()));

        if (!m.empty()) {
            middle = new BSPTreeNode;
            middle->submesh = m;
            middle->subdivide();
        }
        submesh.clear();

        if (!l.empty()) {
            left = new BSPTreeNode;
            left->submesh = l;
            left->subdivide();
        }

        if (!r.empty()) {
            right = new BSPTreeNode;
            right->submesh = r;
            right->subdivide();
        }
    }

    void cache()
    {
        cached.clear();
        for (const GeometryType &triangle : submesh)
        {
            cached.push_back(triangle.toCache());
        }
        if (left   != NULL)   left->cache();
        if (right  != NULL)  right->cache();
        if (middle != NULL) middle->cache();

    }

    int childCount()
    {
        int sum = 1;
        if (left) {
            sum += left->childCount();
        }
        if (right) {
            sum += right->childCount();
        }
        if (middle) {
            sum += middle->childCount();
        }
        return sum;
    }


    int elementCount()
    {
        int sum = (int)submesh.size();
        if (left) {
            sum += left->elementCount();
        }
        if (right) {
            sum += right->elementCount();
        }
        if (middle) {
            sum += middle->elementCount();
        }
        return sum;
    }


   /* double expectedMeanDepth(int depth = 0)
    {
        depth++;
        double sum = (int)submesh.size() * depth;

        if (left) {
            sum += left->elementCount();
        }
        if (right) {
            sum += right->elementCount();
        }
        if (middle) {
            sum += middle->elementCount();
        }
        return sum;
    }*/


    void dumpToMesh(Mesh3D &mesh, int depth, bool plane, bool volume)
    {
        mesh.addIcoSphere(bound, 3);

        if (left)   left  ->dumpToMesh(mesh, depth + 1, plane, volume);
        if (right)  right ->dumpToMesh(mesh, depth + 1, plane, volume);
        if (middle) middle->dumpToMesh(mesh, depth + 1, plane, volume);
    }


    ~BSPTreeNode()
    {
        delete_safe(left);
        delete_safe(right);
        delete_safe(middle);
    }
};

} // namespace corecvs

#endif // BSPTREE_H

