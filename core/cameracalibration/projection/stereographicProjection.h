#ifndef STEREOGRAPHICPROJECTION_H
#define STEREOGRAPHICPROJECTION_H

#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"
#include "function/function.h"

#include "cameracalibration/projection/projectionModels.h"
#include "xml/generated/projectionBaseParameters.h"

namespace corecvs {

class StereographicProjection : public ProjectionBaseParameters, public CameraProjection {
public:
    //Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    //double    focal;            /**< Focal length */


    StereographicProjection();

    StereographicProjection(const Vector2dd &principal, double focal, const Vector2dd &size);

public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {

        double tau = rayToAngle<double>(p);
        Vector2dd dir = p.xy().normalised();
        return dir * 2 * focal() * tan(tau / 2);
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal();
        double r = shift.l2Metric();
        shift /= r;
        double tau = 2 * atan2(r / 2, focal());
        return Vector3dd(shift * tau, 1.0).normalised();
    }

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }

    virtual Vector2dd size() const override
    {
        return  Vector2dd(sizeX(), sizeY());
    }

    virtual Vector2dd distortedSize() const override
    {
        return  Vector2dd(distortedSizeX(), distortedSizeY());
    }

    /*Vector2dd focal() const
    {
        return  Vector2dd(focalX(), focalY());
    }*/

    virtual Vector2dd principal() const override
    {
        return  Vector2dd(principalX(), principalY());
    }

    /* Misc */
    virtual CameraProjection *clone() const
    {
        StereographicProjection *p = new StereographicProjection();
        *p = *this;
        return p;
    }

    virtual DynamicObjectWrapper getDynamicWrapper() override
    {
        return DynamicObjectWrapper(getReflection(), static_cast<ProjectionBaseParameters *>(this));
    }


    virtual ~StereographicProjection();
};

} // namespace corecvs



#endif // STEREOGRAPHICPROJECTION_H
