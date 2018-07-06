#ifndef EQUISOLIDANGLEPROJECTION_H
#define EQUISOLIDANGLEPROJECTION_H

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"
#include "core/function/function.h"

#include "core/cameracalibration/projection/projectionModels.h"
#include "core/xml/generated/projectionBaseParameters.h"

namespace corecvs {
/**
 * Equisolid projection
 *
 * \f[ r=2f\sin(\theta / 2) \f]
 * \f[ \theta = asin (r / (2 * f)) * 2 \f]
 *
 **/

class EquisolidAngleProjection :  public ProjectionBaseParameters, public CameraProjection {
public:
    //Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    //double    focal;            /**< Focal length */

    EquisolidAngleProjection() :
        CameraProjection(ProjectionType::EQUISOLID)
    {}

    EquisolidAngleProjection(const Vector2dd &principal, double focal, const Vector2dd &size) :
        ProjectionBaseParameters(principal.x(), principal.y(), focal, size.x(), size.y(), size.x(), size.y()),
        CameraProjection(ProjectionType::EQUISOLID)
    {}

    // CameraProjection interface
public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        double theta = rayToAngle(p);
        Vector2dd dir = p.xy().normalised();
        return dir * 2 * focal() * sin(theta / 2) + principal();
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal();
        double r = shift.l2Metric();
        shift /= r;
        double theta = 2 * asin(r / 2.0 / focal());
        return Vector3dd(shift.normalised() * sin(theta), cos(theta));
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
    virtual Vector2dd principal() const override
    {
        return  Vector2dd(principalX(), principalY());
    }

    /* Misc */
    virtual EquisolidAngleProjection *clone() const
    {
        EquisolidAngleProjection *p = new EquisolidAngleProjection();
        *p = *this;
        return p;
    }

    virtual DynamicObjectWrapper getDynamicWrapper() override
    {
        return DynamicObjectWrapper(&reflection, static_cast<ProjectionBaseParameters *>(this));
    }

    virtual ~EquisolidAngleProjection() {}
};

} // namespace corecvs
#endif // EQUISOLIDANGLEPROJECTION_H
