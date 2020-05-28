#ifndef RAYTRACEABLENODEWRAPPER_H
#define RAYTRACEABLENODEWRAPPER_H

#include "geometry/raytrace/raytraceRenderer.h"
#include "bspTree.h"

namespace corecvs {

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
        vector<Vector3dd> points = box.getPoints();
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
        vector<Vector3dd> points = box.getPoints();
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

}

#endif // RAYTRACEABLENODEWRAPPER_H
