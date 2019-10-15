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

class RaytraceableSky2 : public RaytraceableMaterial {
public:
    PerlinNoise noise;
    double skyLevel = 0.8;

    TraceColor sky  = RGBColor::Cyan().toDouble();
    TraceColor low  = RGBColor::Cyan().toDouble();
    TraceColor high = RGBColor::White().toDouble();

    virtual void getColor(RayIntersection &ray, RaytraceRenderer &renderer) override;

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


/**
 *
 *          0
 *        1 2 3 4
 *          5
 *
 *         Top
 *  Left  Front  Right  Back
 *       Bottom
 **/
class RaytraceableCubemap : public RaytraceableMaterial {
public:
    RGB24Buffer *cubemap = NULL;

    RaytraceableCubemap(RGB24Buffer *cubemap = NULL) :
        cubemap(cubemap)
    {}

    enum CubemapPart {
        TOP    = 0,
        LEFT   = 1,
        FRONT  = 2,
        RIGHT  = 3,
        BACK   = 4,
        BOTTOM = 5,

        NUMBER_OF_PARTS = 6
    };

    static inline const char *getName(const CubemapPart &value)
    {
        switch (value)
        {
         case TOP : return "TOP"; break ;
         case LEFT : return "LEFT"; break ;
         case FRONT : return "FRONT"; break ;
         case RIGHT : return "RIGHT"; break ;
         case BACK : return "BACK"; break ;
         case BOTTOM : return "BOTTOM"; break ;
         case NUMBER_OF_PARTS : return "NUMBER_OF_PARTS"; break ;
         default : return "Not in range"; break ;

        }
        return "Not in range";
    }

    static void cubeMap  (const Vector3dd &dir, CubemapPart &id, Vector2dd &uv);
    static void cubeUnmap(const CubemapPart &id, const Vector2dd &uv, Vector3dd &dir);

    virtual void getColor(RayIntersection &ray, RaytraceRenderer &renderer) override;

};

#if 0
class RaytraceableCubemap6Images : public RaytraceableMaterial {
public:
    RGB24Buffer *cubeFront  = NULL;
    RGB24Buffer *cubeTop    = NULL;
    RGB24Buffer *cubeBottom = NULL;
    RGB24Buffer *cubeLeft   = NULL;
    RGB24Buffer *cubeRight  = NULL;
    RGB24Buffer *cubeBack   = NULL;

    RaytraceableCubemap6Images(RGB24Buffer *cubemap = NULL) :
        cubemap(cubemap)
    {}

    virtual void getColor(RayIntersection &ray, RaytraceRenderer &renderer) override;

};
#endif

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
