#ifndef BSPTREE_H
#define BSPTREE_H

#include "geometry/line.h"
#include "geometry/polygons.h"
#include "geometry/axisAlignedBox.h"
#include "geometry/conic.h"
#include "geometry/raytrace/raytraceRenderer.h"

namespace corecvs {

namespace  BSPSeparatorSide {

enum BSPSeparatorSide {
    LEFT_SIDE,   /**< Definetly left side */
    RIGHT_SIDE,  /**< Definetly right side */
    MIDDLE       /**< Possibly intersect the separator */
};

}

class BSPVector3ddPointer {
public:
    Vector3dd *p;

    /** Debug only **/
    double &x() {
        return p->x();
    }
    double &y() {
        return p->y();
    }
    double &z() {
        return p->z();
    }

    const double &x() const {
        return p->x();
    }
    const double &y() const {
        return p->y();
    }
    const double &z() const {
        return p->z();
    }


    BSPVector3ddPointer(Vector3dd *p = NULL) :
        p(p)
    {}

    void addToApproximation(EllipticalApproximation3d &approx) const
    {
        approx.addPoint(*p);
    }

    AxisAlignedBox3d getBoundingBox() const
    {
        return AxisAlignedBox3d(*p, *p);
    }

    BSPSeparatorSide::BSPSeparatorSide side(const Plane3d &separator) const
    {
        double v = separator.pointWeight(*p);
        if (v < 0)
            return BSPSeparatorSide::LEFT_SIDE;
        if (v > 0)
            return BSPSeparatorSide::RIGHT_SIDE;

        return BSPSeparatorSide::MIDDLE;
    }

    double distanceTo(const BSPVector3ddPointer& v)
    {
        return (*p - *v.p).l2Metric();
    }

    friend double distanceTo(const Plane3d &plane, const BSPVector3ddPointer& v)
    {
        return plane.distanceTo(*v.p);
    }


};




template<typename RealType, typename GeometryType, class SeparatorType = Plane3d>
class BSPTreeNodeBase {
public:
/*    typedef NumTriangle3dd GeometryType;
    typedef NumPlaneFrame  CacheType;
    typedef Plane3d        SeparatorType;*/

    vector<GeometryType> submesh;

    BSPTreeNodeBase *left = NULL;
    BSPTreeNodeBase *right = NULL;
    BSPTreeNodeBase *middle = NULL;

    SeparatorType  plane;



    /** ================= Interface ===================== **/
    template<typename GeometryApproxType>
    void addToApproximation(const GeometryApproxType &element, EllipticalApproximation3d &approx)
    {
        element.addToApproximation(approx);
    }

    void addToApproximation(const Vector3dd &element, EllipticalApproximation3d &approx)
    {
        approx.addPoint(element);
    }

    void addToApproximation(const Vector3dd * &element, EllipticalApproximation3d &approx)
    {
        approx.addPoint(*element);
    }

    template<typename GeometryApproxType>
    AxisAlignedBox3d getBoundingBox(const GeometryApproxType &element)
    {
        return element.getBoundingBox();
    }

    AxisAlignedBox3d getBoundingBox(const Vector3dd &element)
    {
        return AxisAlignedBox3d(element, element);
    }

    AxisAlignedBox3d getBoundingBox(const Vector3dd * &element)
    {
        return AxisAlignedBox3d(*element, *element);
    }


    template<class GeometryApproxType>
    BSPSeparatorSide::BSPSeparatorSide side(const GeometryApproxType &element, const Plane3d &separator)
    {
        return element.side(separator);
    }

    BSPSeparatorSide::BSPSeparatorSide side(const Vector3dd &element, const Plane3d &separator)
    {
        double v = separator.pointWeight(element);
        if (v < 0)
            return BSPSeparatorSide::LEFT_SIDE;
        if (v > 0)
            return BSPSeparatorSide::RIGHT_SIDE;

        return BSPSeparatorSide::MIDDLE;
    }

    BSPSeparatorSide::BSPSeparatorSide side(const Vector3dd *element, const Plane3d &separator)
    {
        double v = separator.pointWeight(*element);
        if (v < 0)
            return BSPSeparatorSide::LEFT_SIDE;
        if (v > 0)
            return BSPSeparatorSide::RIGHT_SIDE;

        return BSPSeparatorSide::MIDDLE;
    }

    double distanceTo(const Plane3d &plane, const Vector3dd& v)
    {
        return plane.distanceTo(v);
    }

    double distanceTo(const Plane3d &plane, const BSPVector3ddPointer& v)
    {
        return plane.distanceTo(*v.p);
    }

    /** =========================================================== **/

    void prepareBounding()
    {

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
            addToApproximation(element, approx);
        }
        Vector3dd center = approx.getCenter();
        approx.getEllipseParameters();
        Vector3dd normal = approx.mAxes[0];
        plane = SeparatorType::FromNormalAndPoint(normal, center);


        static_cast<RealType *>(this)->prepareBounding();

        if (submesh.size() <= 3)
            return;

        vector<GeometryType> m;
        vector<GeometryType> l;
        vector<GeometryType> r;
        m.reserve(submesh.size() / 8.0);
        l.reserve(submesh.size() / 2.0);
        r.reserve(submesh.size() / 2.0);

        for (const GeometryType &element : submesh)
        {
            switch (side(element, plane)) {
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
            middle = new RealType;
            middle->submesh = m;
            middle->subdivide();
        }
        submesh.clear();

        if (!l.empty()) {
            left = new RealType;
            left->submesh = l;
            left->subdivide();
        }

        if (!r.empty()) {
            right = new RealType;
            right->submesh = r;
            right->subdivide();
        }
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


    void dumpToMesh(Mesh3D &mesh, int depth, bool plane, bool volume)
    {
        // mesh.addIcoSphere(bound, 3);

        if (left)   left  ->dumpToMesh(mesh, depth + 1, plane, volume);
        if (right)  right ->dumpToMesh(mesh, depth + 1, plane, volume);
        if (middle) middle->dumpToMesh(mesh, depth + 1, plane, volume);
    }


    struct Result {
        GeometryType *obj = NULL;
        double bestDist = std::numeric_limits<double>::max();
    };

    void findClosest(
            const GeometryType &point,
            Result &guess)
    {
        /*
        SYNC_PRINT(("BSPTreeNode::findClosest(_, [ %lf %s, _]): called\n",
                    guess.bestDist == std::numeric_limits<double>::max() ? 0 : guess.bestDist,
                    guess.bestDist == std::numeric_limits<double>::max() ? "{max}" : ""));
        */

        //SYNC_PRINT(("Checking %d submeshes\n", submesh.size()));
        for (GeometryType &obj: submesh)
        {
            //SYNC_PRINT(("  Checking (%lf %lf %lf)\n", obj.x(), obj.y(), obj.z() ));

            double dist = obj.distanceTo(point);
            if (dist < guess.bestDist) {
                guess.bestDist = dist;
                guess.obj = &obj;
            }
        }


        if (middle != NULL) {
            //SYNC_PRINT(("Checking middle node\n"));
            middle->findClosest(point, guess);
        }


        BSPSeparatorSide::BSPSeparatorSide  s = side(point, plane);
        double toSeparator = distanceTo(plane, point);

        //SYNC_PRINT(("Checking my side\n"));

        if (s == BSPSeparatorSide::LEFT_SIDE  && left  != NULL) left ->findClosest(point, guess);
        if (s == BSPSeparatorSide::RIGHT_SIDE && right != NULL) right->findClosest(point, guess);

        //cout << "best: " << guess.bestDist << " separator: " << toSeparator << endl;

        if (guess.bestDist <= toSeparator)
            return;

        //SYNC_PRINT(("Checking other side\n"));

        if (s == BSPSeparatorSide::LEFT_SIDE  && right != NULL) right->findClosest(point, guess);
        if (s == BSPSeparatorSide::RIGHT_SIDE && left  != NULL) left ->findClosest(point, guess);

        return;
    }


    GeometryType *findClosest(const GeometryType &request)
    {
        //SYNC_PRINT(("BSPTreeNode::findClosest(%lf %lf %lf)):called\n", request.x(), request.y(), request.z()));
        Result res;
        findClosest(request, res);
        return res.obj;
    }


    ~BSPTreeNodeBase()
    {
        delete_safe(left);
        delete_safe(right);
        delete_safe(middle);
    }
};

template<typename GeometryType, class SeparatorType = Plane3d>
class BSPTreeNode:
        public BSPTreeNodeBase<BSPTreeNode<GeometryType, SeparatorType>, GeometryType, SeparatorType>
{
};


#if 1
template<typename GeometryType, class CacheType, class SeparatorType = Plane3d>
class BSPTreeNodeRender : public BSPTreeNodeBase< BSPTreeNodeRender<GeometryType, CacheType, SeparatorType>, GeometryType, SeparatorType>
{
public:
   vector<CacheType>    cached;

   // Sphere3d bound;
   AxisAlignedBox3d box;

   BSPTreeNodeRender *getLeft()
   {
        return static_cast< BSPTreeNodeRender *>(this->left);
   }

   BSPTreeNodeRender *getRight()
   {
       return static_cast< BSPTreeNodeRender *>(this->right);
   }

   BSPTreeNodeRender *getMiddle()
   {
       return static_cast< BSPTreeNodeRender *>(this->middle);
   }


   void prepareBounding()
   {
       box = AxisAlignedBox3d::Empty();
       for (const GeometryType &element : this->submesh)
       {
           box = AxisAlignedBox3d(box, this->getBoundingBox(element));
       }
       box.outset(0.000001);

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
   }

   bool intersect(RayIntersection &intersection)
   {
       intersection.object = NULL;

       double d1,d2;
       if (!this->box.intersectWith(intersection.ray, d1, d2))
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

       if (this->getMiddle() != NULL) {
           bool result = this->getMiddle()->intersect(intersection);
           if (result) {
               if (intersection.t > 0.000001 && intersection.t < best.t) {
                   best = intersection;
               }
           }
       }

       bool side = this->plane.pointWeight(intersection.ray.p) > 0;
       BSPTreeNodeRender *closer  = side ? this->getLeft()  : this->getRight();
       BSPTreeNodeRender *further = side ? this->getRight() : this->getLeft();


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


   void cache()
   {
       cached.clear();
       for (const GeometryType &triangle : this->submesh)
       {
           cached.push_back(triangle.toCache());
       }
       if (this->getLeft()   != NULL)   this->getLeft()->cache();
       if (this->getRight()  != NULL)   this->getRight()->cache();
       if (this->getMiddle() != NULL)   this->getMiddle()->cache();

   }

};
#endif

} // namespace corecvs

#endif // BSPTREE_H

