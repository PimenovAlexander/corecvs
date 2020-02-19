#ifndef RAYTRACERENDERER_H
#define RAYTRACERENDERER_H

#include "geometry/polygons.h"
#include "cameracalibration/cameraModel.h"
#include "geometry/mesh/mesh3d.h"
#include "geometry/line.h"
#include "cameracalibration/projection/projectionModels.h"

namespace corecvs {

typedef Vector3dd TraceColor;

class Raytraceable;
class RaytraceRenderer;

class RayIntersection {
public:
    Raytraceable *object = NULL;
    int payload = -1;
    Ray3d ray;

    Vector2dd uvCoord;
    Vector2dd texCoord;
    Vector3dd normal;
    double t = 0;
    double weight = 1.0;
    int depth = 0;

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

class RaytraceableSky : public RaytraceableMaterial {
public:
    TraceColor low  = RGBColor::Black().toDouble();
    TraceColor high = RGBColor::Cyan().toDouble();
    Vector3dd direction = Vector3dd::OrtY();

    virtual void getColor(RayIntersection &ray, RaytraceRenderer &) override;

};


class RaytraceableChessMaterial : public RaytraceableMaterial {
public:
    double cellSize;

    RaytraceableChessMaterial(double cellSize = 10.0) :
        cellSize(cellSize)
    {}

    virtual void getColor(RayIntersection &ray, RaytraceRenderer &renderer) override;
};


class Raytraceable {
public:
    std::string name;

    TraceColor color;
    RaytraceableMaterial *material = NULL;

    /**
     *  This function updates the intersection with
     *  t - closest intersection
     *  object - object that will then handle the material
     *  payload - optional payload
     **/
    virtual bool intersect(RayIntersection &intersection) = 0;

    virtual void normal(RayIntersection &intersection);
    virtual bool inside (Vector3dd &point) = 0;

    /* Helper function to help debuging */
    virtual bool toMesh (Mesh3D & /*target*/) { return false; }

    virtual AxisAlignedBox3d getBoundingBox(); /* < not mandtory*/

    virtual ~Raytraceable();
};



class RaytraceRenderer
{
private:
    CameraProjection *projection = NULL;

public:

    typedef AbstractBuffer<TraceColor> ColorBuffer;
    typedef AbstractBuffer<int> MarkupType;

    //PinholeCameraIntrinsics intrisics;

    Affine3DQ position;

    Raytraceable *object = NULL; /**< Owned by renderer */
    vector<RaytraceablePointLight *> lights;
    TraceColor ambient;
    RaytraceableMaterial *sky = NULL;

    /* Ray trace end condition */
    int maxDepth = 4;
    double minWeight = 1.0 / 255.0;

    /* Renderer global parameters */
    bool supersample = false;
    int  sampleNum = 20;   

    bool parallel = true;
    bool traceProgress = true;

    /**/
    AbstractBuffer<TraceColor> *energy = NULL;
    AbstractBuffer<int> *markup = NULL;

    int currentY, currentX;

    RaytraceRenderer();
    ~RaytraceRenderer();

    void trace(RayIntersection &intersection);
    void trace(RGB24Buffer *buffer);

    void traceFOV(RGB24Buffer *buffer, double apperture, double focus);

/* Getters and setters */
    void setProjection(CameraProjection *projection); /**< takes ownership */

private:
    void pack(RGB24Buffer *target, ColorBuffer *energy, MarkupType *markup = NULL);

};

} // namespace corecvs

#endif // RAYTRACERENDERER_H
