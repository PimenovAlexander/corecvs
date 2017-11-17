#ifndef PROJECTIONMODELS_H
#define PROJECTIONMODELS_H

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"


namespace corecvs {

class CameraProjection {
public:
    virtual Vector2dd   project(const Vector3dd &p) const = 0;
    virtual Vector3dd   reverse(const Vector2dd &p) const = 0;
    virtual bool      isVisible(const Vector3dd &p) const = 0;

    static double rayToAngle(const Vector3dd &ray)
    {
        return atan2(ray.xy().l2Metric(), ray.z());
    }

    virtual ~CameraProjection();
};



class StereographicProjection : public CameraProjection {
public:
    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    focal;            /**< Focal length */

    StereographicProjection(Vector2dd principal, double focal) :
        principal(principal),
        focal(focal)
    {}

public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {

        double tau = rayToAngle(p);
        Vector2dd dir = p.xy().normalised();
        return dir * 2 * focal * tan(tau / 2);
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal;
        double r = shift.l2Metric();
        shift /= r;
        double tau = 2 * atan(r / 2 / focal);
        return Vector3dd(shift * tau, 1.0).normalised();
    }

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }

    virtual ~StereographicProjection();
};

class EquidistantProjection : public CameraProjection {
public:
    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    focal;            /**< Focal length */

    EquidistantProjection(Vector2dd principal, double focal) :
        principal(principal),
        focal(focal)
    {}

    // CameraProjection interface
public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        double tau = rayToAngle(p);
        Vector2dd dir = p.xy().normalised();
        return dir * focal * tau;
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal;
        double r = shift.l2Metric();
        shift /= r;
        double tau = r / focal;
        return Vector3dd(shift * tau, 1.0).normalised();
    }

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }
};

class EquisolidAngleProjection : public CameraProjection {
public:
    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    focal;            /**< Focal length */

    EquisolidAngleProjection(Vector2dd principal, double focal) :
        principal(principal),
        focal(focal)
    {}



    // CameraProjection interface
public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        double tau = rayToAngle(p);
        Vector2dd dir = p.xy().normalised();
        return dir * 2 * focal * sin(tau / 2);
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal;
        double r = shift.l2Metric();
        shift /= r;
        double tau = 2 * asin(r / 2.0 / focal);
        return Vector3dd(shift * tau, 1.0).normalised();
    }

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }
};

class OrthographicProjection : public CameraProjection {
public:

    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    focal;            /**< Focal length */

    OrthographicProjection(Vector2dd principal, double focal) :
        principal(principal),
        focal(focal)
    {}

    // CameraProjection interface
public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        double tau = rayToAngle(p);
        Vector2dd dir = p.xy().normalised();
        return dir * focal * sin(tau);
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal;
        double r = shift.l2Metric();
        shift /= r;
        double tau = asin(r / focal);
        return Vector3dd(shift * tau, 1.0).normalised();
    }

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }
};



} // namespace corecvs


#endif // PROJECTIONMODELS_H

