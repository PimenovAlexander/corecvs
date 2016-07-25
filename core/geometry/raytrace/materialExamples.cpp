#include "materialExamples.h"

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

void TextureMaterial::getColor(RayIntersection &ray, RaytraceRenderer &renderer)
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
