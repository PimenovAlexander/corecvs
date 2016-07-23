#ifndef RAYTRACERENDERER_H
#define RAYTRACERENDERER_H

#include "polygons.h"
#include "calibrationCamera.h"
#include "mesh3d.h"
#include "line.h"

namespace corecvs {

typedef Vector3dd TraceColor;

class Raytraceable;
class RaytraceRenderer;

class RayIntersection {
public:
    Raytraceable *object = NULL;
    Ray3d ray;

    Vector2dd texCoord;
    Vector3dd normal;
    double t;

//    Ray3d reflection;
//    Ray3d refraction;

    double weight;
    int depth;

    /* This needs to be separated */
    TraceColor ownColor;
    Vector3dd getPoint();

};

class RaytraceablePointLight {
public:
    TraceColor color;
    Vector3dd position;

    RaytraceablePointLight(
        TraceColor _color,
        Vector3dd _position
    ) : color(_color),
        position(_position)
    {}

    virtual bool checkRay(RayIntersection & /*ray*/) {
        return true;
    }
};

class RaytraceableMaterial {
public:
    double reflCoef = 0.0;
    double refrCoef = 0.0;

    double opticalDens = 1.0;

    TraceColor ambient  = TraceColor::Zero();
    TraceColor diffuse  = TraceColor::Zero();
    TraceColor specular = TraceColor::Zero();
    double specPower = 4.0;

    virtual void getColor(RayIntersection &ray, RaytraceRenderer &renderer);
    virtual Ray3d getReflection(RayIntersection &ray);
    virtual Ray3d getRefraction(RayIntersection &ray);
};

class RaytraceableChessMaterial : public RaytraceableMaterial {
public:
    virtual void getColor(RayIntersection &ray, RaytraceRenderer &renderer);
};


class Raytraceable {
public:
    std::string name;

    TraceColor color;
    RaytraceableMaterial *material = NULL;

    virtual bool intersect(RayIntersection &intersection) = 0;
    virtual void normal(const Vector3dd &vector, Vector3dd &normal) = 0;
    virtual bool inside (Vector3dd &point) = 0;

    virtual ~Raytraceable();
};



class RaytraceRenderer
{
public:
    PinholeCameraIntrinsics intrisics;
    Affine3DQ position;

    Raytraceable *object;
    vector<RaytraceablePointLight *> lights;
    TraceColor ambient;

    /* Exit condition */
    int maxDepth = 4;
    double minWeight = 1.0 / 255.0;

    bool supersample = false;
    int  sampleNum = 20;

    bool parallel = true;

    /**/
    AbstractBuffer<TraceColor> *energy = NULL;
    AbstractBuffer<int> *markup = NULL;

    int currentY, currentX;

    RaytraceRenderer();

    void trace(RayIntersection &intersection);
    void trace(RGB24Buffer *buffer);

    void traceFOV(RGB24Buffer *buffer, double apperture, double focus);


};

} // namespace corecvs

#endif // RAYTRACERENDERER_H
