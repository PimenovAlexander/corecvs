#include "core/geometry/raytrace/sdfRenderableObjects.h"
#include "core/math/vector/vectorUtils.h"

SDFRenderableSphere::SDFRenderableSphere(const Vector3dd &sphere, double r) :
    sphere(sphere),
    r(r)
{
    this->F = [this](Vector3dd v) {
        return lengthVec3dd(v - this->sphere) - this->r;
    };
}

SDFRenderableEllipsoid::SDFRenderableEllipsoid(const Vector3dd &pos, const Vector3dd &dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd p = v - this->pos;
        Vector3dd r = this->dim;
        return (lengthVec3dd( p/r ) - 1.0) * std::min(std::min(r.x(),r.y()),r.z());
    };
}


SDFRenderableBox::SDFRenderableBox(const Vector3dd &pos, const Vector3dd dim):
     pos(pos),
     dim(dim)
 {
     this->F = [this](Vector3dd v) {
         Vector3dd d = absVec3dd(v -this-> pos) - this->dim;
         return std::min(std::max(d.x(),std::max(d.y(),d.z())),0.0)
                 + lengthVec3dd(maxVec3dd(d,Vector3dd(0.0, 0.0, 0.0)));
     };
 }

SDFRenderableRoundedBox::SDFRenderableRoundedBox(
        const Vector3dd &pos, const Vector3dd dim, double r):
    pos(pos),
    dim(dim),
    r(r)
{
    this->F = [this](Vector3dd v) {
        Vector3dd d = (v - this-> pos);
        return lengthVec3dd(maxVec3dd(absVec3dd(d)-this->dim,Vector3dd(0.0, 0.0, 0.0)))-this->r;
    };
}


SDFRenderableTorus::SDFRenderableTorus(const Vector3dd &pos, const Vector2dd dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd d = v - this->pos;
        double xzlen = sqrt(d.x()*d.x() + d.z()*d.z()) - this->dim.x();
        return sqrt(xzlen*xzlen + d.y()*d.y()) - this->dim.y();
    };
}

SDFRenderableTorus88::SDFRenderableTorus88(const Vector3dd &pos, const Vector2dd dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd d = v - this->pos;
        Vector2dd t = this->dim;
        Vector2dd q = Vector2dd(length8Vec2dd(Vector2dd(d.x(), d.z())) - t.x(), d.y());
        return length8Vec2dd(q) - t.y();
    };
}

SDFRenderableTorus82::SDFRenderableTorus82(const Vector3dd &pos, const Vector2dd dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd d = v - this->pos;
        Vector2dd t = this->dim;
        Vector2dd q = Vector2dd(lengthVec2dd(Vector2dd(d.x(), d.z())) - t.x(), d.y());
        return length8Vec2dd(q) - t.y();
    };
}

SDFRenderableCone::SDFRenderableCone(const Vector3dd &pos, const Vector3dd dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd p = v - this->pos;
        Vector2dd q = Vector2dd(lengthVec2dd(Vector2dd(p.x(), p.z())), p.y());
        Vector3dd c = this->dim;
        double d1 = -q.y()-c.z();
        double d2 = std::max(dotVec2dd(q,Vector2dd(c.x(),c.y())), q.y());
        return lengthVec2dd(
                    maxVec2dd(Vector2dd(d1,d2),Vector2dd(0.0, 0.0)))
                + std::min(std::max(d1,d2), 0.0);
    };
}

SDFRenderableCylinder::SDFRenderableCylinder(const Vector3dd &pos, const Vector2d<double> dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd d = v - this->pos;
        Vector2dd ne = Vector2d<double>(
                    std::abs(sqrt(d.x() * d.x() + d.z() * d.z())),
                    std::abs(d.y()));
        Vector2dd dd = ne - this->dim;
        return std::min(std::max(dd.x(),dd.y()),0.0) + lengthVec2dd(maxVec2dd(dd,Vector2dd(0.0, 0.0)));
    };
}

SDFRenderableSubstraction::SDFRenderableSubstraction(const SDFRenderable &d1,
                                                     const SDFRenderable &d2):
    d1(d1),
    d2(d2)
{
    this->F = [this](Vector3dd v) {
        return std::max(-this->d2.F(v), this->d1.F(v));
    };
}

SDFRenderableUnion::SDFRenderableUnion(const SDFRenderable &d1,
                                                     const SDFRenderable &d2):
    d1(d1),
    d2(d2)
{
    this->F = [this](Vector3dd v) {
      double d1 = this->d2.F(v);
      double d2 = this->d1.F(v);
      return std::min(d1, d2);
    };
}

SDFRenderableBlend::SDFRenderableBlend(const SDFRenderable &d1,
                                                     const SDFRenderable &d2):
    d1(d1),
    d2(d2)
{
    this->F = [this](Vector3dd v) {
        long double a = this->d2.F(v);
        long double b = this->d1.F(v);
        long double k = 6.00;
        a = pow( a, k );
        b = pow( b, k );
        return pow( ((long double)a * b)/(a + b), 1.0/k );
    };
}
