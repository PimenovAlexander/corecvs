#ifndef MATERIALEXAMPLES_H
#define MATERIALEXAMPLES_H

#include "core/geometry/raytrace/perlinNoise.h"
#include "core/geometry/raytrace/raytraceRenderer.h"

namespace corecvs {

class BumpyMaterial : public  RaytraceableMaterial
{
public:
    double power;
    double scale;

    PerlinNoise noise;
    BumpyMaterial(double power = 1.0 / 10.0, double scale = 2.0) :
        RaytraceableMaterial()
      , power(power)
      , scale(scale)
    {}

    void getColor(RayIntersection &ray, RaytraceRenderer &renderer);
};

class TextureMaterial : public  RaytraceableMaterial
{
public:
    RGB24Buffer *texture = NULL;
    TextureMaterial()
    {}
    void getColor(RayIntersection &ray, RaytraceRenderer &renderer);
};

class RaytraceableSky1 : public RaytraceableMaterial {
public:
    PerlinNoise noise;
    double skyLevel = 0.8;

    TraceColor sky  = RGBColor::Cyan().toDouble();
    TraceColor low  = RGBColor::Cyan().toDouble();
    TraceColor high = RGBColor::White().toDouble();

    virtual void getColor(RayIntersection &ray, RaytraceRenderer &renderer) override;

};

class RaytraceableCubemap : public RaytraceableMaterial {
public:
    RGB24Buffer *cubemap = NULL;

    RaytraceableCubemap(RGB24Buffer *cubemap = NULL) :
        cubemap(cubemap)
    {}

    virtual void getColor(RayIntersection &ray, RaytraceRenderer &renderer) override;

};

class MaterialExamples
{
public:
    MaterialExamples();

    static RaytraceableMaterial *ex1();
    static RaytraceableMaterial *ex2();

    static RaytraceableMaterial *ex3(const RGBColor &color = RGBColor::Yellow());

    static RaytraceableMaterial *bumpy();
    static RaytraceableMaterial *glass(double dens = 1.2);

    static RaytraceableMaterial *texture();

};



} // namespace corecvs

#endif // MATERIALEXAMPLES_H
