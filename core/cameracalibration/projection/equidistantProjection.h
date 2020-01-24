#ifndef EQUIDISTANTPROJECTION_H
#define EQUIDISTANTPROJECTION_H

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"
#include "core/function/function.h"

#include "core/cameracalibration/projection/projectionModels.h"
#include "core/xml/generated/projectionBaseParameters.h"

namespace corecvs{
/**
 *   Equidistant projection
 *
 **/
template<class ElementType>
class GenericEquidistantProjection{
public:
    enum {
        PRINCIPAL_X,
        PRINCIPAL_Y,
        FOCAL,
        PARAMTER_LAST
    };


    ElementType parameter[PARAMTER_LAST];

    GenericEquidistantProjection(const ElementType &cx, const ElementType &cy, const ElementType &focal)
    {
        parameter[PRINCIPAL_X] = cx;
        parameter[PRINCIPAL_Y] = cy;
        parameter[FOCAL] = focal;
    }

    /** Principal point of optical axis on image plane (in pixel). Usually center of imager */

    Vector2d<ElementType> principal() const
    {
        return Vector2d<ElementType>(parameter[PRINCIPAL_X], parameter[PRINCIPAL_Y]);
    }

    /** Focal length */
    ElementType focal() const
    {
        return parameter[FOCAL];
    }

    void project(const Vector3d<ElementType> &in, Vector2d<ElementType> &out) const
    {
        ElementType tau = in.angleToZ();
        Vector2d<ElementType> dir = in.xy().normalised();
        out = dir * focal() * tau + principal();
    }

    void reverse(const Vector2d<ElementType> &in, Vector3d<ElementType> &out) const
    {
        Vector2d<ElementType> shift = in - principal();
        ElementType r = shift.l2Metric();
        shift /= r;
        ElementType tau = r / focal();
        out = Vector3d<ElementType>(shift.normalised() * sin(tau), cos(tau));
    }
};


/**
 *
 **/
class EquidistantProjection : public ProjectionBaseParameters, public CameraProjection {
public:
    EquidistantProjection() : CameraProjection(ProjectionType::EQUIDISTANT) {}

    EquidistantProjection(const Vector2dd &principal, double focal, const Vector2dd &size) :
        ProjectionBaseParameters(principal.x(), principal.y(), focal, size.x(), size.y(), size.x(), size.y()),
        CameraProjection(ProjectionType::EQUIDISTANT)
    {

    }

    // CameraProjection interface
public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        Vector2dd res;
        GenericEquidistantProjection<double>(principalX(), principalY(), focal()).project(p, res);
        return res;
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector3dd res;
        GenericEquidistantProjection<double>(principalX(), principalY(), focal()).reverse(p, res);
        return res;
    }

    /**
     *  Optional inteface part that automates projection usage as function
     **/

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
    virtual EquidistantProjection *clone() const
    {
        EquidistantProjection *p = new EquidistantProjection();
        *p = *this;
        return p;
    }

    virtual DynamicObjectWrapper getDynamicWrapper() override
    {
        return DynamicObjectWrapper(getReflection(), static_cast<ProjectionBaseParameters *>(this));
    }

    virtual ~EquidistantProjection() {}
};

}

#endif // EQUIDISTANTPROJECTION_H
