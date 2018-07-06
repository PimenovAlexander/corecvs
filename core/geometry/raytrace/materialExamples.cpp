#include "core/geometry/raytrace/materialExamples.h"

MaterialExamples::MaterialExamples()
{

}

RaytraceableMaterial *MaterialExamples::ex1()
{
    RaytraceableMaterial *blueMirror = new RaytraceableMaterial;
    blueMirror->ambient = RGBColor::Blue().toDouble() * 0.2 ;
    blueMirror->diffuse = RGBColor::White().toDouble() / 255.0;
    blueMirror->reflCoef = 0.3;
    blueMirror->refrCoef = 0;
    blueMirror->opticalDens = 1.3;
    blueMirror->specular = RGBColor::Blue().toDouble() / 255.0;
    return blueMirror;
}

RaytraceableMaterial *MaterialExamples::ex2()
{
    RaytraceableMaterial *blueMirror = new RaytraceableMaterial;
    blueMirror->ambient = RGBColor::White().toDouble() * 0.2 ;
    blueMirror->diffuse = RGBColor::White().toDouble() / 255.0;
    blueMirror->reflCoef = 0.3;
    blueMirror->refrCoef = 0;
    blueMirror->opticalDens = 1.3;
    blueMirror->specular = RGBColor::Red().toDouble() / 255.0;
    return blueMirror;
}

RaytraceableMaterial *MaterialExamples::ex3(const RGBColor &color)
{
    RaytraceableMaterial *generic = new RaytraceableMaterial;
    generic->ambient = RGBColor::White().toDouble() * 0.2 ;
    generic->diffuse = color.toDouble() / 255.0;
    generic->reflCoef = 0.3;
    generic->refrCoef = 0;
    generic->opticalDens = 1.3;
    generic->specular = color.toDouble() / 255.0;
    return generic;
}

RaytraceableMaterial *MaterialExamples::bumpy()
{
    BumpyMaterial * bumpy = new BumpyMaterial;

    bumpy->ambient = RGBColor::Red().toDouble() * 0.2 ;
    bumpy->diffuse = RGBColor::White().toDouble() / 255.0;
    bumpy->reflCoef = 0.3;
    bumpy->refrCoef = 0.0;
    bumpy->specular = RGBColor(250.0, 220.0, 255.0).toDouble() / 255.0;

    return bumpy;
}

RaytraceableMaterial *MaterialExamples::glass(double dens)
{
    RaytraceableMaterial *glass = new RaytraceableMaterial();
    glass->ambient = RGBColor::Red().toDouble() * 0.03 ;
    glass->diffuse = RGBColor::White().toDouble() / 255.0;
    glass->reflCoef = 0.0;
    glass->refrCoef = 0.7;
    glass->opticalDens = dens;
    glass->specular = RGBColor::Gray(120.0).toDouble() / 255.0;

    return glass;
}

RaytraceableMaterial *MaterialExamples::texture()
{
    TextureMaterial *texture = new TextureMaterial();
    return texture;
}



void BumpyMaterial::getColor(RayIntersection &ray, RaytraceRenderer &renderer)
{
    Vector3dd oldN = ray.normal;

    Vector3dd bumped = noise.turbulence3d(ray.getPoint() / scale);

    ray.normal += (bumped - Vector3dd(0.5)) * power;
    ray.normal.normalise();
    double k = ray.normal & ray.ray.a;
    if (k > 0){
        ray.normal = oldN;
    }
    RaytraceableMaterial::getColor(ray, renderer);
}

void TextureMaterial::getColor(RayIntersection &ray, RaytraceRenderer &/*renderer*/)
{
    ray.ownColor = RGBColor::Yellow().toDouble();
    if (texture == NULL) {
        return;
    }


    Vector2dd coords = Vector2dd(ray.texCoord.x(), 1.0 - ray.texCoord.y()) * Vector2dd(texture->w, texture->h);


    if (!texture->isValidCoordBl(coords)) {
        SYNC_PRINT(("invalid tex coords"));
        return;
    }

    //SYNC_PRINT(("tex coords [%lf %lf]\n", coords.x(), coords.y()));
    ray.ownColor = texture->elementBl(coords).toDouble();
}

void RaytraceableSky1::getColor(RayIntersection &ray, RaytraceRenderer &/*renderer*/)
{
    double v = noise.turbulence(ray.ray.a * 10.0);
    if (v <= skyLevel) {
        ray.ownColor = sky;
    } else {
        ray.ownColor = lerpLimit(low, high, v, skyLevel, 2.0);
    }

    //ray.ownColor;
}

void RaytraceableSky2::getColor(RayIntersection &ray, RaytraceRenderer &renderer)
{
    CORE_UNUSED(renderer);

    double v = noise.turbulence(ray.ray.a * 10.0);
    ray.ownColor = RGBColor::rainbow1(v / 2.0).toDouble();

}



void RaytraceableCubemap::getColor(RayIntersection &ray, RaytraceRenderer &/*renderer*/)
{
    int p = 0;
    Vector2dd uv = Vector2dd::Zero();
    Vector3dd d = ray.ray.a.normalised();
    Vector3dd a = Vector3dd(fabs(d.x()), fabs(d.y()), fabs(d.z()));

    ray.ownColor = RGBColor::Magenta().toDouble();

    /* X */
    if (d.x() > 0 && a.x() > a.y() && a.x() > a.z())
    {
        p = 3;
        uv = Vector2dd(-d.z(), d.y()) / a.x() ;
    }

    if (d.x() < 0 && a.x() > a.y() && a.x() > a.z())
    {
        p = 1;
        uv = Vector2dd(d.z(), d.y()) / a.x();
    }

    /* Y */
    if (d.y() > 0 && a.y() > a.x() && a.y() > a.z())
    {
        p = 5;
        uv = Vector2dd(d.x(), -d.z()) / a.y();
    }

    if (d.y() < 0 && a.y() > a.x() && a.y() > a.z())
    {
        p = 0;
        uv = Vector2dd(d.x(), d.z()) / a.y();
    }

    /* Z */
    if (d.z() > 0 && a.z() > a.y() && a.z() > a.x())
    {
        p = 2;
        uv = d.xy() / a.z();
    }

    if (d.z() < 0 && a.z() > a.y() && a.z() > a.x())
    {
        p = 4;
        uv = Vector2dd(-d.x(), d.y()) / a.z();
    }

    uv += Vector2dd(1.0, 1.0);
    uv /= 2.0;


    if (cubemap == NULL || p < 0 || p >= 6) {
        ray.ownColor = RGBColor::rainbow1( p / 6.0).toDouble();
    } else {
        Vector2dd map[6] = {
            Vector2dd( 1 / 4.0, 0 / 3.0),
            Vector2dd( 0 / 4.0, 1 / 3.0),
            Vector2dd( 1 / 4.0, 1 / 3.0),
            Vector2dd( 2 / 4.0, 1 / 3.0),
            Vector2dd( 3 / 4.0, 1 / 3.0),
            Vector2dd( 1 / 4.0, 2 / 3.0)
        };
        Vector2dd coor = (map[p] + Vector2dd(1 / 4.0, 1 / 3.0) * uv) * Vector2dd(cubemap->w - 1, cubemap->h - 1);
        if (cubemap->isValidCoordBl(coor)) {
            ray.ownColor = cubemap->elementBl(coor).toDouble() * 2.0;
        }

    }

}

