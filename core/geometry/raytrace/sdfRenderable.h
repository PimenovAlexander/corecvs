#ifndef SDFRENDERABLE_H
#define SDFRENDERABLE_H

#include "core/function/function.h"

#include "core/math/vector/vector3d.h"
#include "core/geometry/raytrace/raytraceObjects.h"


class SDFRenderable : public Raytraceable
{
public:
    SDFRenderable();

    std::function<double(Vector3dd)> F;

    // Raytraceable interface
    bool intersect(RayIntersection &intersection);
    void normal(const Vector3dd &vector, Vector3dd &normal);
    bool inside(Vector3dd &point);
};

#endif // SDFRENDERABLE_H
