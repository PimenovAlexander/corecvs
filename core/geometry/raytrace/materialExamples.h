#ifndef MATERIALEXAMPLES_H
#define MATERIALEXAMPLES_H

#include "perlinNoise.h"
#include "raytraceRenderer.h"

namespace corecvs {

class BumpyMaterial : public  RaytraceableMaterial
{
public:
    double power;
    double scale;

    PerlinNoise noise;
    BumpyMaterial(double power = 1.0 / 10.0, double scale = 2.0) :
        power(power),
        scale(scale),
        RaytraceableMaterial()
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


class MaterialExamples
{
public:
    MaterialExamples();

    static RaytraceableMaterial *ex1();
    static RaytraceableMaterial *bumpy();
    static RaytraceableMaterial *glass(double dens = 1.2);

    static RaytraceableMaterial *texture();

};

} // namespace corecvs

#endif // MATERIALEXAMPLES_H
