#include "core/geometry/raytrace/sdfRenderableObjects.h"

SDFRenderableSphere::SDFRenderableSphere(const Vector3dd &sphere, double r) :
    sphere(sphere),
    r(r)
{
    this->F = [this](Vector3dd v) {
        return (v - this->sphere).l2Metric() - this->r;
    };
}

SDFRenderableEllipsoid::SDFRenderableEllipsoid(const Vector3dd &pos, const Vector3dd &dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd p = v - this->pos;
        Vector3dd r = this->dim;
        return (( p/r ).l2Metric() - 1.0) * std::min(std::min(r.x(),r.y()),r.z());
    };
}


SDFRenderableBox::SDFRenderableBox(const Vector3dd &pos, const Vector3dd dim):
     pos(pos),
     dim(dim)
 {
     this->F = [this](Vector3dd v) {
         Vector3dd d = (v - this->pos).perElementAbs() - this->dim;
         return std::min(std::max(d.x(),std::max(d.y(),d.z())),0.0)
                 + d.perElementMax(Vector3dd::Zero()).l2Metric();
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
        return ((d.perElementAbs() - this->dim).perElementMax(Vector3dd::Zero())).l2Metric() - this->r;
    };
}


SDFRenderableTorus::SDFRenderableTorus(const Vector3dd &pos, const Vector2dd dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd d = v - this->pos;
        double xzlen = d.xz().l2Metric() - this->dim.x();
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
        Vector2dd q = Vector2dd(Vector2dd(d.x(), d.z()).lnMetric<8>() - t.x(), d.y());
        return q.lnMetric<8>() - t.y();
    };
}

SDFRenderableTorus82::SDFRenderableTorus82(const Vector3dd &pos, const Vector2dd dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd d = v - this->pos;
        Vector2dd t = this->dim;
        Vector2dd q = Vector2dd((Vector2dd(d.x(), d.z())).l2Metric() - t.x(), d.y());
        return q.lnMetric<8>() - t.y();
    };
}

SDFRenderableCone::SDFRenderableCone(const Vector3dd &pos, const Vector3dd dim):
    pos(pos),
    dim(dim)
{
    this->F = [this](Vector3dd v) {
        Vector3dd p = v - this->pos;
        Vector2dd q = Vector2dd(Vector2dd(p.x(), p.z()).l2Metric(), p.y());
        Vector3dd c = this->dim;
        double d1 = -q.y()-c.z();
        double d2 = std::max(q & Vector2dd(c.x(),c.y()), q.y());
        return Vector2dd(d1,d2).perElementMax(Vector2dd(0.0, 0.0)).l2Metric()
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
                    std::abs(d.xz().l2Metric()),
                    std::abs(d.y()));
        Vector2dd dd = ne - this->dim;
        return std::min(std::max(dd.x(),dd.y()),0.0) + dd.perElementMax(Vector2dd(0.0, 0.0)).l2Metric();
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
