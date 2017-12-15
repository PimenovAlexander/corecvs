#ifndef SDFRENDERABLEOBJECTS_H
#define SDFRENDERABLEOBJECTS_H

#include "core/function/function.h"

#include "core/math/vector/vector3d.h"
#include "core/geometry/raytrace/sdfRenderable.h"

class SDFRenderableSphere : public SDFRenderable{
    Vector3dd sphere;
    double r;

public:
    SDFRenderableSphere(const Vector3dd &sphere, double r);
};

class SDFRenderableEllipsoid : public SDFRenderable{
    Vector3dd pos;
    Vector3dd dim;

public:
    SDFRenderableEllipsoid(const Vector3dd &pos, const Vector3dd &dim);
};

class SDFRenderableBox : public SDFRenderable{
    Vector3dd pos;
    Vector3dd dim;

public:
    SDFRenderableBox(const Vector3dd &pos, const Vector3dd dim);
};

class SDFRenderableRoundedBox : public SDFRenderable{
    Vector3dd pos;
    Vector3dd dim;
    double r;
public:
    SDFRenderableRoundedBox(const Vector3dd &pos, const Vector3dd dim, double r);
};

class SDFRenderableTorus : public SDFRenderable{
    Vector3dd pos;
    Vector2d<double> dim;

public:
    SDFRenderableTorus(const Vector3dd &pos, const Vector2d<double> dim);
};

class SDFRenderableTorus88 : public SDFRenderable{
    Vector3dd pos;
    Vector2d<double> dim;

public:
    SDFRenderableTorus88(const Vector3dd &pos, const Vector2d<double> dim);
};

class SDFRenderableTorus82 : public SDFRenderable{
    Vector3dd pos;
    Vector2d<double> dim;

public:
    SDFRenderableTorus82(const Vector3dd &pos, const Vector2d<double> dim);
};

class SDFRenderableCone: public SDFRenderable{
    Vector3dd pos;
    Vector3dd dim;

public:
    SDFRenderableCone(const Vector3dd &pos, const Vector3dd dim);
};

class SDFRenderableCylinder : public SDFRenderable{
    Vector3dd pos;
    Vector2d<double> dim;

public:
    SDFRenderableCylinder(const Vector3dd &pos, const Vector2d<double> dim);
};

class SDFRenderableSubstraction : public SDFRenderable{
    SDFRenderable d1;
    SDFRenderable d2;

public:
    SDFRenderableSubstraction(const SDFRenderable &d1, const SDFRenderable &d2);
};

class SDFRenderableUnion : public SDFRenderable{
    SDFRenderable d1;
    SDFRenderable d2;

public:
    SDFRenderableUnion(const SDFRenderable &d1, const SDFRenderable &d2);
};

class SDFRenderableBlend : public SDFRenderable{
    SDFRenderable d1;
    SDFRenderable d2;

public:
    SDFRenderableBlend(const SDFRenderable &d1, const SDFRenderable &d2);
};
#endif // SDFRENDERABLEOBJECTS_H

